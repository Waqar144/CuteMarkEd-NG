/*
 * Copyright 2014 Christian Loose <christian.loose@hamburg.de>
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
#ifndef HTMLTEMPLATE_H
#define HTMLTEMPLATE_H

#include "template.h"

class HtmlTemplate : public Template
{
public:
    HtmlTemplate();
	explicit HtmlTemplate(const QString &templateString);

    virtual QString render(const QString &body, RenderOptions options) const;
    virtual QString exportAsHtml(const QString &header, const QString &body, RenderOptions options) const;

private:
    QString renderAsHtml(const QString &header, const QString &body, RenderOptions options) const;
    QString buildHtmlHeader(RenderOptions options) const;
    void convertDiagramCodeSectionToDiv(QString &body) const;
    void convertWavedromCodeSectionToScript(QString &body) const;

    QString htmlTemplate;
};

#endif // HTMLTEMPLATE_H
