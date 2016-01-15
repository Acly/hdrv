TEMPLATE = app

QT += qml quick
CONFIG += qt

INCLUDEPATH += src dependencies/include \
               $$BOOST_ROOT

LIBS += -L$$PWD/dependencies/lib_win64_vc14_debug \
        -lpic \
        -lpfm

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
