/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 * Copyright 2015 Aetf <7437103@gmail.com>
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
#include "cutemarkdownhighlighter.h"

#include <QDebug>
#include <QFile>
#include <QTextDocument>
#include <QTextLayout>

#include "yamlheaderchecker.h"

#include "hunspell/spellchecker.h"
using hunspell::SpellChecker;

#include <QDebug>

CuteMarkdownHighlighter::CuteMarkdownHighlighter(QTextDocument *document, hunspell::SpellChecker *spellChecker) :
    MarkdownHighlighter(document),
    spellingCheckEnabled(false),
    yamlHeaderSupportEnabled(false)
{
    this->spellChecker = spellChecker;

    // QTextCharFormat::SpellCheckUnderline has issues with Qt 5.
    spellFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    spellFormat.setUnderlineColor(Qt::red);
}

//void CuteMarkdownHighlighter::reset()
//{
//    previousText.clear();
//}

//void CuteMarkdownHighlighter::setStyles(const QVector<PegMarkdownHighlight::HighlightingStyle> &styles)
//{
//    highlightingStyles = styles;
//    reset();
//}

//void CuteMarkdownHighlighter::setSpellingCheckEnabled(bool enabled)
//{
//   spellingCheckEnabled = enabled;
//}

//void CuteMarkdownHighlighter::setYamlHeaderSupportEnabled(bool enabled)
//{
//    yamlHeaderSupportEnabled = enabled;
//}

//void CuteMarkdownHighlighter::highlightBlock(const QString &textBlock)
//{
//    if (document()->isEmpty()) {
//        return;
//    }

//    // check spelling of passed text block
//    if (spellingCheckEnabled) {
//        checkSpelling(textBlock);
//    }

//    QString text = document()->toPlainText();

//    // document changed since last call?
//    if (text == previousText) {
//        return;
//    }

//    // cut YAML headers
//    QString actualText;
//    unsigned long offset = 0;
//    if (yamlHeaderSupportEnabled) {
//        YamlHeaderChecker checker(text);
//        actualText = checker.body();
//        offset = checker.bodyOffset();
//    } else {
//        actualText = text;
//    }

//    workerThread->enqueue(actualText, offset);

//    previousText = text;
//}

//void CuteMarkdownHighlighter::applyFormat(unsigned long pos, unsigned long end,
//                                      QTextCharFormat format, bool merge)
//{
//    // The QTextDocument contains an additional single paragraph separator (unicode 0x2029).
//    // https://bugreports.qt-project.org/browse/QTBUG-4841
//    unsigned long max_offset = document()->characterCount() - 1;

//    if (end <= pos || max_offset < pos) {
//        return;
//    }

//    if (max_offset < end) {
//        end = max_offset;
//    }

//    // "The QTextLayout object can only be modified from the
//    // documentChanged implementation of a QAbstractTextDocumentLayout
//    // subclass. Any changes applied from the outside cause undefined
//    // behavior." -- we are breaking this rule here. There might be
//    // a better (more correct) way to do this.

//    int startBlockNum = document()->findBlock(pos).blockNumber();
//    int endBlockNum = document()->findBlock(end).blockNumber();
//    for (int j = startBlockNum; j <= endBlockNum; j++)
//    {
//        QTextBlock block = document()->findBlockByNumber(j);

//        QTextLayout *layout = block.layout();
//        int blockpos = block.position();
//        QTextLayout::FormatRange r;
//        r.format = format;
//        QList<QTextLayout::FormatRange> list;
//        if (merge) {
//            list = layout->additionalFormats();
//        }

//        if (j == startBlockNum) {
//            r.start = pos - blockpos;
//            r.length = (startBlockNum == endBlockNum)
//                        ? end - pos
//                        : block.length() - r.start;
//        } else if (j == endBlockNum) {
//            r.start = 0;
//            r.length = end - blockpos;
//        } else {
//            r.start = 0;
//            r.length = block.length();
//        }

//        list.append(r);
//        layout->setAdditionalFormats(list);
//    }
//}

//void CuteMarkdownHighlighter::checkSpelling(const QString &textBlock)
//{
//    QStringList wordList = textBlock.split(QRegExp("\\W+"), QString::SkipEmptyParts);
//    int index = 0;
//    foreach (QString word, wordList) {
//        index = textBlock.indexOf(word, index);

//        if (!spellChecker->isCorrect(word)) {
//            setFormat(index, word.length(), spellFormat);
//        }
//        index += word.length();
//    }
//}
