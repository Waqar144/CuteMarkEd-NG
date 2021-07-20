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
#include "htmlpreviewgenerator.h"

#include <QFile>

#include <converter/markdownconverter.h>
#include <converter/markdowndocument.h>
#include <converter/MD4Cmarkdownconverter.h>
#include <converter/revealmarkdownconverter.h>
#include <QDebug>
#include <QTimer>

#include <template/template.h>

#include "options.h"
#include "yamlheaderchecker.h"

HtmlPreviewGenerator::HtmlPreviewGenerator(Options *opt, QObject *parent) :
    QObject(parent),
    options(opt),
    document(0),
    converter(0)
{
    connect(options, SIGNAL(markdownConverterChanged()), SLOT(markdownConverterChanged()));
    markdownConverterChanged();
}

bool HtmlPreviewGenerator::isSupported(MarkdownConverter::ConverterOption option) const
{
    return converter->supportedOptions().testFlag(option);
}

void HtmlPreviewGenerator::markdownTextChanged(const QString &text)
{
    // cut YAML header
    YamlHeaderChecker checker(text);
    QString actualText = checker.hasHeader() && options->isYamlHeaderSupportEnabled() ?
                            checker.body()
                          : text;

    this->text = actualText;
}

QString HtmlPreviewGenerator::exportHtml(const QString &styleSheet, const QString &highlightingScript)
{
    if (!document) return QString();

    QString header;
    if (!styleSheet.isEmpty()) {
        header += QStringLiteral("\n<style>%1</style>").arg(styleSheet);
    }

    if (!highlightingScript.isEmpty()) {
        // FIXME: doesn't really belong here
        QString highlightStyle;
        QFile f(QStringLiteral(":/scripts/highlight.js/styles/%1.css").arg(converter->templateRenderer()->codeHighlightingStyle()));
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            highlightStyle = f.readAll();
        }

        header += QStringLiteral("\n<style>%1</style>").arg(highlightStyle);
        header += QStringLiteral("\n<script>%1</script>").arg(highlightingScript);
        header += QLatin1String("\n<script>hljs.initHighlightingOnLoad();</script>");
    }

    return converter->templateRenderer()->exportAsHtml(header, converter->renderAsHtml(document), renderOptions());
}

void HtmlPreviewGenerator::setMathSupportEnabled(bool enabled)
{
    options->setMathSupportEnabled(enabled);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::setDiagramSupportEnabled(bool enabled)
{
    options->setDiagramSupportEnabled(enabled);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::setWavedromSupportEnabled(bool enabled)
{
    options->setWavedromSupportEnabled(enabled);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::setCodeHighlightingEnabled(bool enabled)
{
    options->setCodeHighlightingEnabled(enabled);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::setCodeHighlightingStyle(const QString &style)
{
    converter->templateRenderer()->setCodeHighlightingStyle(style);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::markdownConverterChanged()
{
    QString style;

    if (converter) {
        style = converter->templateRenderer()->codeHighlightingStyle();
        delete converter;
    }

    switch (options->markdownConverter()) {
    case Options::RevealMarkdownConverter:
        converter = new RevealMarkdownConverter();
        converter->templateRenderer()->setCodeHighlightingStyle(style);
        break;

    case Options::MD4CMarkdownConverter:
    default:
        converter = new MD4CMarkdownConverter();
        converter->templateRenderer()->setCodeHighlightingStyle(style);
        break;
    }
}

void HtmlPreviewGenerator::updatePreview()
{
    // delete previous markdown document
    if (document)
        delete document;

    // generate HTML from markdown
    document = converter->createDocument(this->text, converterOptions());
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::generateHtmlFromMarkdown()
{
    if (!document) return;
    QString html = converter->templateRenderer()->render(converter->renderAsHtml(document), renderOptions());
    emit htmlResultReady(html);
}

MarkdownConverter::ConverterOptions HtmlPreviewGenerator::converterOptions() const
{
    MarkdownConverter::ConverterOptions parserOptionFlags(MarkdownConverter::TableOfContentsOption | MarkdownConverter::NoStyleOption);

    // autolink
    if (options->isAutolinkEnabled()) {
        parserOptionFlags |= MarkdownConverter::AutolinkOption;
    }

    // strikethrough
    if (!options->isStrikethroughEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoStrikethroughOption;
    }

    // underlines
    if (options->isUnderlineEnabled()) {
        parserOptionFlags |= MarkdownConverter::UnderlineOption;
    }

    // alphabetic lists
    if (!options->isAlphabeticListsEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoAlphaListOption;
    }

    // definition lists
    if (!options->isDefinitionListsEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoDefinitionListOption;
    }

    // SmartyPants
    if (!options->isSmartyPantsEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoSmartypantsOption;
    }

    // Footnotes
    if (options->isFootnotesEnabled()) {
        parserOptionFlags |= MarkdownConverter::ExtraFootnoteOption;
    }

    // Superscript
    if (!options->isSuperscriptEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoSuperscriptOption;
    }

    return parserOptionFlags;
}

Template::RenderOptions HtmlPreviewGenerator::renderOptions() const
{
    Template::RenderOptions renderOptionFlags;

    // math support
    if (options->isMathSupportEnabled()) {
        renderOptionFlags |= Template::MathSupport;
    }

    // inline math support
    if (options->isMathInlineSupportEnabled()) {
        renderOptionFlags |= Template::MathInlineSupport;
    }

    // diagram support
    if (options->isDiagramSupportEnabled()) {
        renderOptionFlags |= Template::DiagramSupport;
    }

    // wavedrom support
    if (options->isWavedromSupportEnabled()) {
        renderOptionFlags |= Template::WavedromSupport;
    }

    // code highlighting
    if (options->isCodeHighlightingEnabled()) {
        renderOptionFlags |= Template::CodeHighlighting;
    }

    return renderOptionFlags;
}

int HtmlPreviewGenerator::calculateDelay(const QString &text)
{
    const int MINIMUM_DELAY = 10;
    const int MAXIMUM_DELAY = 2000;

    // calculate the processing delay based on amount of characters in the
    // markdown text. The delay is restricted to the interval [50, 2000] milliseconds;
    int delay = qMin(qMax(text.size() / 100, MINIMUM_DELAY), MAXIMUM_DELAY);

    return delay;
}
