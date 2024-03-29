/*
 * Copyright 2013-2015 Christian Loose <christian.loose@hamburg.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "cutemarkedmainwindow.h"
#include "ui_cutemarkedmainwindow.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QDirIterator>
#include <QFileDialog>
#ifndef Q_OS_OSX
#include <QIcon>
#endif
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QPrintDialog>
#include <QPrinter>
#include <QProcess>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include <QTextDocumentWriter>
#include <QTimer>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QWebChannel>
#include <QWebEnginePage>

#ifdef Q_OS_WIN
#include <QWinJumpList>
#include <QWinJumpListCategory>
#endif

#include <cutemarkdownhighlighter.h>
#include <jsonfile.h>
#include <snippets/jsonsnippettranslatorfactory.h>
#include <snippets/snippetcollection.h>
#include <spellchecker/dictionary.h>
#include <themes/stylemanager.h>
#include <themes/themecollection.h>
#include <datalocation.h>
#include "controls/activelabel.h"
#include "controls/findreplacewidget.h"
#include "controls/languagemenu.h"
#include "controls/recentfilesmenu.h"
#include "aboutdialog.h"
#include "htmlpreviewcontroller.h"
#include "htmlpreviewgenerator.h"
#include "htmlviewsynchronizer.h"
#include "htmlhighlighter.h"
#include "imagetooldialog.h"
#include "markdownmanipulator.h"
#include "exporthtmldialog.h"
#include "exportpdfdialog.h"
#include "options.h"
#include "optionsdialog.h"
#include "revealviewsynchronizer.h"
#include "savefileadapter.h"
#include "snippetcompleter.h"
#include "tabletooldialog.h"
#include "statusbarwidget.h"
#include "../fontawesomeicon/fontawesomeiconengine.h"

class MyQWebEnginePage : public QWebEnginePage
{
    Q_OBJECT

private:
    MainWindow *mainWindow;

public:
    MyQWebEnginePage(QObject *parent, MainWindow *mainWindow) : QWebEnginePage(parent), mainWindow(mainWindow) {}

    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
    {
        if (type == QWebEnginePage::NavigationTypeLinkClicked && isMainFrame) {
            mainWindow->previewLinkClicked(url);
            return false;
        }
        return true;
    }
};

MainWindow::MainWindow(const QString &fileName, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    options(new Options(this)),
    stylesGroup(new QActionGroup(this)),
    statusBarWidget(0),
    generator(new HtmlPreviewGenerator(options, this)),
    snippetCollection(new SnippetCollection(this)),
    viewSynchronizer(0),
    htmlPreviewController(0),
    themeCollection(new ThemeCollection()),
    splitFactor(0.5),
    rightViewCollapsed(false),
    channel(new QWebChannel(this))
{
    ui->setupUi(this);
    setupUi();

    setFileName(fileName);

    QTimer::singleShot(0, this, SLOT(initializeApp()));

    previewTimer = new QTimer;
    previewTimer->setSingleShot(false);
    previewTimer->setInterval(1000);
    previewTimer->callOnTimeout(generator, &HtmlPreviewGenerator::updatePreview);
    previewTimer->start();
}

MainWindow::~MainWindow()
{
    delete viewSynchronizer;

    // stop background HTML preview generator
    generator->markdownTextChanged(QString());
    //generator->wait();
    delete generator;

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    // check if file needs saving
    if (maybeSave()) {
        writeSettings();
        e->accept();
    } else {
        e->ignore();
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    updateSplitter();
}

void MainWindow::initializeApp()
{
    ui->webView->setPage(new MyQWebEnginePage(this, this));
    // inform us when a link in the table of contents or preview view is clicked
    // ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    addJavaScriptObject();
    ui->webView->page()->setWebChannel(channel);

    themeCollection->load(QStringLiteral(":/builtin-htmlpreview-themes.json"));
    loadCustomStyles();
    setupHtmlPreviewThemes();

    // apply last used theme
    lastUsedTheme();

    ui->plainTextEdit->tabWidthChanged(options->tabWidth());
    ui->plainTextEdit->rulerEnabledChanged(options->isRulerEnabled());
    ui->plainTextEdit->rulerPosChanged(options->rulerPos());

    // init extension flags
    ui->actionAutolink->setChecked(options->isAutolinkEnabled());
    ui->actionStrikethroughOption->setChecked(options->isStrikethroughEnabled());
    ui->actionUnderline->setChecked(options->isUnderlineEnabled());

    // init option flags
    ui->actionMathSupport->setChecked(options->isMathSupportEnabled());
    ui->actionDiagramSupport->setChecked(options->isDiagramSupportEnabled());
    ui->actionWavedromSupport->setChecked(options->isWavedromSupportEnabled());
    ui->actionCodeHighlighting->setChecked(options->isCodeHighlightingEnabled());
    ui->actionShowSpecialCharacters->setChecked(options->isShowSpecialCharactersEnabled());
    ui->actionWordWrap->setChecked(options->isWordWrapEnabled());
    ui->actionCheckSpelling->setChecked(options->isSpellingCheckEnabled());
    ui->plainTextEdit->setSpellingCheckEnabled(options->isSpellingCheckEnabled());
    ui->actionYamlHeaderSupport->setChecked(options->isYamlHeaderSupportEnabled());

    // set url to markdown syntax help
    //ui->webView_2->setUrl(tr("qrc:/syntax.html"));

    // allow loading of remote javascript
    ui->webView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    //ui->webView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    ui->webView->page()->setInspectedPage(ui->webView->page());
//    QWebInspector *inspector = new QWebInspector();
//    inspector->setPage(ui->webView->page());

    ui->menuLanguages->loadDictionaries(options->dictionaryLanguage());

    //: path to built-in snippets resource.
    JsonFile<Snippet>::load(QStringLiteral(":/markdown-snippets.json"), snippetCollection);
    QString path = DataLocation::writableLocation();
    JsonFile<Snippet>::load(path + "/user-snippets.json", snippetCollection);

    // setup file explorer
    connect(ui->fileExplorerDockContents, SIGNAL(fileSelected(QString)),
            this, SLOT(openRecentFile(QString)));
    connect(options, &Options::explorerPathChanged,
            ui->fileExplorerDockContents, &FileExplorerWidget::onDefaultPathChanged);

    //toc/navigation parser
    connect(ui->plainTextEdit->getHighlighter(), &CuteMarkdownHighlighter::highlightingFinished,
            this, &MainWindow::startNavigationParser);
    connect(ui->navigationWidget, &NavigationWidget::positionClicked,
            this, &MainWindow::onNavigationWidgetPositionClicked);

    // setup jump list on windows
#ifdef Q_OS_WIN
    QWinJumpList jumplist;
    jumplist.recent()->setVisible(true);
#endif

    // load file passed to application on start
    if (!fileName.isEmpty()) {
        load(fileName);
    }
}

void MainWindow::openRecentFile(const QString &fileName)
{
    if (maybeSave()) {
        load(fileName);
    }
}

void MainWindow::languageChanged(const Dictionary &dictionary)
{
    options->setDictionaryLanguage(dictionary.language());
    ui->plainTextEdit->setSpellingDictionary(dictionary);
}

void MainWindow::fileNew()
{
    if (maybeSave()) {
        ui->plainTextEdit->clear();
        ui->plainTextEdit->resetHighlighting();
        ui->webView->setHtml(QString());
        ui->htmlSourceTextEdit->clear();
        setFileName(QString());
    }
}

void MainWindow::fileOpen()
{
    if (maybeSave()) {
        QString name = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                                    QString(), tr("Markdown Files (*.markdown *.md *.mdown);;All Files (*)"));
        if (!name.isEmpty()) {
            load(name);
        }
    }
}

bool MainWindow::fileSave()
{
    // file has no name yet?
    if (fileName.isEmpty()) {
        return fileSaveAs();
    }

    SaveFileAdapter file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextDocumentWriter writer(&file, "plaintext");
    bool success = writer.write(ui->plainTextEdit->document());
    if (success) {
        file.commit();

        // set status to unmodified
        ui->plainTextEdit->document()->setModified(false);
        setWindowModified(false);

        // add to recent file list
        recentFilesMenu->addFile(fileName);
    }

    return success;
}

bool MainWindow::fileSaveAs()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save as..."), ui->fileExplorerDockContents->getPath(),
                                              tr("Markdown Files (*.md *.markdown *.mdown);;All Files (*)"));
    if (name.isEmpty()) {
        return false;
    }

    // Add default extension ".md" if the file name as no extension yet.
    if (QFileInfo(name).suffix().isEmpty()) {
        name.append(QStringLiteral(".md"));
    }

    setFileName(name);
    return fileSave();
}

void MainWindow::fileExportToHtml()
{
    ExportHtmlDialog dialog(fileName);
    if (dialog.exec() == QDialog::Accepted) {

        QString cssStyle;
        if (dialog.includeCSS()) {
            // get url of current css stylesheet
            QUrl cssUrl = StyleManager::previewStylesheetPath(currentTheme);

            // get resource or file name from url
            QString cssFileName;
            if (cssUrl.scheme() == QLatin1String("qrc")) {
                cssFileName = cssUrl.toString().remove(0, 3);
            } else {
                cssFileName = cssUrl.toLocalFile();
            }

            // read currently used css stylesheet file
            QFile f(cssFileName);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                cssStyle = f.readAll();
            }
        }

        QString highlightJs;
        if (dialog.includeCodeHighlighting()) {
            QFile f(QStringLiteral(":/scripts/highlight.js/highlight.pack.js"));
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                highlightJs = f.readAll();
            }
        }

        QString html = generator->exportHtml(cssStyle, highlightJs);

        // write HTML source to disk
        QFile f(dialog.fileName());
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            out.setCodec("UTF-8");
            out << html;
        }
    }
}

void MainWindow::fileExportToPdf()
{
    // using temporary QTextDocument instance to get links exported\printed correctly,
    // as links will dissappear when printing directly from QWebView in current Qt implementation
    // of QWebView::print() method (possible bug in Qt?)
    // more info here: http://stackoverflow.com/questions/11629093/add-working-url-into-pdf-using-qt-qprinter

    ExportPdfDialog dialog(fileName);
    if (dialog.exec() == QDialog::Accepted) {
        auto orientation = dialog.printer()->orientation() == QPrinter::Portrait ? QPageLayout::Portrait : QPageLayout::Landscape;
        auto paperSize = (QPageSize::PageSizeId)dialog.printer()->paperSize();
        const QPageLayout &layout = QPageLayout(QPageSize(paperSize), orientation, QMarginsF());;
        const auto &filename = dialog.printer()->outputFileName();
        ui->webView->page()->printToPdf(filename, layout);
    }
}

void MainWindow::filePrint()
{
    QPrinter printer;
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));

    if (ui->webView->hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);

    if (dlg->exec() == QDialog::Accepted)
//        ui->webView->page()->printToPdf()

    delete dlg;
}

void MainWindow::editUndo()
{
    if (ui->plainTextEdit->document()->isUndoAvailable()) {
        ui->plainTextEdit->document()->undo();
    }
}

void MainWindow::editRedo()
{
    if (ui->plainTextEdit->document()->isRedoAvailable()) {
        ui->plainTextEdit->document()->redo();
    }
}

void MainWindow::editCopyHtml()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(ui->htmlSourceTextEdit->toPlainText());
}

void MainWindow::editGotoLine()
{
    const int STEP = 1;
    const int MIN_VALUE = 1;

    QTextCursor cursor = ui->plainTextEdit->textCursor();
    int currentLine = cursor.blockNumber()+1;
    int maxValue = ui->plainTextEdit->document()->blockCount();

    bool ok;
    int line = QInputDialog::getInt(this, tr("Go to..."),
                                          tr("Line: ", "Line number in the Markdown editor"), currentLine, MIN_VALUE, maxValue, STEP, &ok);
    if (!ok) return;
    ui->plainTextEdit->gotoLine(line);
}

void MainWindow::editFindReplace()
{
    ui->findReplaceWidget->setTextEdit(ui->plainTextEdit);
    ui->findReplaceWidget->show();
    ui->findReplaceWidget->setFocus();
}

void MainWindow::editStrong()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText(QStringLiteral("**"));
}

void MainWindow::editEmphasize()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText(QStringLiteral("*"));
}

void MainWindow::editStrikethrough()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText(QStringLiteral("~~"));
}

void MainWindow::editInlineCode()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapSelectedText(QStringLiteral("`"));
}

void MainWindow::editCenterParagraph()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.wrapCurrentParagraph(QStringLiteral("->"), QStringLiteral("<-"));
}

void MainWindow::editHardLinebreak()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.appendToLine(QStringLiteral("  \n"));
}

void MainWindow::editBlockquote()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.formatTextAsQuote();
}

void MainWindow::editIncreaseHeaderLevel()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.increaseHeadingLevel();
}

void MainWindow::editDecreaseHeaderLevel()
{
    MarkdownManipulator manipulator(ui->plainTextEdit);
    manipulator.decreaseHeadingLevel();
}

void MainWindow::editInsertTable()
{
    TableToolDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        MarkdownManipulator manipulator(ui->plainTextEdit);
        manipulator.insertTable(dialog.rows(), dialog.columns(),
                                dialog.alignments(), dialog.tableCells());
    }
}

void MainWindow::editInsertImage()
{
    ImageToolDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        MarkdownManipulator manipulator(ui->plainTextEdit);
        manipulator.insertImageLink(dialog.alternateText(), dialog.imageSourceLink(), dialog.optionalTitle());
    }
}

void MainWindow::viewChangeSplit()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action->objectName() == ui->actionSplit_1_1->objectName()) {
        splitFactor = 0.5;
    } else if (action->objectName() == ui->actionSplit_2_1->objectName()) {
        splitFactor = 0.666;
    } else if (action->objectName() == ui->actionSplit_1_2->objectName()) {
        splitFactor = 0.333;
    } else if (action->objectName() == ui->actionSplit_3_1->objectName()) {
        splitFactor = 0.75;
    } else if (action->objectName() == ui->actionSplit_1_3->objectName()) {
        splitFactor = 0.25;
    }

    updateSplitter();

    // web view was collapsed and is now visible again, so update it
    if (rightViewCollapsed) {
        syncWebViewToHtmlSource();
    }
}

void MainWindow::lastUsedTheme()
{
    QString themeName = options->lastUsedTheme();

    currentTheme = themeCollection->theme(themeName);
    applyCurrentTheme();
    const auto styleGroupActions = stylesGroup->actions();
    for (const auto action : styleGroupActions) {
        if (action->text() == themeName) {
            action->setChecked(true);
            emit stylesGroup->triggered(action);
            break;
        }
    }
}

void MainWindow::themeChanged()
{
    removeStyleSheet(currentTheme.name(), true);
    QAction *action = qobject_cast<QAction*>(sender());
    QString themeName = action->text();

    currentTheme = themeCollection->theme(themeName);
    applyCurrentTheme();

    options->setLastUsedTheme(themeName);
}

void MainWindow::editorStyleChanged()
{
    QString markdownHighlighting = StyleManager::markdownHighlightingPath(currentTheme);
    ui->plainTextEdit->loadStyleFromStylesheet(stylePath(markdownHighlighting));
}

void MainWindow::applyCurrentTheme()
{
    QString markdownHighlighting = StyleManager::markdownHighlightingPath(currentTheme);
    QString codeHighlighting = StyleManager::codeHighlightingPath(currentTheme);
    QString previewStylesheet = StyleManager::previewStylesheetPath(currentTheme);

    generator->setCodeHighlightingStyle(codeHighlighting);
    ui->plainTextEdit->loadStyleFromStylesheet(stylePath(markdownHighlighting));

    QString stylesheetSource;
    QUrl cssUrl = previewStylesheet;

    // get resource or file name from url
    QString cssFileName;
    if (cssUrl.scheme() == QLatin1String("qrc")) {
        cssFileName = cssUrl.toString().remove(0, 3);
    } else {
        cssFileName = cssUrl.toLocalFile();
    }

    // read currently used css stylesheet file
    QFile f(cssFileName);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        stylesheetSource = f.readAll();
    }

    insertStyleSheet(currentTheme.name(), stylesheetSource, true);
   // ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl(previewStylesheet));
}

void MainWindow::viewFullScreenMode()
{
    if (ui->actionFullScreenMode->isChecked()) {
        showFullScreen();
    } else {
        showNormal();
    }
}

void MainWindow::viewHorizontalLayout(bool checked)
{
    if (checked) {
        ui->splitter->setOrientation(Qt::Vertical);
    } else {
        ui->splitter->setOrientation(Qt::Horizontal);
    }
}

void MainWindow::extrasShowSpecialCharacters(bool checked)
{
    options->setShowSpecialCharactersEnabled(checked);
    ui->plainTextEdit->setShowSpecialCharacters(checked);
}

void MainWindow::extrasYamlHeaderSupport(bool checked)
{
    options->setYamlHeaderSupportEnabled(checked);
    ui->plainTextEdit->setYamlHeaderSupportEnabled(checked);
    plainTextChanged();
}

void MainWindow::extrasWordWrap(bool checked)
{
    options->setWordWrapEnabled(checked);
    ui->plainTextEdit->setLineWrapMode(checked ? MarkdownEditor::WidgetWidth : MarkdownEditor::NoWrap);
}

void MainWindow::extensionsAutolink(bool checked)
{
    options->setAutolinkEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsStrikethrough(bool checked)
{
    options->setStrikethroughEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsAlphabeticLists(bool checked)
{
    options->setAlphabeticListsEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsDefinitionLists(bool checked)
{
    options->setDefinitionListsEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsSmartyPants(bool checked)
{
    options->setSmartyPantsEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsFootnotes(bool enabled)
{
    options->setFootnotesEnabled(enabled);
    plainTextChanged();
}

void MainWindow::extensionsUnderline(bool enabled)
{
    options->setUnderlineEnabled(enabled);
    plainTextChanged();
}

void MainWindow::extrasCheckSpelling(bool checked)
{
    ui->plainTextEdit->setSpellingCheckEnabled(checked);
    options->setSpellingCheckEnabled(checked);
}

void MainWindow::extrasOptions()
{
    QVector<QAction*> actions;
    actions.reserve(sizeof(QAction*) * 38);
    // file menu
    actions << ui->actionNew
            << ui->actionOpen
            << ui->actionSave
            << ui->actionSaveAs
            << ui->actionExportToHTML
            << ui->actionExportToPDF
            << ui->action_Print
            << ui->actionExit;
    // edit menu
    actions << ui->actionUndo
            << ui->actionRedo
            << ui->actionCut
            << ui->actionMyEditorCopy
            << ui->actionPaste
            << ui->actionStrong
            << ui->actionCopyHtmlToClipboard
            << ui->actionEmphasize
            << ui->actionStrikethrough
            << ui->actionInline_Code
            << ui->actionCenterParagraph
            << ui->actionBlockquote
            << ui->actionIncreaseHeaderLevel
            << ui->actionDecreaseHeaderLevel
            << ui->actionInsertTable
            << ui->actionInsertImage
            << ui->actionFindReplace
            << ui->actionFindNext
            << ui->actionFindPrevious
            << ui->actionGotoLine;
    // view menu
    actions << ui->dockWidget->toggleViewAction()
            << ui->fileExplorerDockWidget->toggleViewAction()
            << ui->actionHtmlSource
            << ui->actionSplit_1_1
            << ui->actionSplit_2_1
            << ui->actionSplit_1_2
            << ui->actionSplit_3_1
            << ui->actionSplit_1_3
            << ui->actionFullScreenMode
            << ui->actionHorizontalLayout;

    // snippet complete
    actions << ui->plainTextEdit->actions().toVector();

    OptionsDialog dialog(options, snippetCollection, actions, this);
    if (dialog.exec() == QDialog::Accepted) {
        options->writeSettings();

        QString path = DataLocation::writableLocation();
        QSharedPointer<SnippetCollection> userDefinedSnippets = snippetCollection->userDefinedSnippets();
        JsonFile<Snippet>::save(path + QStringLiteral("/user-snippets.json"), userDefinedSnippets.data());

        // update shortcuts
        setupCustomShortcuts();
    }
}

void MainWindow::helpMarkdownSyntax()
{
    ui->dockWidget_2->show();
}

void MainWindow::helpAbout()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::setHtmlSource(bool enabled)
{
    if (enabled) {
        ui->stackedWidget->setCurrentWidget(ui->htmlSourcePage);

        // activate HTML highlighter
        htmlHighlighter->setEnabled(true);
        htmlHighlighter->rehighlight();
    } else {
        ui->stackedWidget->setCurrentWidget(ui->webViewPage);

        // deactivate HTML highlighter
        htmlHighlighter->setEnabled(false);

        // update webView now since it was not updated while hidden
        syncWebViewToHtmlSource();
    }

    // sync view menu action
    if (ui->actionHtmlSource->isChecked() != enabled)
        ui->actionHtmlSource->setChecked(enabled);

    updateSplitter();
}

void MainWindow::plainTextChanged()
{
    QString code = ui->plainTextEdit->toPlainText();

    // generate HTML from markdown
    generator->markdownTextChanged(code);

    // show modification indicator in window title
    setWindowModified(ui->plainTextEdit->document()->isModified());
}

void MainWindow::htmlResultReady(const QString &html)
{
    // show html preview
    QUrl baseUrl;
    if (fileName.isEmpty()) {
        baseUrl = QUrl::fromLocalFile(qApp->applicationDirPath());
    } else {
        baseUrl = QUrl::fromLocalFile(QFileInfo(fileName).absolutePath() + "/");
    }

    QList<int> childSizes = ui->splitter->sizes();
    if (previewHtml != html) {
        if (ui->webView->isVisible() && childSizes[1] != 0) {
            previewHtml = html;
            ui->webView->setHtml(html, baseUrl);
        }

        // show html source
        ui->htmlSourceTextEdit->setPlainText(html);
    }
}

/**
 * Starts the parsing for the navigation widget
 */
