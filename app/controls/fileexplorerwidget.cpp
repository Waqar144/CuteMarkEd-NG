#include "fileexplorerwidget.h"
#include "ui_fileexplorerwidget.h"

#include <QDebug>
#include <QFileSystemModel>
#include <QMenu>
#include <QSettings>
#include <QSortFilterProxyModel>

class FileSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    FileSortFilterProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const
    {
        QFileSystemModel* model = static_cast<QFileSystemModel*>(this->sourceModel());

        QFileInfo leftInfo  = model->fileInfo(left);
        QFileInfo rightInfo = model->fileInfo(right);

        if (leftInfo.isDir() == rightInfo.isDir())
            return (leftInfo.filePath().compare(rightInfo.filePath(), Qt::CaseInsensitive) < 0);

        return leftInfo.isDir();
    }
};


FileExplorerWidget::FileExplorerWidget(QWidget *parent) :
    QWidget(parent),
    initialized(false),
    ui(new Ui::FileExplorerWidget),
    model(new QFileSystemModel(this)),
    sortModel(new FileSortFilterProxyModel(this))
{
    ui->setupUi(this);

    //only show .md and .txt files
    model->setNameFilters({ "*.txt", "*.md" });
    model->setNameFilterDisables(false);

    ui->fileTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fileTreeView, &QTreeView::customContextMenuRequested,
        this, &FileExplorerWidget::oncustomContextMenuRequested);
    connect(ui->fileTreeView, &QTreeView::doubleClicked, this, &FileExplorerWidget::onDoubleClick);

    sortModel->setDynamicSortFilter(true);
    sortModel->setSourceModel(model);

    ui->fileTreeView->setModel(model);
    ui->fileTreeView->hideColumn(1);
    ui->fileTreeView->hideColumn(3);
    ui->fileTreeView->setColumnWidth(0, 150);
    ui->fileTreeView->sortByColumn(0, Qt::AscendingOrder);

    QSettings settings;
    QString path = settings.value(QStringLiteral("General/explorerPath"), QDir::homePath()).toString();
    setPath(path);

    connect(ui->fileTreeView, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(fileOpen(QModelIndex)));
}

FileExplorerWidget::~FileExplorerWidget()
{
    delete ui;
}

void FileExplorerWidget::showEvent(QShowEvent *event)
{
    if (!initialized) {
        QSettings settings;
        model->setRootPath("");
        QString path = settings.value(QStringLiteral("General/explorerPath"), QDir::homePath()).toString();
        setPath(path);
        initialized = true;
    }
    QWidget::showEvent(event);
}

void FileExplorerWidget::setPath(const QString &path)
{
    ui->fileTreeView->setRootIndex(model->index(path));
}

void FileExplorerWidget::fileOpen(const QModelIndex &index)
{
    QFileInfo info = model->fileInfo(index);
    if (info.isFile()) {
        const QString filePath = info.filePath();

        emit fileSelected(filePath);
    }
}

void FileExplorerWidget::onDoubleClick(const QModelIndex& index)
{
    if (model->isDir(index)) {
        ui->fileTreeView->setRootIndex(index);
    }
}

void FileExplorerWidget::oncustomContextMenuRequested(const QPoint p)
{
    QMenu menu(this);
    auto goUp = menu.addAction(tr("Go up"));

    auto execAction = menu.exec(mapToGlobal(p));
    if (execAction == goUp) {
        ui->fileTreeView->setRootIndex(ui->fileTreeView->rootIndex().parent());
    }
}
