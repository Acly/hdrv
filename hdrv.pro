TEMPLATE = app

QT += qml quick
CONFIG += qt

INCLUDEPATH += src dependencies/include \
               $$BOOST_ROOT

win32:Debug:LIBS += -L$$PWD/dependencies/lib_win64_vc14_debug -lpic -lpfm
win32:Release:LIBS += -L$$PWD/dependencies/lib_win64_vc14_release -lpic -lpfm

HEADERS += src/image/Image.hpp \
           src/model/ImageCollection.hpp \
           src/model/ImageDocument.hpp \
           src/view/ImageArea.hpp \
           src/view/ImageRenderer.hpp

SOURCES += src/Main.cpp \
           src/image/Image.cpp \
           src/model/ImageCollection.cpp \
           src/model/ImageDocument.cpp \
           src/view/ImageArea.cpp \
           src/view/ImageRenderer.cpp

RESOURCES += hdrv.qrc

OTHER_FILES = src/view/ErrorMessage.qml \
              src/view/ImageProperties.qml \
              src/view/Main.qml \
              src/view/TabBar.qml