void MainWindow::startNavigationParser() {
    if (ui->navigationWidget->isVisible()) {
        ui->navigationWidget->parse(ui->plainTextEdit->document());
    }
}

void MainWindow::onNavigationWidgetPositionClicked(int position)
{
    auto textEdit = ui->plainTextEdit;

    // set the focus first so the preview also scrolls to the headline
    textEdit->setFocus();

    QTextCursor c = textEdit->textCursor();

    // if the current position of the cursor is smaller than the position
    // where we want to jump to set the cursor to the end of the note to make
    // sure it scrolls up, not down
    // everything is visible that way
    if (c.position() < position) {
        c.movePosition(QTextCursor::End);
        textEdit->setTextCursor(c);
    }

    c.setPosition(position);

    // select the text of the headline
    c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    textEdit->setTextCursor(c);

//    // update the preview-slider
//    noteTextSliderValueChanged(textEdit->verticalScrollBar()->value(), true);
}

void MainWindow::previewLinkClicked(const QUrl &url)
{
    if (url.isLocalFile()) {
        // directories are not supported
        if (QFileInfo(url.toLocalFile()).isDir()) return;

        QString filePath = url.toLocalFile();
        // Links to markdown files open new instance
        if (filePath.endsWith(QLatin1String(".md")) || filePath.endsWith(QLatin1String(".markdown")) || filePath.endsWith(QLatin1String(".mdown"))) {
            QProcess::startDetached(qApp->applicationFilePath(), QStringList() << filePath);
            return;
        }
    }

    QDesktopServices::openUrl(url);
}

