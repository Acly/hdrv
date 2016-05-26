TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += . ../include

HEADERS += config.hpp \
           ../include/pic/pic.hpp \
           ../include/pic/pixel.hpp \
           ../include/pic/pic_input_file.hpp \
           ../include/pic/pic_output_file.hpp

SOURCES += pic_input_file.cpp pic_output_file.cpp
