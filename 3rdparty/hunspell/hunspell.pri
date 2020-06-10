INCLUDEPATH += $$PWD/hunspell/src

win32 {
    message("Hunspell Windows Build")
    DEFINES += BUILDING_LIBHUNSPELL
}

HEADERS += \
    $$PWD/hunspell/src/hunspell/affentry.hxx \
    $$PWD/hunspell/src/hunspell/affixmgr.hxx \
    $$PWD/hunspell/src/hunspell/atypes.hxx \
    $$PWD/hunspell/src/hunspell/baseaffix.hxx \
    $$PWD/hunspell/src/hunspell/csutil.hxx \
    $$PWD/hunspell/src/hunspell/filemgr.hxx \
    $$PWD/hunspell/src/hunspell/hashmgr.hxx \
    $$PWD/hunspell/src/hunspell/htypes.hxx \
    $$PWD/hunspell/src/hunspell/hunspell.h \
    $$PWD/hunspell/src/hunspell/hunspell.hxx \
    $$PWD/hunspell/src/hunspell/hunvisapi.h \
    $$PWD/hunspell/src/hunspell/hunvisapi.h.in \
    $$PWD/hunspell/src/hunspell/hunzip.hxx \
    $$PWD/hunspell/src/hunspell/langnum.hxx \
    $$PWD/hunspell/src/hunspell/phonet.hxx \
    $$PWD/hunspell/src/hunspell/replist.hxx \
    $$PWD/hunspell/src/hunspell/suggestmgr.hxx \
    $$PWD/hunspell/src/hunspell/utf_info.hxx \
    $$PWD/hunspell/src/hunspell/w_char.hxx \
    $$PWD/hunspell/src/parsers/firstparser.hxx \
    $$PWD/hunspell/src/parsers/htmlparser.hxx \
    $$PWD/hunspell/src/parsers/latexparser.hxx \
    $$PWD/hunspell/src/parsers/manparser.hxx \
    $$PWD/hunspell/src/parsers/odfparser.hxx \
    $$PWD/hunspell/src/parsers/textparser.hxx \
    $$PWD/hunspell/src/parsers/xmlparser.hxx

SOURCES += \
    $$PWD/hunspell/src/hunspell/affentry.cxx \
    $$PWD/hunspell/src/hunspell/affixmgr.cxx \
    $$PWD/hunspell/src/hunspell/csutil.cxx \
    $$PWD/hunspell/src/hunspell/filemgr.cxx \
    $$PWD/hunspell/src/hunspell/hashmgr.cxx \
    $$PWD/hunspell/src/hunspell/hunspell.cxx \
    $$PWD/hunspell/src/hunspell/hunzip.cxx \
    $$PWD/hunspell/src/hunspell/phonet.cxx \
    $$PWD/hunspell/src/hunspell/replist.cxx \
    $$PWD/hunspell/src/hunspell/suggestmgr.cxx \
    $$PWD/hunspell/src/parsers/firstparser.cxx \
    $$PWD/hunspell/src/parsers/htmlparser.cxx \
    $$PWD/hunspell/src/parsers/latexparser.cxx \
    $$PWD/hunspell/src/parsers/manparser.cxx \
    $$PWD/hunspell/src/parsers/odfparser.cxx \
#    $$PWD/hunspell/src/parsers/testparser.cxx \
    $$PWD/hunspell/src/parsers/textparser.cxx \
    $$PWD/hunspell/src/parsers/xmlparser.cxx