void MainWindow::splitterMoved(int pos, int index)
{
    Q_UNUSED(index)

    int maxViewWidth = ui->splitter->size().width() - ui->splitter->handleWidth();
    splitFactor = (float)pos / maxViewWidth;

    // web view was collapsed and is now visible again, so update it
    if (rightViewCollapsed && ui->splitter->sizes().at(1) > 0) {
        syncWebViewToHtmlSource();
    }
    rightViewCollapsed = (ui->splitter->sizes().at(1) == 0);
}

void MainWindow::addJavaScriptObject()
{
    channel->registerObject(QStringLiteral("synchronizer"), viewSynchronizer);
    // add view synchronizer object to javascript engine
    //ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("synchronizer", viewSynchronizer);
}

bool MainWindow::load(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        return false;
    }

    // open file
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
        return false;
    }

    // read content from file
    QByteArray content = file.readAll();
    QString text = QString::fromUtf8(content);

    ui->plainTextEdit->resetHighlighting();
    ui->plainTextEdit->setPlainText(text);

    QSettings settings;
    bool willExplorerUseCurrentFilePath = settings.value(QStringLiteral("General/willUseCurrentFilePath"), true).toBool();
    if (willExplorerUseCurrentFilePath) {
        QFileInfo fileInfo(fileName);
        ui->fileExplorerDockContents->setPath(fileInfo.absoluteDir().path());
    }

    // remember name of new file
    setFileName(fileName);

    // add to recent files
    recentFilesMenu->addFile(fileName);

    return true;
}

