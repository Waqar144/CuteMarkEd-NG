/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * The names of its contributors may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "fontawesomeiconengine.h"

#include <QFontDatabase>
#include <QPainter>
#include <QPalette>
#include <QPixmapCache>

int FontAwesomeIconEngine::fontId = -1;
QString FontAwesomeIconEngine::fontName;
QHash<QString, QChar> FontAwesomeIconEngine::namedCodepoints;

FontAwesomeIconEngine::FontAwesomeIconEngine() :
    QIconEngine()
{
    loadFont();
}

FontAwesomeIconEngine::FontAwesomeIconEngine(const QString &iconName) :
    QIconEngine()
{
    setIconName(iconName);
}

void FontAwesomeIconEngine::setIconName(const QString &iconName)
{
    this->iconName = iconName;
}

QIconEngine *FontAwesomeIconEngine::clone() const
{
    return new FontAwesomeIconEngine(iconName);
}

void FontAwesomeIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)

    painter->save();

    int pixelSize = qRound(rect.height()*0.625f);
    QFont font(fontName, pixelSize);
    painter->setFont(font);

    QPalette::ColorGroup colorGroup = QPalette::Active;
    if (mode == QIcon::Disabled) {
        colorGroup = QPalette::Disabled;
    }
    QPalette palette;
    painter->setPen(palette.color(colorGroup, QPalette::WindowText));

    QString text = QString(namedCodepoints.value(iconName));
    painter->drawText(rect, Qt::AlignCenter, text);

    painter->restore();
}

QPixmap FontAwesomeIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QString key = QStringLiteral("icons:%1:%2:%3:%4").arg(iconName).arg(size.width()).arg(size.height()).arg(mode);

    QPixmap pm(size);
    pm.fill(Qt::transparent);

    if (!QPixmapCache::find(key, &pm)) {
        QPainter p(&pm);
        paint(&p, QRect(QPoint(0,0),size), mode, state);
        QPixmapCache::insert(key, pm);
    }

    return pm;
}

