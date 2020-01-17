TEMPLATE = subdirs

include($$PWD/md4c.pri)
include(../3rdparty/qmarkdowntextedit/qmarkdowntextedit.pri)

win32 {
    SUBDIRS += \
        discount \
        hunspell
}