void MainWindow::proxyConfigurationChanged()
{
    if (options->proxyMode() == Options::SystemProxy) {
        qDebug() << "Use system proxy configuration";
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    } else if (options->proxyMode() == Options::ManualProxy) {
        qDebug() << "Use proxy" << options->proxyHost();
        QNetworkProxyFactory::setUseSystemConfiguration(false);

        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(options->proxyHost());
        proxy.setPort(options->proxyPort());
        proxy.setUser(options->proxyUser());
        proxy.setPassword(options->proxyPassword());
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        qDebug() << "Don't use a proxy";
        QNetworkProxyFactory::setUseSystemConfiguration(false);

        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::NoProxy);
        QNetworkProxy::setApplicationProxy(proxy);
    }
}

void MainWindow::markdownConverterChanged()
{
    // regenerate HTML
    plainTextChanged();

    // disable unsupported extensions
    updateExtensionStatus();
    if (viewSynchronizer)
        delete viewSynchronizer;
    switch (options->markdownConverter()) {
    case Options::MD4CMarkdownConverter:
        viewSynchronizer = new HtmlViewSynchronizer(ui->webView, ui->plainTextEdit);
        connect(generator, SIGNAL(htmlResultReady(QString)),
                viewSynchronizer, SLOT(rememberScrollBarPos()));
        break;
    case Options::RevealMarkdownConverter:
        viewSynchronizer = new RevealViewSynchronizer(ui->webView, ui->plainTextEdit);
        break;
    default:
        viewSynchronizer = 0;
        break;
    }
}

