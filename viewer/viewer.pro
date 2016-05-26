TEMPLATE = app

QT += qml quick concurrent
CONFIG += qt
CONFIG += c++14

include(../dependencies/boost/boost.pri)

INCLUDEPATH += . ../dependencies/include $$BOOST_ROOT

Debug:LIBS += -L$$OUT_PWD/../dependencies/pic/debug \
              -L$$OUT_PWD/../dependencies/pfm/debug
Release:LIBS += -L$$OUT_PWD/../dependencies/pic/release \
                -L$$OUT_PWD/../dependencies/pfm/release
LIBS += -lpic -lpfm

win32:Debug:LIBS += -L$$PWD/../dependencies/lib_win64_vc14_debug \
                    -lHalf -lIlmImf-2_2 -lIex-2_2 -lIlmThread-2_2 -lzlibstaticd
win32:Release:LIBS += -L$$PWD/../dependencies/lib_win64_vc14_release \
                      -lHalf -lIlmImf-2_2 -lIex-2_2 -lIlmThread-2_2 -lzlibstatic
win32:RC_FILE = ../media/hdrv.rc

unix:LIBS += -lHalf -lIlmImf

HEADERS += image/Image.hpp \
           model/ImageCollection.hpp \
           model/ImageDocument.hpp \
           view/ImageArea.hpp \
           view/ImageRenderer.hpp

SOURCES += Main.cpp \
           image/Image.cpp \
           model/ImageCollection.cpp \
           model/ImageDocument.cpp \
           view/ImageArea.cpp \
           view/ImageRenderer.cpp

RESOURCES += viewer.qrc

OTHER_FILES = view/ExportButton.qml \
              view/ImageProperties.qml \
              view/Main.qml \
              view/TabBar.qml

TARGET = hdrv
