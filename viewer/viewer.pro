TEMPLATE = app

QT += qml quick concurrent
CONFIG += qt
CONFIG += c++14

include(../dependencies/boost/boost.pri)
include(../dependencies/openexr/openexr.pri)

INCLUDEPATH += . \ 
               ../dependencies/pic/include \
               ../dependencies/pfm/include \
               $$BOOST_ROOT \
               $$OPENEXR_INCLUDES

Debug:LIBS += -L$$OUT_PWD/../dependencies/pic/debug \
              -L$$OUT_PWD/../dependencies/pfm/debug
Release:LIBS += -L$$OUT_PWD/../dependencies/pic/release \
                -L$$OUT_PWD/../dependencies/pfm/release
LIBS += -L$$OUT_PWD/../dependencies/pic \
        -L$$OUT_PWD/../dependencies/pfm

LIBS += -lpic -lpfm $$OPENEXR_LIBS

win32:RC_FILE = ../media/hdrv.rc

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