void MainWindow::setupUi()
{
    htmlPreviewController = new HtmlPreviewController(ui->webView, this);

    setupActions();
    setupMarkdownEditor();
    setupHtmlPreview();
    setupHtmlSourceView();
    setupStatusBar();

    // hide find/replace widget on startup
    ui->findReplaceWidget->hide();
    connect(ui->findReplaceWidget, SIGNAL(dialogClosed()),
            ui->plainTextEdit, SLOT(setFocus()));

    // close table of contents dockwidget
    ui->dockWidget->close();

    // hide markdown syntax help dockwidget
    ui->dockWidget_2->hide();
    ui->dockWidget_2->setFloating(true);
    ui->dockWidget_2->resize(550, 400);

    // show HTML preview on right panel
    setHtmlSource(ui->actionHtmlSource->isChecked());

    connect(options, SIGNAL(proxyConfigurationChanged()),
            this, SLOT(proxyConfigurationChanged()));
    connect(options, SIGNAL(markdownConverterChanged()),
            this, SLOT(markdownConverterChanged()));
    connect(options, &Options::editorStyleChanged,
            this, &MainWindow::editorStyleChanged);

    readSettings();
    setupCustomShortcuts();

    ui->actionFullScreenMode->setChecked(this->isFullScreen());
}

