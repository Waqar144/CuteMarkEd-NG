///*
// * Copyright 2013-2014 Christian Loose <christian.loose@hamburg.de>
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 2 of the License, or
// * (at your option) any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
// */
#include "md4cmarkdownconvertertest.h"

#include <QtTest>

#include <converter/MD4Cmarkdownconverter.h>
#include "loremipsumtestdata.h"

void MD4CMarkdownConverterTest::initTestCase()
{
    converter = new MD4CMarkdownConverter();
}

void MD4CMarkdownConverterTest::convertsEmptyStringToEmptyHtml()
{
    QString html = transformMarkdownToHtml(QString());

    QVERIFY(html.isNull());
}

void MD4CMarkdownConverterTest::convertsMarkdownParagraphToHtml()
{
    QString markdown = "This is an example";

    QString html = transformMarkdownToHtml(markdown);

    QVERIFY(!html.isEmpty());
    QCOMPARE(html, QStringLiteral("<p>This is an example</p>\n"));
}

void MD4CMarkdownConverterTest::convertsMarkdownHeaderToHtml()
{
    QString html = transformMarkdownToHtml(QStringLiteral("# This is an example"));

    QCOMPARE(html, QStringLiteral("<h1>This is an example</h1>\n"));

    html = transformMarkdownToHtml(QStringLiteral("## This is an example"));

    QCOMPARE(html, QStringLiteral("<h2>This is an example</h2>\n"));
}

void MD4CMarkdownConverterTest::preservesGermanUmlautsInHtml()
{
//#if defined(Q_CC_MSVC)
//    QSKIP("This causes an assert with MSVC");
//#endif

    QString markdown = QStringLiteral("äöü");

    QString html = transformMarkdownToHtml(markdown);

    QVERIFY(!html.isEmpty());
    QCOMPARE(html, QStringLiteral("<p>äöü</p>\n"));
}

void MD4CMarkdownConverterTest::supportsSuperscriptIfEnabled()
{
    QSKIP("no superscript in MD4C");
    QString html = transformMarkdownToHtml(QStringLiteral("a^2"));
    QCOMPARE(html, QStringLiteral("<p>a<sup>2</sup></p>"));
}

void MD4CMarkdownConverterTest::ignoresSuperscriptIfDisabled()
{
    MarkdownDocument *doc = converter->createDocument(QStringLiteral("a^2"), MD4CMarkdownConverter::NoSuperscriptOption);
    QCOMPARE(converter->renderAsHtml(doc), QStringLiteral("<p>a^2</p>\n"));
}

void MD4CMarkdownConverterTest::benchmark_data()
{
    QTest::addColumn<QString>("text");
    QTest::newRow("500 words text") << fiveHundredWordsLoremIpsumText;
    QString twoThousandWordsLoremIpsumText = (fiveHundredWordsLoremIpsumText +
                                              fiveHundredWordsLoremIpsumText +
                                              fiveHundredWordsLoremIpsumText +
                                              fiveHundredWordsLoremIpsumText);
    QTest::newRow("2000 words text") << twoThousandWordsLoremIpsumText;
    QString tenThousandWordsLoremIpsumText = (twoThousandWordsLoremIpsumText +
                                              twoThousandWordsLoremIpsumText +
                                              twoThousandWordsLoremIpsumText +
                                              twoThousandWordsLoremIpsumText +
                                              twoThousandWordsLoremIpsumText);
    QTest::newRow("10000 words text") << tenThousandWordsLoremIpsumText;
}

void MD4CMarkdownConverterTest::benchmark()
{
    QFETCH(QString, text);
    QBENCHMARK {
        MarkdownDocument *doc = converter->createDocument(text, 0);
        QString html = converter->renderAsHtml(doc);
    }
}

void MD4CMarkdownConverterTest::cleanupTestCase()
{
    delete converter;
}

bool MD4CMarkdownConverterTest::isIdAnchorDisabled(const QString &html)
{
    return html.startsWith("<a name=");
}
    
QString MD4CMarkdownConverterTest::transformMarkdownToHtml(const QString &text)
{
    MarkdownDocument *doc = converter->createDocument(text, 0);
    return converter->renderAsHtml(doc);
}
