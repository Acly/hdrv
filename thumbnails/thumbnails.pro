TEMPLATE = lib
CONFIG += c++14 shared
#CONFIG -= qt

include(../dependencies/boost/boost.pri)
include(../dependencies/openexr/openexr.pri)

INCLUDEPATH += . \
               ../viewer \
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

Release:DESTDIR = $$OUT_PWD/../viewer/release
Debug:DESTDIR = $$OUT_PWD/../viewer/debug
        
LIBS += -lpic -lpfm $$OPENEXR_LIBS

HEADERS += Thumbnails.hpp \
           Registry.hpp \
           ThumbnailProvider.hpp \
           ClassFactory.hpp \
           StreamWrapper.hpp

SOURCES += Thumbnails.cpp \
           Registry.cpp \
           ThumbnailProvider.cpp \
           ClassFactory.cpp \
           ../viewer/image/Image.cpp