void SetActionShortcut(QAction *action, const QKeySequence &shortcut)
{
    action->setShortcut(shortcut);
    action->setProperty("defaultshortcut", shortcut);
}

void MainWindow::setupActions()
{
    // file menu
    SetActionShortcut(ui->actionNew, QKeySequence::New);
    SetActionShortcut(ui->actionOpen, QKeySequence::Open);
    SetActionShortcut(ui->actionSave, QKeySequence::Save);
    SetActionShortcut(ui->actionSaveAs, QKeySequence::SaveAs);
    SetActionShortcut(ui->action_Print, QKeySequence::Print);
    SetActionShortcut(ui->actionExit, QKeySequence::Quit);

    recentFilesMenu = new RecentFilesMenu(ui->menuFile);
    ui->menuFile->insertMenu(ui->actionSave, recentFilesMenu);

    connect(recentFilesMenu, SIGNAL(recentFileTriggered(QString)),
            this, SLOT(openRecentFile(QString)));

    // edit menu
    SetActionShortcut(ui->actionUndo, QKeySequence::Undo);
    SetActionShortcut(ui->actionRedo, QKeySequence::Redo);

    // TODO： Do we want to disable global Ctrl-V shortcut too?
    SetActionShortcut(ui->actionCut, QKeySequence::Cut);

    // We disable global Ctrl-C shortcut, to enable Ctrl-C on HTML preview

    // We have to change the name to actionMyEditorCopy.
    // Action named actionCopy will be automatically assigned shortcut Ctrl-C,
    // which is not what we want
    // SetActionShortcut(ui->actionMyEditorCopy, QKeySequence::Copy);

    SetActionShortcut(ui->actionPaste, QKeySequence::Paste);

    SetActionShortcut(ui->actionStrong, QKeySequence::Bold);
    SetActionShortcut(ui->actionEmphasize, QKeySequence::Italic);

    SetActionShortcut(ui->actionFindReplace, QKeySequence::Find);
    SetActionShortcut(ui->actionFindNext, QKeySequence::FindNext);
    SetActionShortcut(ui->actionFindPrevious, QKeySequence::FindPrevious);

    connect(ui->actionFindNext, SIGNAL(triggered()),
            ui->findReplaceWidget, SLOT(findNextClicked()));

    connect(ui->actionFindPrevious, SIGNAL(triggered()),
            ui->findReplaceWidget, SLOT(findPreviousClicked()));

    // view menu
    ui->menuView->insertAction(ui->menuView->actions()[0], ui->dockWidget->toggleViewAction());
    ui->menuView->insertAction(ui->menuView->actions()[1], ui->fileExplorerDockWidget->toggleViewAction());
    SetActionShortcut(ui->fileExplorerDockWidget->toggleViewAction(), QKeySequence(Qt::ALT + Qt::Key_E));
    SetActionShortcut(ui->actionFullScreenMode, QKeySequence::FullScreen);

    // extras menu
    connect(ui->actionMathSupport, SIGNAL(triggered(bool)),
            generator, SLOT(setMathSupportEnabled(bool)));
    connect(ui->actionDiagramSupport, SIGNAL(triggered(bool)),
            generator, SLOT(setDiagramSupportEnabled(bool)));
    connect(ui->actionWavedromSupport, SIGNAL(triggered(bool)),
            generator, SLOT(setWavedromSupportEnabled(bool)));
    connect(ui->actionCodeHighlighting, SIGNAL(triggered(bool)),
            generator, SLOT(setCodeHighlightingEnabled(bool)));
    connect(ui->menuLanguages, SIGNAL(languageTriggered(Dictionary)),
            this, SLOT(languageChanged(Dictionary)));

    // help menu
    ui->actionMarkdownSyntax->setShortcut(QKeySequence::HelpContents);

    // set actions icons
    setActionsIcons();

    // set names for dock widget actions
    ui->dockWidget->toggleViewAction()->setObjectName(QStringLiteral("actionTableOfContents"));
    ui->fileExplorerDockWidget->toggleViewAction()->setObjectName(QStringLiteral("actionFileExplorer"));

    // setup default shortcuts
    ui->actionGotoLine->setProperty("defaultshortcut", ui->actionGotoLine->shortcut());
    ui->actionBlockquote->setProperty("defaultshortcut", ui->actionBlockquote->shortcut());
    ui->actionIncreaseHeaderLevel->setProperty("defaultshortcut", ui->actionIncreaseHeaderLevel->shortcut());
    ui->actionDecreaseHeaderLevel->setProperty("defaultshortcut", ui->actionDecreaseHeaderLevel->shortcut());
    ui->actionInsertTable->setProperty("defaultshortcut", ui->actionInsertTable->shortcut());
    ui->actionInsertImage->setProperty("defaultshortcut", ui->actionInsertImage->shortcut());
    ui->dockWidget->toggleViewAction()->setProperty("defaultshortcut", ui->dockWidget->toggleViewAction()->shortcut());
    ui->fileExplorerDockWidget->toggleViewAction()->setProperty("defaultshortcut", ui->fileExplorerDockWidget->toggleViewAction()->shortcut());
    ui->actionHtmlSource->setProperty("defaultshortcut", ui->actionHtmlSource->shortcut());
}

