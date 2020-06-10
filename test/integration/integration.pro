#
# Integration Test Project for CuteMarkEd-NG
#
# Github : https://github.com/waqar144/CuteMarkEd-NG
#

QT       += testlib
QT       += gui webenginewidgets

TARGET = integrationtest
CONFIG += console testcase
CONFIG += c++11

SOURCES += \
    md4cmarkdownconvertertest.cpp \
    htmlpreviewcontrollertest.cpp \
    htmltemplatetest.cpp \
    jsonsnippetfiletest.cpp \
    jsonthemefiletest.cpp \
    main.cpp \
    revealmarkdownconvertertest.cpp \
    themecollectiontest.cpp

HEADERS += \
    md4cmarkdownconvertertest.h \
    htmlpreviewcontrollertest.h \
    htmltemplatetest.h \
    jsonsnippetfiletest.h \
    jsonthemefiletest.h \
    revealmarkdownconvertertest.h \
    themecollectiontest.h

target.CONFIG += no_default_install

#
# JSON configuration library
#
INCLUDEPATH += $$PWD/../../libs/jsonconfig

#
# Add search paths below /usr/local for Mac OSX
#
macx {
  LIBS += -L/usr/local/lib
  INCLUDEPATH += /usr/local/include
}

##################################################
# Use internal static library: app-static
##################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../app-static/release/ -lapp-static
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../app-static/debug/ -lapp-static
else:unix: LIBS += -L$$OUT_PWD/../../app-static/ -lapp-static

INCLUDEPATH += $$PWD/../../app-static
DEPENDPATH += $$PWD/../../app-static

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/release/libapp-static.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/debug/libapp-static.a
else:win32-msvc*:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/release/app-static.lib
else:win32-msvc*:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/debug/app-static.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../app-static/libapp-static.a

INCLUDEPATH += $$PWD/../../3rdparty/
DEPENDPATH += $$PWD/../../3rdparty/
