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

void CuteMarkdownHighlighter::setSpellingCheckEnabled(bool enabled)
{
   spellingCheckEnabled = enabled;
}

void CuteMarkdownHighlighter::setYamlHeaderSupportEnabled(bool enabled)
{
    yamlHeaderSupportEnabled = enabled;
}

void CuteMarkdownHighlighter::highlightBlock(const QString &textBlock)
{
    if (textBlock.isEmpty()) {
        return;
    }

    MarkdownHighlighter::highlightBlock(textBlock);

//    // check spelling of passed text block
    if (spellingCheckEnabled) {
        checkSpelling(textBlock);
    }

}

void CuteMarkdownHighlighter::checkSpelling(const QString &textBlock)
{
    const QStringList wordList = textBlock.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    int index = 0;
    for (const QString &word : wordList) {
        index = textBlock.indexOf(word, index);

        if (!spellChecker->isCorrect(word)) {
            setFormat(index, word.length(), spellFormat);
        }
        index += word.length();
    }
}