void MainWindow::setActionsIcons()
{

#ifndef Q_OS_OSX
  // file menu
  ui->actionSave->setIcon(QIcon("fa-floppy-o.fontawesome"));
  ui->actionExportToPDF->setIcon(QIcon("fa-file-pdf-o.fontawesome"));
  ui->action_Print->setIcon(QIcon("fa-print.fontawesome"));

  // edit menu
  ui->actionUndo->setIcon(QIcon("fa-undo.fontawesome"));
  ui->actionRedo->setIcon(QIcon("fa-repeat.fontawesome"));

  ui->actionCut->setIcon(QIcon("fa-scissors.fontawesome"));
  ui->actionMyEditorCopy->setIcon(QIcon("fa-files-o.fontawesome"));
  ui->actionPaste->setIcon(QIcon("fa-clipboard.fontawesome"));

  ui->actionStrong->setIcon(QIcon("fa-bold.fontawesome"));
  ui->actionEmphasize->setIcon(QIcon("fa-italic.fontawesome"));
  ui->actionStrikethrough->setIcon(QIcon("fa-strikethrough.fontawesome"));
  ui->actionCenterParagraph->setIcon(QIcon("fa-align-center.fontawesome"));
  ui->actionIncreaseHeaderLevel->setIcon(QIcon("fa-level-up.fontawesome"));
  ui->actionBlockquote->setIcon(QIcon("fa-quote-left.fontawesome"));
  ui->actionDecreaseHeaderLevel->setIcon(QIcon("fa-level-down.fontawesome"));

  ui->actionInsertTable->setIcon(QIcon("fa-table.fontawesome"));
  ui->actionInsertImage->setIcon(QIcon("fa-picture-o.fontawesome"));

  ui->actionFindReplace->setIcon(QIcon("fa-search.fontawesome"));

  // view menu
  ui->actionFullScreenMode->setIcon(QIcon("fa-arrows-alt.fontawesome"));
  ui->webView->pageAction(QWebEnginePage::Copy)->setIcon(QIcon("fa-copy.fontawesome"));
#endif
}

void MainWindow::setupStatusBar()
{
    statusBarWidget = new StatusBarWidget(ui->plainTextEdit);
    statusBarWidget->setHtmlAction(ui->actionHtmlSource);

    connect(options, &Options::lineColumnEnabledChanged,
            statusBarWidget, &StatusBarWidget::showLineColumn);

    statusBarWidget->update();

    // remove border around statusbar widgets
    statusBar()->setStyleSheet(QStringLiteral("QStatusBar::item { border: 0px solid black }; "));
    statusBar()->addPermanentWidget(statusBarWidget, 1);
}

void MainWindow::setupMarkdownEditor()
{
    ui->plainTextEdit->setSnippetCompleter(new SnippetCompleter(snippetCollection, ui->plainTextEdit));

    // load file that are dropped on the editor
    connect(ui->plainTextEdit, SIGNAL(loadDroppedFile(QString)),
            this, SLOT(load(QString)));

    connect(options, &Options::editorFontChanged,
            ui->plainTextEdit, &MarkdownEditor::editorFontChanged);
    connect(options, &Options::tabWidthChanged,
            ui->plainTextEdit, &MarkdownEditor::tabWidthChanged);
    connect(options, &Options::rulerEnabledChanged,
            ui->plainTextEdit, &MarkdownEditor::rulerEnabledChanged);
    connect(options, &Options::rulerPosChanged,
            ui->plainTextEdit, &MarkdownEditor::rulerPosChanged);
}

void MainWindow::setupHtmlPreview()
{
    // add our objects everytime JavaScript environment is cleared
//    connect(ui->webView->page(), SIGNAL(javaScriptWindowObjectCleared()),
//            this, SLOT(addJavaScriptObject()));s()

    // start background HTML preview generator
    connect(generator, SIGNAL(htmlResultReady(QString)),
            this, SLOT(htmlResultReady(QString)));
}

void MainWindow::setupHtmlSourceView()
{
    QFont font(QStringLiteral("Monospace"), 10);
    font.setStyleHint(QFont::TypeWriter);
    ui->htmlSourceTextEdit->setFont(font);
    htmlHighlighter = new HtmlHighlighter(ui->htmlSourceTextEdit->document());
}

void MainWindow::setupCustomShortcuts()
{
    setCustomShortcut(ui->menuFile);
    setCustomShortcut(ui->menuEdit);
    setCustomShortcut(ui->menuView);

    const auto plainTextEditActions = ui->plainTextEdit->actions();
    for (const auto action : plainTextEditActions) {
        setCustomShortcut(action);
    }
}

void MainWindow::setCustomShortcut(QMenu *menu)
{
    const auto menuActions = menu->actions();
    for (const auto action : menuActions) {
        if (action->menu()) {
            // recurse into submenu
            setCustomShortcut(action->menu());
        } else {
            setCustomShortcut(action);
        }
    }
}

void MainWindow::setCustomShortcut(QAction *action)
{
    if (options->hasCustomShortcut(action->objectName())) {
        action->setShortcut(options->customShortcut(action->objectName()));
    }
}

void MainWindow::updateExtensionStatus()
{
    ui->actionAutolink->setEnabled(generator->isSupported(MarkdownConverter::AutolinkOption));
    ui->actionAlphabeticLists->setEnabled(generator->isSupported(MarkdownConverter::NoAlphaListOption));
    ui->actionUnderline->setEnabled(generator->isSupported(MarkdownConverter::UnderlineOption));
}

