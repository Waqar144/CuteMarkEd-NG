/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     (1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 
 *     (2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.  
 *     
 *     (3) The name of the author may not be used to
 *     endorse or promote products derived from this software without
 *     specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <QDebug>
#include "MD4Cmarkdownconverter.h"


#include "md4c-html.h"
extern "C" {
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
}

#include "markdowndocument.h"
#include "template/htmltemplate.h"

class MD4CMarkdownDocument : public MarkdownDocument
{
public:
    QString text;
    MarkdownConverter::ConverterOptions options;
};

MD4CMarkdownConverter::MD4CMarkdownConverter(){}

MarkdownDocument *MD4CMarkdownConverter::createDocument(const QString &text, ConverterOptions options)
{
    MD4CMarkdownDocument *doc = new MD4CMarkdownDocument();
    doc->text = text;
    doc->options = options;
    return doc;
}

void captureHtmlFragment (const MD_CHAR* data, MD_SIZE data_size, void* userData) {
    QByteArray* array = static_cast<QByteArray*>(userData);

    if (data_size > 0) {
        array->append(data, int(data_size));
    }
}

QString MD4CMarkdownConverter::renderAsHtml(MarkdownDocument *document)
{
    MD4CMarkdownDocument *doc = dynamic_cast<MD4CMarkdownDocument*>(document);
    if (mdData == doc->text)
        return htmlResult;
    //cache
    mdData = doc->text;
    const auto data = doc->text.toUtf8();

    // return an empty string if the note is empty
    if (data.isEmpty()) {
        return QLatin1String("");
    }

    QByteArray array;
    const int renderResult = md_html(data.data(), MD_SIZE(data.size()),
        &captureHtmlFragment,
        &array,
        translateConverterOptions(doc->options),
        0);

    QString result;
    if (renderResult == 0) {
        result = QString::fromUtf8(array);
        htmlResult = result;
    } else {
        qWarning() << "MD4C Failure!";
    }

    return result;
}

Template *MD4CMarkdownConverter::templateRenderer() const
{
    static HtmlTemplate htmlTemplate;
    return &htmlTemplate;
}

MarkdownConverter::ConverterOptions MD4CMarkdownConverter::supportedOptions() const
{
    return MarkdownConverter::AutolinkOption |
           MarkdownConverter::NoStrikethroughOption |
           MarkdownConverter::NoHtmlOption |
           MarkdownConverter::NoTablesOption |
           MarkdownConverter::UnderlineOption;
}

unsigned long MD4CMarkdownConverter::translateConverterOptions(ConverterOptions options)
{
    unsigned converterOptions = MD_DIALECT_GITHUB;

    converterOptions &= ~(MD_FLAG_PERMISSIVEAUTOLINKS);
    converterOptions &= ~MD_FLAG_UNDERLINE;

    // autolink
    if (options.testFlag(MarkdownConverter::AutolinkOption)) {
        converterOptions |= MD_FLAG_PERMISSIVEAUTOLINKS | MD_FLAG_PERMISSIVEURLAUTOLINKS |
                            MD_FLAG_PERMISSIVEWWWAUTOLINKS | MD_FLAG_PERMISSIVEEMAILAUTOLINKS;
    }

    // strikethrough
    if (options.testFlag(MarkdownConverter::NoStrikethroughOption)) {
        converterOptions &= ~MD_FLAG_STRIKETHROUGH;
    }

    if (options.testFlag(MarkdownConverter::NoHtmlOption)) {
        converterOptions |= MD_FLAG_NOHTML;
    }

    if (options.testFlag(MarkdownConverter::NoTablesOption)) {
        converterOptions &= ~MD_FLAG_TABLES;
    }

    if (options.testFlag(MarkdownConverter::UnderlineOption)) {
        converterOptions |= MD_FLAG_UNDERLINE;
    }

    return converterOptions;
}