void FontAwesomeIconEngine::loadFont()
{
    if (fontId < 0) {
        fontId = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/fontawesome.ttf"));

        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        fontName = fontFamilies.at(0);

        // 0xf000
        namedCodepoints.insert(QStringLiteral("fa-glass"), QChar(0xf000));
        namedCodepoints.insert(QStringLiteral("fa-music"), QChar(0xf001));
        namedCodepoints.insert(QStringLiteral("fa-search"), QChar(0xf002));
        namedCodepoints.insert(QStringLiteral("fa-envelope-o"), QChar(0xf003));
        namedCodepoints.insert(QStringLiteral("fa-heart"), QChar(0xf004));
        namedCodepoints.insert(QStringLiteral("fa-star"), QChar(0xf005));
        namedCodepoints.insert(QStringLiteral("fa-star-o"), QChar(0xf006));
        namedCodepoints.insert(QStringLiteral("fa-user"), QChar(0xf007));
        namedCodepoints.insert(QStringLiteral("fa-film"), QChar(0xf008));
        namedCodepoints.insert(QStringLiteral("fa-th-large"), QChar(0xf009));
        namedCodepoints.insert(QStringLiteral("fa-th"), QChar(0xf00a));
        namedCodepoints.insert(QStringLiteral("fa-th-list"), QChar(0xf00b));
        namedCodepoints.insert(QStringLiteral("fa-check"), QChar(0xf00c));
        namedCodepoints.insert(QStringLiteral("fa-times"), QChar(0xf00d));
        namedCodepoints.insert(QStringLiteral("fa-close"), QChar(0xf00d)); // alias
        namedCodepoints.insert(QStringLiteral("fa-remove"), QChar(0xf00d)); // alias
        namedCodepoints.insert(QStringLiteral("fa-search-plus"), QChar(0xf00e));

        // 0xf010
        namedCodepoints.insert(QStringLiteral("fa-search-minus"), QChar(0xf010));
        namedCodepoints.insert(QStringLiteral("fa-power-off"), QChar(0xf011));
        namedCodepoints.insert(QStringLiteral("fa-signal"), QChar(0xf012));
        namedCodepoints.insert(QStringLiteral("fa-cog"), QChar(0xf013));
        namedCodepoints.insert(QStringLiteral("fa-gear"), QChar(0xf013)); // alias
        namedCodepoints.insert(QStringLiteral("fa-trash-o"), QChar(0xf014));
        namedCodepoints.insert(QStringLiteral("fa-home"), QChar(0xf015));
        namedCodepoints.insert(QStringLiteral("fa-file-o"), QChar(0xf016));
        namedCodepoints.insert(QStringLiteral("fa-clock-o"), QChar(0xf017));
        namedCodepoints.insert(QStringLiteral("fa-road"), QChar(0xf018));
        namedCodepoints.insert(QStringLiteral("fa-download"), QChar(0xf019));
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-o-down"), QChar(0xf01a));
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-o-up"), QChar(0xf01b));
        namedCodepoints.insert(QStringLiteral("fa-inbox"), QChar(0xf01c));
        namedCodepoints.insert(QStringLiteral("fa-play-circle-o"), QChar(0xf01d));
        namedCodepoints.insert(QStringLiteral("fa-repeat"), QChar(0xf01e));
        namedCodepoints.insert(QStringLiteral("fa-rotate-right"), QChar(0xf01e)); // alias

        // 0xf021
        namedCodepoints.insert(QStringLiteral("fa-refresh"), QChar(0xf021));
        namedCodepoints.insert(QStringLiteral("fa-list-alt"), QChar(0xf022));
        namedCodepoints.insert(QStringLiteral("fa-lock"), QChar(0xf023));
        namedCodepoints.insert(QStringLiteral("fa-flag"), QChar(0xf024));
        namedCodepoints.insert(QStringLiteral("fa-headphones"), QChar(0xf025));
        namedCodepoints.insert(QStringLiteral("fa-volume-off"), QChar(0xf026));
        namedCodepoints.insert(QStringLiteral("fa-volume-down"), QChar(0xf027));
        namedCodepoints.insert(QStringLiteral("fa-volume-up"), QChar(0xf028));
        namedCodepoints.insert(QStringLiteral("fa-qrcode"), QChar(0xf029));
        namedCodepoints.insert(QStringLiteral("fa-barcode"), QChar(0xf02a));
        namedCodepoints.insert(QStringLiteral("fa-tag"), QChar(0xf02b));
        namedCodepoints.insert(QStringLiteral("fa-tags"), QChar(0xf02c));
        namedCodepoints.insert(QStringLiteral("fa-book"), QChar(0xf02d));
        namedCodepoints.insert(QStringLiteral("fa-bookmark"), QChar(0xf02e));
        namedCodepoints.insert(QStringLiteral("fa-print"), QChar(0xf02f));

        // 0xf030
        namedCodepoints.insert(QStringLiteral("fa-camera"), QChar(0xf030));
        namedCodepoints.insert(QStringLiteral("fa-font"), QChar(0xf031));
        namedCodepoints.insert(QStringLiteral("fa-bold"), QChar(0xf032));
        namedCodepoints.insert(QStringLiteral("fa-italic"), QChar(0xf033));
        namedCodepoints.insert(QStringLiteral("fa-text-height"), QChar(0xf034));
        namedCodepoints.insert(QStringLiteral("fa-text-width"), QChar(0xf035));
        namedCodepoints.insert(QStringLiteral("fa-align-left"), QChar(0xf036));
        namedCodepoints.insert(QStringLiteral("fa-align-center"), QChar(0xf037));
        namedCodepoints.insert(QStringLiteral("fa-align-right"), QChar(0xf038));
        namedCodepoints.insert(QStringLiteral("fa-align-justify"), QChar(0xf039));
        namedCodepoints.insert(QStringLiteral("fa-list"), QChar(0xf03a));
        namedCodepoints.insert(QStringLiteral("fa-outdent"), QChar(0xf03b));
        namedCodepoints.insert(QStringLiteral("fa-dedent"), QChar(0xf03b)); // alias
        namedCodepoints.insert(QStringLiteral("fa-indent"), QChar(0xf03c));
        namedCodepoints.insert(QStringLiteral("fa-video-camera"), QChar(0xf03d));
        namedCodepoints.insert(QStringLiteral("fa-picture-o"), QChar(0xf03e));
        namedCodepoints.insert(QStringLiteral("fa-photo"), QChar(0xf03e)); // alias
        namedCodepoints.insert(QStringLiteral("fa-image"), QChar(0xf03e)); // alias

        // 0xf040
        namedCodepoints.insert(QStringLiteral("fa-pencil"), QChar(0xf040));
        namedCodepoints.insert(QStringLiteral("fa-map-marker"), QChar(0xf041));
        namedCodepoints.insert(QStringLiteral("fa-adjust"), QChar(0xf042));
        namedCodepoints.insert(QStringLiteral("fa-tint"), QChar(0xf043));
        namedCodepoints.insert(QStringLiteral("fa-pencil-square-o"), QChar(0xf044));
        namedCodepoints.insert(QStringLiteral("fa-edit"), QChar(0xf044)); // alias
        namedCodepoints.insert(QStringLiteral("fa-share-square-o"), QChar(0xf045));
        namedCodepoints.insert(QStringLiteral("fa-check-square-o"), QChar(0xf046));
        namedCodepoints.insert(QStringLiteral("fa-arrows"), QChar(0xf047));
        namedCodepoints.insert(QStringLiteral("fa-step-backward"), QChar(0xf048));
        namedCodepoints.insert(QStringLiteral("fa-fast-backward"), QChar(0xf049));
        namedCodepoints.insert(QStringLiteral("fa-backward"), QChar(0xf04a));
        namedCodepoints.insert(QStringLiteral("fa-play"), QChar(0xf04b));
        namedCodepoints.insert(QStringLiteral("fa-pause"), QChar(0xf04c));
        namedCodepoints.insert(QStringLiteral("fa-stop"), QChar(0xf04d));
        namedCodepoints.insert(QStringLiteral("fa-forward"), QChar(0xf04e));

        // 0xf050
        namedCodepoints.insert(QStringLiteral("fa-fast-forward"), QChar(0xf050));
        namedCodepoints.insert(QStringLiteral("fa-step-forward"), QChar(0xf051));
        namedCodepoints.insert(QStringLiteral("fa-eject"), QChar(0xf052));
        namedCodepoints.insert(QStringLiteral("fa-chevron-left"), QChar(0xf053));
        namedCodepoints.insert(QStringLiteral("fa-chevron-right"), QChar(0xf054));
        namedCodepoints.insert(QStringLiteral("fa-plus-circle"), QChar(0xf055));
        namedCodepoints.insert(QStringLiteral("fa-minus-circle"), QChar(0xf056));
        namedCodepoints.insert(QStringLiteral("fa-times-circle"), QChar(0xf057));
        namedCodepoints.insert(QStringLiteral("fa-check-circle"), QChar(0xf058));
        namedCodepoints.insert(QStringLiteral("fa-question-circle"), QChar(0xf059));
        namedCodepoints.insert(QStringLiteral("fa-info-circle"), QChar(0xf05a));
        namedCodepoints.insert(QStringLiteral("fa-crosshairs"), QChar(0xf05b));
        namedCodepoints.insert(QStringLiteral("fa-times-circle-o"), QChar(0xf05c));
        namedCodepoints.insert(QStringLiteral("fa-check-circle-o"), QChar(0xf05d));
        namedCodepoints.insert(QStringLiteral("fa-ban"), QChar(0xf05e));

        // 0xf060
        namedCodepoints.insert(QStringLiteral("fa-arrow-left"), QChar(0xf060));
        namedCodepoints.insert(QStringLiteral("fa-arrow-right"), QChar(0xf061));
        namedCodepoints.insert(QStringLiteral("fa-arrow-up"), QChar(0xf062));
        namedCodepoints.insert(QStringLiteral("fa-arrow-down"), QChar(0xf063));
        namedCodepoints.insert(QStringLiteral("fa-share"), QChar(0xf064));
        namedCodepoints.insert(QStringLiteral("fa-mail-forward"), QChar(0xf064)); // alias
        namedCodepoints.insert(QStringLiteral("fa-expand"), QChar(0xf065));
        namedCodepoints.insert(QStringLiteral("fa-compress"), QChar(0xf066));
        namedCodepoints.insert(QStringLiteral("fa-plus"), QChar(0xf067));
        namedCodepoints.insert(QStringLiteral("fa-minus"), QChar(0xf068));
        namedCodepoints.insert(QStringLiteral("fa-asterisk"), QChar(0xf069));
        namedCodepoints.insert(QStringLiteral("fa-exclamation-circle"), QChar(0xf06a));
        namedCodepoints.insert(QStringLiteral("fa-gift"), QChar(0xf06b));
        namedCodepoints.insert(QStringLiteral("fa-leaf"), QChar(0xf06c));
        namedCodepoints.insert(QStringLiteral("fa-fire"), QChar(0xf06d));
        namedCodepoints.insert(QStringLiteral("fa-eye"), QChar(0xf06e));

        // 0xf070
        namedCodepoints.insert(QStringLiteral("fa-eye-slash"), QChar(0xf070));
        namedCodepoints.insert(QStringLiteral("fa-exclamation-triangle"), QChar(0xf071));
        namedCodepoints.insert(QStringLiteral("fa-warning"), QChar(0xf071)); // alias
        namedCodepoints.insert(QStringLiteral("fa-plane"), QChar(0xf072));
        namedCodepoints.insert(QStringLiteral("fa-calendar"), QChar(0xf073));
        namedCodepoints.insert(QStringLiteral("fa-random"), QChar(0xf074));
        namedCodepoints.insert(QStringLiteral("fa-comment"), QChar(0xf075));
        namedCodepoints.insert(QStringLiteral("fa-magnet"), QChar(0xf076));
        namedCodepoints.insert(QStringLiteral("fa-chevron-up"), QChar(0xf077));
        namedCodepoints.insert(QStringLiteral("fa-chevron-down"), QChar(0xf078));
        namedCodepoints.insert(QStringLiteral("fa-retweet"), QChar(0xf079));
        namedCodepoints.insert(QStringLiteral("fa-shopping-cart"), QChar(0xf07a));
        namedCodepoints.insert(QStringLiteral("fa-folder"), QChar(0xf07b));
        namedCodepoints.insert(QStringLiteral("fa-folder-open"), QChar(0xf07c));
        namedCodepoints.insert(QStringLiteral("fa-arrows-v"), QChar(0xf07d));
        namedCodepoints.insert(QStringLiteral("fa-arrows-h"), QChar(0xf07e));

        // 0xf080
        namedCodepoints.insert(QStringLiteral("fa-bar-chart"), QChar(0xf080));
        namedCodepoints.insert(QStringLiteral("fa-bar-chart-o"), QChar(0xf080)); // alias
        namedCodepoints.insert(QStringLiteral("fa-twitter-square"), QChar(0xf081));
        namedCodepoints.insert(QStringLiteral("fa-facebook-square"), QChar(0xf082));
        namedCodepoints.insert(QStringLiteral("fa-camera-retro"), QChar(0xf083));
        namedCodepoints.insert(QStringLiteral("fa-key"), QChar(0xf084));
        namedCodepoints.insert(QStringLiteral("fa-cogs"), QChar(0xf085));
        namedCodepoints.insert(QStringLiteral("fa-gears"), QChar(0xf085)); // alias
        namedCodepoints.insert(QStringLiteral("fa-comments"), QChar(0xf086));
        namedCodepoints.insert(QStringLiteral("fa-thumbs-o-up"), QChar(0xf087));
        namedCodepoints.insert(QStringLiteral("fa-thumbs-o-down"), QChar(0xf088));
        namedCodepoints.insert(QStringLiteral("fa-star-half"), QChar(0xf089));
        namedCodepoints.insert(QStringLiteral("fa-heart-o"), QChar(0xf08a));
        namedCodepoints.insert(QStringLiteral("fa-sign-out"), QChar(0xf08b));
        namedCodepoints.insert(QStringLiteral("fa-linkedin-square"), QChar(0xf08c));
        namedCodepoints.insert(QStringLiteral("fa-thumb-tack"), QChar(0xf08d));
        namedCodepoints.insert(QStringLiteral("fa-external-link"), QChar(0xf08e));

        // 0xf090
        namedCodepoints.insert(QStringLiteral("fa-sign-in"), QChar(0xf090));
        namedCodepoints.insert(QStringLiteral("fa-trophy"), QChar(0xf091));
        namedCodepoints.insert(QStringLiteral("fa-github-square"), QChar(0xf092));
        namedCodepoints.insert(QStringLiteral("fa-upload"), QChar(0xf093));
        namedCodepoints.insert(QStringLiteral("fa-lemon-o"), QChar(0xf094));
        namedCodepoints.insert(QStringLiteral("fa-phone"), QChar(0xf095));
        namedCodepoints.insert(QStringLiteral("fa-square-o"), QChar(0xf096));
        namedCodepoints.insert(QStringLiteral("fa-bookmark-o"), QChar(0xf097));
        namedCodepoints.insert(QStringLiteral("fa-phone-square"), QChar(0xf098));
        namedCodepoints.insert(QStringLiteral("fa-twitter"), QChar(0xf099));
        namedCodepoints.insert(QStringLiteral("fa-facebook"), QChar(0xf09a));
        namedCodepoints.insert(QStringLiteral("fa-github"), QChar(0xf09b));
        namedCodepoints.insert(QStringLiteral("fa-unlock"), QChar(0xf09c));
        namedCodepoints.insert(QStringLiteral("fa-credit-card"), QChar(0xf09d));
        namedCodepoints.insert(QStringLiteral("fa-rss"), QChar(0xf09e));

        // 0xf0a0
        namedCodepoints.insert(QStringLiteral("fa-hdd-o"), QChar(0xf0a0));
        namedCodepoints.insert(QStringLiteral("fa-bullhorn"), QChar(0xf0a1));
        namedCodepoints.insert(QStringLiteral("fa-bell-o"), QChar(0xf0a2));
        namedCodepoints.insert(QStringLiteral("fa-certificate"), QChar(0xf0a3));
        namedCodepoints.insert(QStringLiteral("fa-hand-o-right"), QChar(0xf0a4));
        namedCodepoints.insert(QStringLiteral("fa-hand-o-left"), QChar(0xf0a5));
        namedCodepoints.insert(QStringLiteral("fa-hand-o-up"), QChar(0xf0a6));
        namedCodepoints.insert(QStringLiteral("fa-hand-o-down"), QChar(0xf0a7));
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-left"), QChar(0xf0a8));
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-right"), QChar(0xf0a9));
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-up"), QChar(0xf0aa));
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-down"), QChar(0xf0ab));
        namedCodepoints.insert(QStringLiteral("fa-globe"), QChar(0xf0ac));
        namedCodepoints.insert(QStringLiteral("fa-wrench"), QChar(0xf0ad));
        namedCodepoints.insert(QStringLiteral("fa-tasks"), QChar(0xf0ae));

        // 0xf0b0
        namedCodepoints.insert(QStringLiteral("fa-filter"), QChar(0xf0b0));
        namedCodepoints.insert(QStringLiteral("fa-briefcase"), QChar(0xf0b1));
        namedCodepoints.insert(QStringLiteral("fa-arrows-alt"), QChar(0xf0b2));

        // 0xf0c0
        namedCodepoints.insert(QStringLiteral("fa-users"), QChar(0xf0c0));
        namedCodepoints.insert(QStringLiteral("fa-group"), QChar(0xf0c0)); // alias
        namedCodepoints.insert(QStringLiteral("fa-link"), QChar(0xf0c1));
        namedCodepoints.insert(QStringLiteral("fa-chain"), QChar(0xf0c1)); // alias
        namedCodepoints.insert(QStringLiteral("fa-cloud"), QChar(0xf0c2));
        namedCodepoints.insert(QStringLiteral("fa-flask"), QChar(0xf0c3));
        namedCodepoints.insert(QStringLiteral("fa-scissors"), QChar(0xf0c4));
        namedCodepoints.insert(QStringLiteral("fa-cut"), QChar(0xf0c4)); // alias
        namedCodepoints.insert(QStringLiteral("fa-files-o"), QChar(0xf0c5));
        namedCodepoints.insert(QStringLiteral("fa-copy"), QChar(0xf0c5)); // alias
        namedCodepoints.insert(QStringLiteral("fa-paperclip"), QChar(0xf0c6));
        namedCodepoints.insert(QStringLiteral("fa-floppy-o"), QChar(0xf0c7));
        namedCodepoints.insert(QStringLiteral("fa-save"), QChar(0xf0c7)); // alias
        namedCodepoints.insert(QStringLiteral("fa-square"), QChar(0xf0c8));
        namedCodepoints.insert(QStringLiteral("fa-bars"), QChar(0xf0c9));
        namedCodepoints.insert(QStringLiteral("fa-navicon"), QChar(0xf0c9)); // alias
        namedCodepoints.insert(QStringLiteral("fa-reorder"), QChar(0xf0c9)); // alias
        namedCodepoints.insert(QStringLiteral("fa-list-ul"), QChar(0xf0ca));
        namedCodepoints.insert(QStringLiteral("fa-list-ol"), QChar(0xf0cb));
        namedCodepoints.insert(QStringLiteral("fa-strikethrough"), QChar(0xf0cc));
        namedCodepoints.insert(QStringLiteral("fa-underline"), QChar(0xf0cd));
        namedCodepoints.insert(QStringLiteral("fa-table"), QChar(0xf0ce));

        // 0xf0d0
        namedCodepoints.insert(QStringLiteral("fa-magic"), QChar(0xf0d0));
        namedCodepoints.insert(QStringLiteral("fa-truck"), QChar(0xf0d1));
        namedCodepoints.insert(QStringLiteral("fa-pinterest"), QChar(0xf0d2));
        namedCodepoints.insert(QStringLiteral("fa-pinterest-square"), QChar(0xf0d3));
        namedCodepoints.insert(QStringLiteral("fa-google-plus-square"), QChar(0xf0d4));
        namedCodepoints.insert(QStringLiteral("fa-google-plus"), QChar(0xf0d5));
        namedCodepoints.insert(QStringLiteral("fa-money"), QChar(0xf0d6));
        namedCodepoints.insert(QStringLiteral("fa-caret-down"), QChar(0xf0d7));
        namedCodepoints.insert(QStringLiteral("fa-caret-up"), QChar(0xf0d8));
        namedCodepoints.insert(QStringLiteral("fa-caret-left"), QChar(0xf0d9));
        namedCodepoints.insert(QStringLiteral("fa-caret-right"), QChar(0xf0da));
        namedCodepoints.insert(QStringLiteral("fa-columns"), QChar(0xf0db));
        namedCodepoints.insert(QStringLiteral("fa-sort"), QChar(0xf0dc));
        namedCodepoints.insert(QStringLiteral("fa-unsorted"), QChar(0xf0dc)); // alias
        namedCodepoints.insert(QStringLiteral("fa-sort-asc"), QChar(0xf0dd));
        namedCodepoints.insert(QStringLiteral("fa-sort-down"), QChar(0xf0dd)); // alias
        namedCodepoints.insert(QStringLiteral("fa-sort-desc"), QChar(0xf0de));
        namedCodepoints.insert(QStringLiteral("fa-sort-up"), QChar(0xf0de)); // alias

        // 0xf0e0
        namedCodepoints.insert(QStringLiteral("fa-envelope"), QChar(0xf0e0));
        namedCodepoints.insert(QStringLiteral("fa-linkedin"), QChar(0xf0e1));
        namedCodepoints.insert(QStringLiteral("fa-undo"), QChar(0xf0e2));
        namedCodepoints.insert(QStringLiteral("fa-rotate-left"), QChar(0xf0e2)); // alias
        namedCodepoints.insert(QStringLiteral("fa-gavel"), QChar(0xf0e3));
        namedCodepoints.insert(QStringLiteral("fa-legal"), QChar(0xf0e3)); // alias
        namedCodepoints.insert(QStringLiteral("fa-tachometer"), QChar(0xf0e4));
        namedCodepoints.insert(QStringLiteral("fa-dashboard"), QChar(0xf0e4)); // alias
        namedCodepoints.insert(QStringLiteral("fa-comment-o"), QChar(0xf0e5));
        namedCodepoints.insert(QStringLiteral("fa-comments-o"), QChar(0xf0e6));
        namedCodepoints.insert(QStringLiteral("fa-bolt"), QChar(0xf0e7));
        namedCodepoints.insert(QStringLiteral("fa-flash"), QChar(0xf0e7)); // alias
        namedCodepoints.insert(QStringLiteral("fa-sitemap"), QChar(0xf0e8));
        namedCodepoints.insert(QStringLiteral("fa-umbrella"), QChar(0xf0e9));
        namedCodepoints.insert(QStringLiteral("fa-clipboard"), QChar(0xf0ea));
        namedCodepoints.insert(QStringLiteral("fa-paste"), QChar(0xf0ea)); // alias
        namedCodepoints.insert(QStringLiteral("fa-lightbulb-o"), QChar(0xf0eb));
        namedCodepoints.insert(QStringLiteral("fa-exchange"), QChar(0xf0ec));
        namedCodepoints.insert(QStringLiteral("fa-cloud-download"), QChar(0xf0ed));
        namedCodepoints.insert(QStringLiteral("fa-cloud-upload"), QChar(0xf0ee));

        // 0xf0f0
        namedCodepoints.insert(QStringLiteral("fa-user-md"), QChar(0xf0f0));
        namedCodepoints.insert(QStringLiteral("fa-stethoscope"), QChar(0xf0f1));
        namedCodepoints.insert(QStringLiteral("fa-suitcase"), QChar(0xf0f2));
        namedCodepoints.insert(QStringLiteral("fa-bell"), QChar(0xf0f3));
        namedCodepoints.insert(QStringLiteral("fa-coffee"), QChar(0xf0f4));
        namedCodepoints.insert(QStringLiteral("fa-cutlery"), QChar(0xf0f5));
        namedCodepoints.insert(QStringLiteral("fa-file-text-o"), QChar(0xf0f6));
        namedCodepoints.insert(QStringLiteral("fa-building-o"), QChar(0xf0f7));
        namedCodepoints.insert(QStringLiteral("fa-hospital-o"), QChar(0xf0f8));
        namedCodepoints.insert(QStringLiteral("fa-ambulance"), QChar(0xf0f9));
        namedCodepoints.insert(QStringLiteral("fa-medkit"), QChar(0xf0fa));
        namedCodepoints.insert(QStringLiteral("fa-fighter-jet"), QChar(0xf0fb));
        namedCodepoints.insert(QStringLiteral("fa-beer"), QChar(0xf0fc));
        namedCodepoints.insert(QStringLiteral("fa-h-square"), QChar(0xf0fd));
        namedCodepoints.insert(QStringLiteral("fa-plus-square"), QChar(0xf0fe));

        // 0xf100
        namedCodepoints.insert(QStringLiteral("fa-angle-double-left"), QChar(0xf100));
        namedCodepoints.insert(QStringLiteral("fa-angle-double-right"), QChar(0xf101));
        namedCodepoints.insert(QStringLiteral("fa-angle-double-up"), QChar(0xf102));
        namedCodepoints.insert(QStringLiteral("fa-angle-double-down"), QChar(0xf103));
        namedCodepoints.insert(QStringLiteral("fa-angle-left"), QChar(0xf104));
        namedCodepoints.insert(QStringLiteral("fa-angle-right"), QChar(0xf105));
        namedCodepoints.insert(QStringLiteral("fa-angle-up"), QChar(0xf106));
        namedCodepoints.insert(QStringLiteral("fa-angle-down"), QChar(0xf107));
        namedCodepoints.insert(QStringLiteral("fa-desktop"), QChar(0xf108));
        namedCodepoints.insert(QStringLiteral("fa-laptop"), QChar(0xf109));
        namedCodepoints.insert(QStringLiteral("fa-tablet"), QChar(0xf10a));
        namedCodepoints.insert(QStringLiteral("fa-mobile"), QChar(0xf10b));
        namedCodepoints.insert(QStringLiteral("fa-mobile-phone"), QChar(0xf10b)); // alias
        namedCodepoints.insert(QStringLiteral("fa-circle-o"), QChar(0xf10c));
        namedCodepoints.insert(QStringLiteral("fa-quote-left"), QChar(0xf10d));
        namedCodepoints.insert(QStringLiteral("fa-quote-right"), QChar(0xf10e));

        // 0xf110
        namedCodepoints.insert(QStringLiteral("fa-spinner"), QChar(0xf110));
        namedCodepoints.insert(QStringLiteral("fa-circle"), QChar(0xf111));
        namedCodepoints.insert(QStringLiteral("fa-reply"), QChar(0xf112));
        namedCodepoints.insert(QStringLiteral("fa-mail-reply"), QChar(0xf112)); // alias
        namedCodepoints.insert(QStringLiteral("fa-github-alt"), QChar(0xf113));
        namedCodepoints.insert(QStringLiteral("fa-folder-o"), QChar(0xf114));
        namedCodepoints.insert(QStringLiteral("fa-folder-open-o"), QChar(0xf115));
        namedCodepoints.insert(QStringLiteral("fa-smile-o"), QChar(0xf118));
        namedCodepoints.insert(QStringLiteral("fa-frown-o"), QChar(0xf119));
        namedCodepoints.insert(QStringLiteral("fa-meh-o"), QChar(0xf11a));
        namedCodepoints.insert(QStringLiteral("fa-gamepad"), QChar(0xf11b));
        namedCodepoints.insert(QStringLiteral("fa-keyboard-o"), QChar(0xf11c));
        namedCodepoints.insert(QStringLiteral("fa-flag-o"), QChar(0xf11d));
        namedCodepoints.insert(QStringLiteral("fa-flag-checkered"), QChar(0xf11e));

        // 0xf120
        namedCodepoints.insert(QStringLiteral("fa-terminal"), QChar(0xf120));
        namedCodepoints.insert(QStringLiteral("fa-code"), QChar(0xf121));
        namedCodepoints.insert(QStringLiteral("fa-reply-all"), QChar(0xf122));
        namedCodepoints.insert(QStringLiteral("fa-mail-reply-all"), QChar(0xf122)); // duplicate
        namedCodepoints.insert(QStringLiteral("fa-star-half-o"), QChar(0xf123));
        namedCodepoints.insert(QStringLiteral("fa-star-half-empty"), QChar(0xf123)); // alias
        namedCodepoints.insert(QStringLiteral("fa-star-half-full"), QChar(0xf123)); // alias
        namedCodepoints.insert(QStringLiteral("fa-location-arrow"), QChar(0xf124));
        namedCodepoints.insert(QStringLiteral("fa-crop"), QChar(0xf125));
        namedCodepoints.insert(QStringLiteral("fa-code-fork"), QChar(0xf126));
        namedCodepoints.insert(QStringLiteral("fa-chain-broken"), QChar(0xf127));
        namedCodepoints.insert(QStringLiteral("fa-unlink"), QChar(0xf127)); // alias
        namedCodepoints.insert(QStringLiteral("fa-question"), QChar(0xf128));
        namedCodepoints.insert(QStringLiteral("fa-info"), QChar(0xf129));
        namedCodepoints.insert(QStringLiteral("fa-exclamation"), QChar(0xf12a));
        namedCodepoints.insert(QStringLiteral("fa-superscript"), QChar(0xf12b));
        namedCodepoints.insert(QStringLiteral("fa-subscript"), QChar(0xf12c));
        namedCodepoints.insert(QStringLiteral("fa-eraser"), QChar(0xf12d));
        namedCodepoints.insert(QStringLiteral("fa-puzzle-piece"), QChar(0xf12e));

        // 0xf130
        namedCodepoints.insert(QStringLiteral("fa-microphone"), QChar(0xf130));
        namedCodepoints.insert(QStringLiteral("fa-microphone-slash"), QChar(0xf131));
        namedCodepoints.insert(QStringLiteral("fa-shield"), QChar(0xf132));
        namedCodepoints.insert(QStringLiteral("fa-calendar-o"), QChar(0xf133));
        namedCodepoints.insert(QStringLiteral("fa-fire-extinguisher"), QChar(0xf134));
        namedCodepoints.insert(QStringLiteral("fa-rocket"), QChar(0xf135));
        namedCodepoints.insert(QStringLiteral("fa-maxcdn"), QChar(0xf136));
        namedCodepoints.insert(QStringLiteral("fa-chevron-circle-left"), QChar(0xf137));
        namedCodepoints.insert(QStringLiteral("fa-chevron-circle-right"), QChar(0xf138));
        namedCodepoints.insert(QStringLiteral("fa-chevron-circle-up"), QChar(0xf139));
        namedCodepoints.insert(QStringLiteral("fa-chevron-circle-down"), QChar(0xf13a));
        namedCodepoints.insert(QStringLiteral("fa-html5"), QChar(0xf13b));
        namedCodepoints.insert(QStringLiteral("fa-css3"), QChar(0xf13c));
        namedCodepoints.insert(QStringLiteral("fa-anchor"), QChar(0xf13d));
        namedCodepoints.insert(QStringLiteral("fa-unlock-alt"), QChar(0xf13e));

        // 0xf140
        namedCodepoints.insert(QStringLiteral("fa-bullseye"), QChar(0xf140));
        namedCodepoints.insert(QStringLiteral("fa-ellipsis-h"), QChar(0xf141));
        namedCodepoints.insert(QStringLiteral("fa-ellipsis-v"), QChar(0xf142));
        namedCodepoints.insert(QStringLiteral("fa-rss-square"), QChar(0xf143));
        namedCodepoints.insert(QStringLiteral("fa-play-circle"), QChar(0xf144));
        namedCodepoints.insert(QStringLiteral("fa-ticket"), QChar(0xf145));
        namedCodepoints.insert(QStringLiteral("fa-minus-square"), QChar(0xf146));
        namedCodepoints.insert(QStringLiteral("fa-minus-square-o"), QChar(0xf147));
        namedCodepoints.insert(QStringLiteral("fa-level-up"), QChar(0xf148));
        namedCodepoints.insert(QStringLiteral("fa-level-down"), QChar(0xf149));
        namedCodepoints.insert(QStringLiteral("fa-check-square"), QChar(0xf14a));
        namedCodepoints.insert(QStringLiteral("fa-pencil-square"), QChar(0xf14b));
        namedCodepoints.insert(QStringLiteral("fa-external-link-square"), QChar(0xf14c));
        namedCodepoints.insert(QStringLiteral("fa-share-square"), QChar(0xf14d));
        namedCodepoints.insert(QStringLiteral("fa-compass"), QChar(0xf14e));

        // 0xf150
        namedCodepoints.insert(QStringLiteral("fa-caret-square-o-down"), QChar(0xf150));
        namedCodepoints.insert(QStringLiteral("fa-toggle-down"), QChar(0xf150)); // alias
        namedCodepoints.insert(QStringLiteral("fa-caret-square-o-up"), QChar(0xf151));
        namedCodepoints.insert(QStringLiteral("fa-toggle-up"), QChar(0xf151)); // alias
        namedCodepoints.insert(QStringLiteral("fa-caret-square-o-right"), QChar(0xf152));
        namedCodepoints.insert(QStringLiteral("fa-toggle-right"), QChar(0xf152)); // alias
        namedCodepoints.insert(QStringLiteral("fa-eur"), QChar(0xf153));
        namedCodepoints.insert(QStringLiteral("fa-euro"), QChar(0xf153)); // alias
        namedCodepoints.insert(QStringLiteral("fa-gbp"), QChar(0xf154));
        namedCodepoints.insert(QStringLiteral("fa-usd"), QChar(0xf155));
        namedCodepoints.insert(QStringLiteral("fa-dollar"), QChar(0xf155)); // alias
        namedCodepoints.insert(QStringLiteral("fa-inr"), QChar(0xf156));
        namedCodepoints.insert(QStringLiteral("fa-rupee"), QChar(0xf156)); // alias
        namedCodepoints.insert(QStringLiteral("fa-jpy"), QChar(0xf157));
        namedCodepoints.insert(QStringLiteral("fa-cny"), QChar(0xf157)); // alias
        namedCodepoints.insert(QStringLiteral("fa-rmb"), QChar(0xf157)); // alias
        namedCodepoints.insert(QStringLiteral("fa-yen"), QChar(0xf157)); // alias
        namedCodepoints.insert(QStringLiteral("fa-rub"), QChar(0xf158));
        namedCodepoints.insert(QStringLiteral("fa-ruble"), QChar(0xf158)); // alias
        namedCodepoints.insert(QStringLiteral("fa-rouble"), QChar(0xf158)); // alias
        namedCodepoints.insert(QStringLiteral("fa-krw"), QChar(0xf159));
        namedCodepoints.insert(QStringLiteral("fa-won"), QChar(0xf159)); // alias
        namedCodepoints.insert(QStringLiteral("fa-btc"), QChar(0xf15a));
        namedCodepoints.insert(QStringLiteral("fa-bitcoin"), QChar(0xf15a)); // alias
        namedCodepoints.insert(QStringLiteral("fa-file"), QChar(0xf15b));
        namedCodepoints.insert(QStringLiteral("fa-file-text"), QChar(0xf15c));
        namedCodepoints.insert(QStringLiteral("fa-sort-alpha-asc"), QChar(0xf15d));
        namedCodepoints.insert(QStringLiteral("fa-sort-alpha-desc"), QChar(0xf15e));

        // 0xf160
        namedCodepoints.insert(QStringLiteral("fa-sort-amount-asc"), QChar(0xf160));
        namedCodepoints.insert(QStringLiteral("fa-sort-amount-desc"), QChar(0xf161));
        namedCodepoints.insert(QStringLiteral("fa-sort-numeric-asc"), QChar(0xf162));
        namedCodepoints.insert(QStringLiteral("fa-sort-numeric-desc"), QChar(0xf163));
        namedCodepoints.insert(QStringLiteral("fa-thumbs-up"), QChar(0xf164));
        namedCodepoints.insert(QStringLiteral("fa-thumbs-down"), QChar(0xf165));
        namedCodepoints.insert(QStringLiteral("fa-youtube-square"), QChar(0xf166));
        namedCodepoints.insert(QStringLiteral("fa-youtube"), QChar(0xf167));
        namedCodepoints.insert(QStringLiteral("fa-xing"), QChar(0xf168));
        namedCodepoints.insert(QStringLiteral("fa-xing-square"), QChar(0xf169));
        namedCodepoints.insert(QStringLiteral("fa-youtube-play"), QChar(0xf16a));
        namedCodepoints.insert(QStringLiteral("fa-dropbox"), QChar(0xf16b));
        namedCodepoints.insert(QStringLiteral("fa-stack-overflow"), QChar(0xf16c));
        namedCodepoints.insert(QStringLiteral("fa-instagram"), QChar(0xf16d));
        namedCodepoints.insert(QStringLiteral("fa-flickr"), QChar(0xf16e));

        // 0xf170
        namedCodepoints.insert(QStringLiteral("fa-adn"), QChar(0xf170));
        namedCodepoints.insert(QStringLiteral("fa-bitbucket"), QChar(0xf171));
        namedCodepoints.insert(QStringLiteral("fa-bitbucket-square"), QChar(0xf172));
        namedCodepoints.insert(QStringLiteral("fa-tumblr"), QChar(0xf173));
        namedCodepoints.insert(QStringLiteral("fa-tumblr-square"), QChar(0xf174));
        namedCodepoints.insert(QStringLiteral("fa-long-arrow-down"), QChar(0xf175));
        namedCodepoints.insert(QStringLiteral("fa-long-arrow-up"), QChar(0xf176));
        namedCodepoints.insert(QStringLiteral("fa-long-arrow-left"), QChar(0xf177));
        namedCodepoints.insert(QStringLiteral("fa-long-arrow-right"), QChar(0xf178));
        namedCodepoints.insert(QStringLiteral("fa-apple"), QChar(0xf179));
        namedCodepoints.insert(QStringLiteral("fa-windows"), QChar(0xf17a));
        namedCodepoints.insert(QStringLiteral("fa-android"), QChar(0xf17b));
        namedCodepoints.insert(QStringLiteral("fa-linux"), QChar(0xf17c));
        namedCodepoints.insert(QStringLiteral("fa-dribbble"), QChar(0xf17d));
        namedCodepoints.insert(QStringLiteral("fa-skype"), QChar(0xf17e));

        // 0xf180
        namedCodepoints.insert(QStringLiteral("fa-foursquare"), QChar(0xf180));
        namedCodepoints.insert(QStringLiteral("fa-trello"), QChar(0xf181));
        namedCodepoints.insert(QStringLiteral("fa-female"), QChar(0xf182));
        namedCodepoints.insert(QStringLiteral("fa-male"), QChar(0xf183));
        namedCodepoints.insert(QStringLiteral("fa-gittip"), QChar(0xf184));
        namedCodepoints.insert(QStringLiteral("fa-sun-o"), QChar(0xf185));
        namedCodepoints.insert(QStringLiteral("fa-moon-o"), QChar(0xf186));
        namedCodepoints.insert(QStringLiteral("fa-archive"), QChar(0xf187));
        namedCodepoints.insert(QStringLiteral("fa-bug"), QChar(0xf188));
        namedCodepoints.insert(QStringLiteral("fa-vk"), QChar(0xf189));
        namedCodepoints.insert(QStringLiteral("fa-weibo"), QChar(0xf18a));
        namedCodepoints.insert(QStringLiteral("fa-renren"), QChar(0xf18b));
        namedCodepoints.insert(QStringLiteral("fa-pagelines"), QChar(0xf18c));
        namedCodepoints.insert(QStringLiteral("fa-stack-exchange"), QChar(0xf18d));
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-o-right"), QChar(0xf18e));

        // 0xf190
        namedCodepoints.insert(QStringLiteral("fa-arrow-circle-o-left"), QChar(0xf190));
        namedCodepoints.insert(QStringLiteral("fa-caret-square-o-left"), QChar(0xf191));
        namedCodepoints.insert(QStringLiteral("fa-toggle-left"), QChar(0xf191)); // alias
        namedCodepoints.insert(QStringLiteral("fa-dot-circle-o"), QChar(0xf192));
        namedCodepoints.insert(QStringLiteral("fa-wheelchair"), QChar(0xf193));
        namedCodepoints.insert(QStringLiteral("fa-vimeo-square"), QChar(0xf194));
        namedCodepoints.insert(QStringLiteral("fa-try"), QChar(0xf195));
        namedCodepoints.insert(QStringLiteral("fa-turkish-lira"), QChar(0xf195)); // alias
        namedCodepoints.insert(QStringLiteral("fa-plus-square-o"), QChar(0xf196));
        namedCodepoints.insert(QStringLiteral("fa-space-shuttle"), QChar(0xf197));
        namedCodepoints.insert(QStringLiteral("fa-slack"), QChar(0xf198));
        namedCodepoints.insert(QStringLiteral("fa-envelope-square"), QChar(0xf199));
        namedCodepoints.insert(QStringLiteral("fa-wordpress"), QChar(0xf19a));
        namedCodepoints.insert(QStringLiteral("fa-openid"), QChar(0xf19b));
        namedCodepoints.insert(QStringLiteral("fa-university"), QChar(0xf19c));
        namedCodepoints.insert(QStringLiteral("fa-bank"), QChar(0xf19c)); // alias
        namedCodepoints.insert(QStringLiteral("fa-institution"), QChar(0xf19c)); // alias
        namedCodepoints.insert(QStringLiteral("fa-graduation-cap"), QChar(0xf19d));
        namedCodepoints.insert(QStringLiteral("fa-mortar-board"), QChar(0xf19d)); // alias
        namedCodepoints.insert(QStringLiteral("fa-yahoo"), QChar(0xf19e));

        // 0xf1a0
        namedCodepoints.insert(QStringLiteral("fa-google"), QChar(0xf1a0));
        namedCodepoints.insert(QStringLiteral("fa-reddit"), QChar(0xf1a1));
        namedCodepoints.insert(QStringLiteral("fa-reddit-square"), QChar(0xf1a2));
        namedCodepoints.insert(QStringLiteral("fa-stumbleupon-circle"), QChar(0xf1a3));
        namedCodepoints.insert(QStringLiteral("fa-stumbleupon"), QChar(0xf1a4));
        namedCodepoints.insert(QStringLiteral("fa-delicious"), QChar(0xf1a5));
        namedCodepoints.insert(QStringLiteral("fa-digg"), QChar(0xf1a6));
        namedCodepoints.insert(QStringLiteral("fa-pied-piper"), QChar(0xf1a7));
        namedCodepoints.insert(QStringLiteral("fa-pied-piper-alt"), QChar(0xf1a8));
        namedCodepoints.insert(QStringLiteral("fa-drupal"), QChar(0xf1a9));
        namedCodepoints.insert(QStringLiteral("fa-joomla"), QChar(0xf1aa));
        namedCodepoints.insert(QStringLiteral("fa-language"), QChar(0xf1ab));
        namedCodepoints.insert(QStringLiteral("fa-fax"), QChar(0xf1ac));
        namedCodepoints.insert(QStringLiteral("fa-building"), QChar(0xf1ad));
        namedCodepoints.insert(QStringLiteral("fa-child"), QChar(0xf1ae));

        // 0xf1b0
        namedCodepoints.insert(QStringLiteral("fa-paw"), QChar(0xf1b0));
        namedCodepoints.insert(QStringLiteral("fa-spoon"), QChar(0xf1b1));
        namedCodepoints.insert(QStringLiteral("fa-cube"), QChar(0xf1b2));
        namedCodepoints.insert(QStringLiteral("fa-cubes"), QChar(0xf1b3));
        namedCodepoints.insert(QStringLiteral("fa-behance"), QChar(0xf1b4));
        namedCodepoints.insert(QStringLiteral("fa-behance-square"), QChar(0xf1b5));
        namedCodepoints.insert(QStringLiteral("fa-steam"), QChar(0xf1b6));
        namedCodepoints.insert(QStringLiteral("fa-steam-square"), QChar(0xf1b7));
        namedCodepoints.insert(QStringLiteral("fa-recycle"), QChar(0xf1b8));
        namedCodepoints.insert(QStringLiteral("fa-car"), QChar(0xf1b9));
        namedCodepoints.insert(QStringLiteral("fa-automobile"), QChar(0xf1b9)); // alias
        namedCodepoints.insert(QStringLiteral("fa-cab"), QChar(0xf1ba)); // alias
        namedCodepoints.insert(QStringLiteral("fa-taxi"), QChar(0xf1ba));
        namedCodepoints.insert(QStringLiteral("fa-tree"), QChar(0xf1bb));
        namedCodepoints.insert(QStringLiteral("fa-spotify"), QChar(0xf1bc));
        namedCodepoints.insert(QStringLiteral("fa-deviantart"), QChar(0xf1bd));
        namedCodepoints.insert(QStringLiteral("fa-soundcloud"), QChar(0xf1be));

        // 0xf1c0
        namedCodepoints.insert(QStringLiteral("fa-database"), QChar(0xf1c0));
        namedCodepoints.insert(QStringLiteral("fa-file-pdf-o"), QChar(0xf1c1));
        namedCodepoints.insert(QStringLiteral("fa-file-word-o"), QChar(0xf1c2));
        namedCodepoints.insert(QStringLiteral("fa-file-excel-o"), QChar(0xf1c3));
        namedCodepoints.insert(QStringLiteral("fa-file-powerpoint-o"), QChar(0xf1c4));
        namedCodepoints.insert(QStringLiteral("fa-file-image-o"), QChar(0xf1c5));
        namedCodepoints.insert(QStringLiteral("fa-file-photo-o"), QChar(0xf1c5)); // alias
        namedCodepoints.insert(QStringLiteral("fa-file-picture-o"), QChar(0xf1c5)); // alias
        namedCodepoints.insert(QStringLiteral("fa-file-archive-o"), QChar(0xf1c6));
        namedCodepoints.insert(QStringLiteral("fa-file-zip-o"), QChar(0xf1c6)); // alias
        namedCodepoints.insert(QStringLiteral("fa-file-audio-o"), QChar(0xf1c7));
        namedCodepoints.insert(QStringLiteral("fa-file-sound-o"), QChar(0xf1c7)); // alias
        namedCodepoints.insert(QStringLiteral("fa-file-video-o"), QChar(0xf1c8));
        namedCodepoints.insert(QStringLiteral("fa-file-movie-o"), QChar(0xf1c8)); // alias
        namedCodepoints.insert(QStringLiteral("fa-file-code-o"), QChar(0xf1c9));
        namedCodepoints.insert(QStringLiteral("fa-vine"), QChar(0xf1ca));
        namedCodepoints.insert(QStringLiteral("fa-codepen"), QChar(0xf1cb));
        namedCodepoints.insert(QStringLiteral("fa-jsfiddle"), QChar(0xf1cc));
        namedCodepoints.insert(QStringLiteral("fa-life-bouy"), QChar(0xf1cd)); // alias
        namedCodepoints.insert(QStringLiteral("fa-life-buoy"), QChar(0xf1cd)); // alias
        namedCodepoints.insert(QStringLiteral("fa-life-ring"), QChar(0xf1cd));
        namedCodepoints.insert(QStringLiteral("fa-life-saver"), QChar(0xf1cd)); // alias
        namedCodepoints.insert(QStringLiteral("fa-support"), QChar(0xf1cd)); // alias
        namedCodepoints.insert(QStringLiteral("fa-circle-o-notch"), QChar(0xf1ce));

        // 0xf1d0
        namedCodepoints.insert(QStringLiteral("fa-rebel"), QChar(0xf1d0));
        namedCodepoints.insert(QStringLiteral("fa-ra"), QChar(0xf1d0)); // alias
        namedCodepoints.insert(QStringLiteral("fa-empire"), QChar(0xf1d1));
        namedCodepoints.insert(QStringLiteral("fa-ge"), QChar(0xf1d1)); // alias
        namedCodepoints.insert(QStringLiteral("fa-git-square"), QChar(0xf1d2));
        namedCodepoints.insert(QStringLiteral("fa-git"), QChar(0xf1d3));
        namedCodepoints.insert(QStringLiteral("fa-hacker-news"), QChar(0xf1d4));
        namedCodepoints.insert(QStringLiteral("fa-tencent-weibo"), QChar(0xf1d5));
        namedCodepoints.insert(QStringLiteral("fa-qq"), QChar(0xf1d6));
        namedCodepoints.insert(QStringLiteral("fa-wechat"), QChar(0xf1d7)); // alias
        namedCodepoints.insert(QStringLiteral("fa-weixin"), QChar(0xf1d7));
        namedCodepoints.insert(QStringLiteral("fa-paper-plane"), QChar(0xf1d8));
        namedCodepoints.insert(QStringLiteral("fa-send"), QChar(0xf1d8)); // alias
        namedCodepoints.insert(QStringLiteral("fa-paper-plane-o"), QChar(0xf1d9));
        namedCodepoints.insert(QStringLiteral("fa-send-o"), QChar(0xf1d9)); // alias
        namedCodepoints.insert(QStringLiteral("fa-history"), QChar(0xf1da));
        namedCodepoints.insert(QStringLiteral("fa-circle-thin"), QChar(0xf1db));
        namedCodepoints.insert(QStringLiteral("fa-header"), QChar(0xf1dc));
        namedCodepoints.insert(QStringLiteral("fa-paragraph"), QChar(0xf1dd));
        namedCodepoints.insert(QStringLiteral("fa-sliders"), QChar(0xf1de));

        // 0xf1e0
        namedCodepoints.insert(QStringLiteral("fa-share-alt"), QChar(0xf1e0));
        namedCodepoints.insert(QStringLiteral("fa-share-alt-square"), QChar(0xf1e1));
        namedCodepoints.insert(QStringLiteral("fa-bomb"), QChar(0xf1e2));
        namedCodepoints.insert(QStringLiteral("fa-futbol-o"), QChar(0xf1e3));
        namedCodepoints.insert(QStringLiteral("fa-soccer-ball-o"), QChar(0xf1e3)); // alias
        namedCodepoints.insert(QStringLiteral("fa-tty"), QChar(0xf1e4));
        namedCodepoints.insert(QStringLiteral("fa-binoculars"), QChar(0xf1e5));
        namedCodepoints.insert(QStringLiteral("fa-plug"), QChar(0xf1e6));
        namedCodepoints.insert(QStringLiteral("fa-slideshare"), QChar(0xf1e7));
        namedCodepoints.insert(QStringLiteral("fa-twitch"), QChar(0xf1e8));
        namedCodepoints.insert(QStringLiteral("fa-yelp"), QChar(0xf1e9));
        namedCodepoints.insert(QStringLiteral("fa-newspaper-o"), QChar(0xf1ea));
        namedCodepoints.insert(QStringLiteral("fa-wifi"), QChar(0xf1eb));
        namedCodepoints.insert(QStringLiteral("fa-calculator"), QChar(0xf1ec));
        namedCodepoints.insert(QStringLiteral("fa-paypal"), QChar(0xf1ed));
        namedCodepoints.insert(QStringLiteral("fa-google-wallet"), QChar(0xf1ee));

        // 0xf1f0
        namedCodepoints.insert(QStringLiteral("fa-cc-visa"), QChar(0xf1f0));
        namedCodepoints.insert(QStringLiteral("fa-cc-mastercard"), QChar(0xf1f1));
        namedCodepoints.insert(QStringLiteral("fa-cc-discover"), QChar(0xf1f2));
        namedCodepoints.insert(QStringLiteral("fa-cc-amex"), QChar(0xf1f3));
        namedCodepoints.insert(QStringLiteral("fa-cc-paypal"), QChar(0xf1f4));
        namedCodepoints.insert(QStringLiteral("fa-cc-stripe"), QChar(0xf1f5));
        namedCodepoints.insert(QStringLiteral("fa-bell-slash"), QChar(0xf1f6));
        namedCodepoints.insert(QStringLiteral("fa-bell-slash-o"), QChar(0xf1f7));
        namedCodepoints.insert(QStringLiteral("fa-trash"), QChar(0xf1f8));
        namedCodepoints.insert(QStringLiteral("fa-copyright"), QChar(0xf1f9));
        namedCodepoints.insert(QStringLiteral("fa-at"), QChar(0xf1fa));
        namedCodepoints.insert(QStringLiteral("fa-eyedropper"), QChar(0xf1fb));
        namedCodepoints.insert(QStringLiteral("fa-paint-brush"), QChar(0xf1fc));
        namedCodepoints.insert(QStringLiteral("fa-birthday-cake"), QChar(0xf1fd));
        namedCodepoints.insert(QStringLiteral("fa-area-chart"), QChar(0xf1fe));

        // 0xf200
        namedCodepoints.insert(QStringLiteral("fa-pie-chart"), QChar(0xf200));
        namedCodepoints.insert(QStringLiteral("fa-line-chart"), QChar(0xf201));
        namedCodepoints.insert(QStringLiteral("fa-lastfm"), QChar(0xf202));
        namedCodepoints.insert(QStringLiteral("fa-lastfm-square"), QChar(0xf203));
        namedCodepoints.insert(QStringLiteral("fa-toggle-off"), QChar(0xf204));
        namedCodepoints.insert(QStringLiteral("fa-toggle-on"), QChar(0xf205));
        namedCodepoints.insert(QStringLiteral("fa-bicycle"), QChar(0xf206));
        namedCodepoints.insert(QStringLiteral("fa-bus"), QChar(0xf207));
        namedCodepoints.insert(QStringLiteral("fa-ioxhost"), QChar(0xf208));
        namedCodepoints.insert(QStringLiteral("fa-angellist"), QChar(0xf209));
        namedCodepoints.insert(QStringLiteral("fa-cc"), QChar(0xf20a));
        namedCodepoints.insert(QStringLiteral("fa-ils"), QChar(0xf20b));
        namedCodepoints.insert(QStringLiteral("fa-shekel"), QChar(0xf20b)); // alias
        namedCodepoints.insert(QStringLiteral("fa-sheqel"), QChar(0xf20b)); // alias
        namedCodepoints.insert(QStringLiteral("fa-meanpath"), QChar(0xf20c));
    }
}