void MainWindow::syncWebViewToHtmlSource()
{
    htmlResultReady(ui->htmlSourceTextEdit->toPlainText());
}

bool MainWindow::maybeSave()
{
    if (!ui->plainTextEdit->document()->isModified())
        return true;

    if (fileName.startsWith(QLatin1String(":/")))
        return true;

    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Save Changes"),
                               tr("The document has been modified.<br>"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;

    return true;
}

void MainWindow::setFileName(const QString &fileName)
{
    this->fileName = fileName;

    // set to unmodified
    ui->plainTextEdit->document()->setModified(false);
    setWindowModified(false);

    // update window title
    QString shownName = fileName;
    if (shownName.isEmpty()) {
        //: default file name for new markdown documents
        shownName = tr("untitled.md");
    }
    setWindowFilePath(shownName);
}

void MainWindow::updateSplitter()
{
    // not fully initialized yet?
    if (centralWidget()->size() != ui->splitter->size()) {
        return;
    }

    // calculate new width of left and right pane
    QList<int> childSizes = ui->splitter->sizes();
    childSizes[0] = ui->splitter->width() * splitFactor;
    childSizes[1] = ui->splitter->width() * (1 - splitFactor);

    ui->splitter->setSizes(childSizes);
}

void MainWindow::setupHtmlPreviewThemes()
{
    ui->menuStyles->clear();

    delete stylesGroup;
    stylesGroup = new QActionGroup(this);

    int key = 1;
    bool separatorAdded = false;
    const auto themeNames = themeCollection->themeNames();
    for (const QString &themeName : themeNames) {
        if (!separatorAdded && !themeCollection->theme(themeName).isBuiltIn()) {
            addSeparatorAfterBuiltInThemes();
            separatorAdded = true;
        }

        QAction *action = ui->menuStyles->addAction(themeName);
        action->setShortcut(QKeySequence(tr("Ctrl+%1").arg(key++)));
        action->setCheckable(true);
        action->setActionGroup(stylesGroup);
        connect(action, &QAction::triggered,
                this, &MainWindow::themeChanged);
    }

    if (statusBarWidget)
        statusBarWidget->setStyleActions(stylesGroup);
}

void MainWindow::addSeparatorAfterBuiltInThemes()
{
    ui->menuStyles->addSeparator();

    QAction *separator = new QAction(stylesGroup);
    separator->setSeparator(true);
    stylesGroup->addAction(separator);
}

void MainWindow::loadCustomStyles()
{
    QStringList paths = DataLocation::standardLocations();
    qDebug() << paths;
    QDir dataPath(paths.first() + QDir::separator() + QStringLiteral("styles"));
    dataPath.setFilter(QDir::Files);
    if (dataPath.exists()) {
        // iterate over all files in the styles subdirectory
        QDirIterator it(dataPath);
        while (it.hasNext()) {
            it.next();

            QString fileName = it.fileName();
            QString styleName = QFileInfo(fileName).baseName();
            QString stylePath = QUrl::fromLocalFile(it.filePath()).toString();

            Theme customTheme { styleName, QStringLiteral("Default"), QStringLiteral("Default"), styleName };
            themeCollection->insert(customTheme);

            StyleManager styleManager;
            styleManager.insertCustomPreviewStylesheet(styleName, stylePath);
        }
    }
}

void MainWindow::readSettings()
{
    // restore window size, position and state
    QSettings settings;
    restoreGeometry(settings.value(QStringLiteral("mainWindow/geometry")).toByteArray());
    restoreState(settings.value(QStringLiteral("mainWindow/windowState")).toByteArray());

    // restore recent files menu
    recentFilesMenu->readState();

    options->readSettings();
}

void MainWindow::writeSettings()
{
    options->writeSettings();

    // save recent files menu
    recentFilesMenu->saveState();

    // save window size, position and state
    QSettings settings;
    settings.setValue(QStringLiteral("mainWindow/geometry"), saveGeometry());
    settings.setValue(QStringLiteral("mainWindow/windowState"), saveState());
}

QString MainWindow::stylePath(const QString &styleName)
{
//    QString suffix = options->isSourceAtSingleSizeEnabled() ? QLatin1String("") : QLatin1String("+");
    return QStringLiteral("%1").arg(styleName);
}

void MainWindow::insertStyleSheet(const QString &name, const QString &source, bool immediately)
{
    QWebEngineScript script;
    QString s = QString::fromLatin1("(function() {"\
                                    "    css = document.createElement('style');"\
                                    "    css.type = 'text/css';"\
                                    "    css.id = '%1';"\
                                    "    document.head.appendChild(css);"\
                                    "    css.innerText = '%2';"\
                                    "})()").arg(name, source.simplified());
    if (immediately)
        ui->webView->page()->runJavaScript(s, QWebEngineScript::ApplicationWorld);

    script.setName(name);
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::ApplicationWorld);
    ui->webView->page()->scripts().insert(script);
}

void MainWindow::removeStyleSheet(const QString &name, bool immediately)
{
    QString s = QString::fromLatin1("(function() {"\
                                    "    var element = document.getElementById('%1');"\
                                    "    element.outerHTML = '';"\
                                    "    delete element;"\
                                    "})()").arg(name);
    if (immediately)
        ui->webView->page()->runJavaScript(s, QWebEngineScript::ApplicationWorld);

    QWebEngineScript script = ui->webView->page()->scripts().findScript(name);
    ui->webView->page()->scripts().remove(script);
}

#include "cutemarkedmainwindow.moc"
