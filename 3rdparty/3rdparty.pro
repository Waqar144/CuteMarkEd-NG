TEMPLATE = subdirs

QT += core gui widgets

include($$PWD/md4c.pri)

win32 {
    SUBDIRS += \
        discount \
        hunspell
}
