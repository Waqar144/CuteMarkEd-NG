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
#ifndef HTMLPREVIEWGENERATOR_H
#define HTMLPREVIEWGENERATOR_H

#include <QtCore/qthread.h>
#include <QtCore/qqueue.h>
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>
#include <QObject>
#include <QTimer>

#include <converter/markdownconverter.h>
#include <template/template.h>

class MarkdownDocument;
class Options;

class HtmlPreviewGenerator : public QObject
{
    Q_OBJECT

public:
    explicit HtmlPreviewGenerator(Options *opt, QObject *parent = 0);
    void updatePreview();
    
    bool isSupported(MarkdownConverter::ConverterOption option) const;

public slots:
    void markdownTextChanged(const QString &text);
    QString exportHtml(const QString &styleSheet, const QString &highlightingScript);

    void setMathSupportEnabled(bool enabled);
    void setDiagramSupportEnabled(bool enabled);
    void setWavedromSupportEnabled(bool enabled);
    void setCodeHighlightingEnabled(bool enabled);
    void setCodeHighlightingStyle(const QString &style);

    void markdownConverterChanged();

signals:
    void htmlResultReady(const QString &html);
private:
    void generateHtmlFromMarkdown();
    MarkdownConverter::ConverterOptions converterOptions() const;
    Template::RenderOptions renderOptions() const;
    int calculateDelay(const QString &text);

private:
    Options *options;
    QTimer *timer;
    MarkdownDocument *document;
    MarkdownConverter *converter;
    QString text;
    QMutex tasksMutex;
    QWaitCondition bufferNotEmpty;
};

#endif // HTMLPREVIEWGENERATOR_H
