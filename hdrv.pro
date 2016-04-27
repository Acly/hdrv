TEMPLATE = app

QT += qml quick
CONFIG += qt

INCLUDEPATH += src dependencies/include \
               $$BOOST_ROOT

win32:Debug:LIBS += -L$$PWD/dependencies/lib_win64_vc14_debug \
                    -lpic -lpfm -lHalf -lIlmImf-2_2 -lIex-2_2 -lIlmThread-2_2 -lzlibstaticd
win32:Release:LIBS += -L$$PWD/dependencies/lib_win64_vc14_release \
                      -lpic -lpfm -lHalf -lIlmImf-2_2 -lIex-2_2 -lIlmThread-2_2 -lzlibstatic
win32:RC_FILE = media/hdrv.rc

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
