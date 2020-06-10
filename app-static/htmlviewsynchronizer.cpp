/*
 * Copyright 2014 Christian Loose <christian.loose@hamburg.de>
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
#include "htmlviewsynchronizer.h"

#include <QPlainTextEdit>
#include <QScrollBar>
#include <QWebEngineView>


HtmlViewSynchronizer::HtmlViewSynchronizer(QWebEngineView *webView, QPlainTextEdit *editor) :
    ViewSynchronizer(webView, editor),
    scrollBarPos(0)
{
    // synchronize scrollbars
    connect(editor->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(scrollValueChanged(int)));

    // restore scrollbar position after content size changed
    connect(webView->page(), SIGNAL(contentsSizeChanged(QSizeF)),
            this, SLOT(htmlContentSizeChanged()));
}

HtmlViewSynchronizer::~HtmlViewSynchronizer()
{
}

void HtmlViewSynchronizer::setMaxScrollBar(double val) {
    maxScrollBar = val;
}

double HtmlViewSynchronizer::getMaxScrollBar() const {
    return maxScrollBar;
}

//disabled for now.
void HtmlViewSynchronizer::webViewScrolled()
{
//    const QSignalBlocker blocker(m_editor);
//    m_webView->page()->runJavaScript("document.body.scrollHeight",
//                                     [this](const QVariant &result){
//        setMaxScrollBar(result.toDouble());
//    });

//    double webViewMax = getMaxScrollBar();
//    double factor = (double)m_editor->verticalScrollBar()->maximum() / webViewMax;
//    double value = m_webView->page()->scrollPosition().y();

//    m_editor->verticalScrollBar()->setValue(qRound(value * factor));
//    // remember new vertical scrollbar position of markdown editor
//    rememberScrollBarPos();
}

void HtmlViewSynchronizer::scrollValueChanged(int value)
{
    int textMax = m_editor->verticalScrollBar()->maximum();
    const QString ScrollJavaScript(QStringLiteral("window.scrollTo(0, document.body.scrollHeight * %1 / %2);"));
    m_webView->page()->runJavaScript(ScrollJavaScript.arg(value).arg(textMax));
}

void HtmlViewSynchronizer::htmlContentSizeChanged()
{
    if (scrollBarPos > 0) {
        // restore previous scrollbar position
        scrollValueChanged(scrollBarPos);
    }
}

void HtmlViewSynchronizer::rememberScrollBarPos()
{
    scrollBarPos = m_editor->verticalScrollBar()->value();
}
