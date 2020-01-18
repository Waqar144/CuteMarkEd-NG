/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
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
#include "htmlhighlighter.h"

HtmlHighlighter::HtmlHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document),
    enabled(false)
{
    keywordFormat.setForeground(QColor("#6c71c4"));
    keywordFormat.setFontWeight(QFont::Bold);

    imageFormat.setForeground(QColor("#cf009a"));

    linkFormat.setForeground(QColor("#4e279a"));

    HighlightingRule rule;

    QString htmlTagRegExp = QStringLiteral("<(/?)(%1)[^>]*(/?)>");
    const QStringList keywords = QStringList()
             << htmlTagRegExp.arg(QStringLiteral("html"))
             << QStringLiteral("<head>") << QStringLiteral("</head>")
             << htmlTagRegExp.arg(QStringLiteral("link"))
             << htmlTagRegExp.arg(QStringLiteral("script"))
             << QStringLiteral("<body>") << QStringLiteral("</body>")
             << QStringLiteral("<title>") << QStringLiteral("</title>")
             << QStringLiteral("<b>") << QStringLiteral("</b>")
             << htmlTagRegExp.arg(QStringLiteral("p"))
             << QStringLiteral("<i>") << QStringLiteral("</i>")
             << QStringLiteral("<u>") << QStringLiteral("</u>")
             << QStringLiteral("<sup>") << QStringLiteral("</sup>")
             << QStringLiteral("<sub>") << QStringLiteral("</sub>")
             << htmlTagRegExp.arg(QStringLiteral("h1"))
             << htmlTagRegExp.arg(QStringLiteral("h2"))
             << htmlTagRegExp.arg(QStringLiteral("h3"))
             << htmlTagRegExp.arg(QStringLiteral("h4"))
             << htmlTagRegExp.arg(QStringLiteral("h5"))
             << htmlTagRegExp.arg(QStringLiteral("h6"))
             << htmlTagRegExp.arg(QStringLiteral("br"))
             << htmlTagRegExp.arg(QStringLiteral("hr"))
             << QStringLiteral("<small>") << QStringLiteral("</small>")
             << QStringLiteral("<big>") << QStringLiteral("</big>")
             << QStringLiteral("<strong>") << QStringLiteral("</strong>")
             << QStringLiteral("<em>") << QStringLiteral("</em>")
             << QStringLiteral("<center>") << QStringLiteral("</center>")
             << QStringLiteral("<nobr>") << QStringLiteral("</nobr>")
             << QStringLiteral("<blockquote>") << QStringLiteral("</blockquote>")
             << QStringLiteral("<pre>") << QStringLiteral("</pre>")
             << QStringLiteral("<code>") << QStringLiteral("</code>")
             << QStringLiteral("<li>") << QStringLiteral("</li>")
             << QStringLiteral("<ul>") << QStringLiteral("</ul>")
             << QStringLiteral("<ol>") << QStringLiteral("</ol>")
             << QStringLiteral("<dl>") << QStringLiteral("</dl>")
             << QStringLiteral("<table>") << QStringLiteral("</table>")
             << QStringLiteral("<thead>") << QStringLiteral("</thead>")
             << QStringLiteral("<tbody>") << QStringLiteral("</tbody>")
             << htmlTagRegExp.arg(QStringLiteral("th"))
             << htmlTagRegExp.arg(QStringLiteral("td"))
             << htmlTagRegExp.arg(QStringLiteral("tr"))
             << QStringLiteral("<strike>") << QStringLiteral("</strike>")
             << QStringLiteral("<del>") << QStringLiteral("</del>");

    for(const QString &keyword : keywords) {
        rule.pattern = QRegExp(keyword);
        rule.format = &keywordFormat;
        highlightingRules.append(rule);
    }

    rule.pattern = QRegExp(htmlTagRegExp.arg("img"));
    rule.format = &imageFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp(htmlTagRegExp.arg("a"));
    rule.format = &linkFormat;
    highlightingRules.append(rule);
}

bool HtmlHighlighter::isEnabled() const
{
    return enabled;
}

void HtmlHighlighter::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

void HtmlHighlighter::highlightBlock(const QString &text)
{
    if (enabled) {
        for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
            QRegExp expression(rule.pattern);
            int index = text.indexOf(expression);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, *(rule.format));
                index = text.indexOf(expression, index + length);
            }
        }
        setCurrentBlockState(0);
    }
}
