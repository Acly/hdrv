TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += . ../include

HEADERS += config.hpp \
           ../include/pfm/byte_order.hpp \
           ../include/pfm/color_pixel.hpp \
           ../include/pfm/pfm.hpp \
           ../include/pfm/pfm_input_file.hpp \
           ../include/pfm/pfm_output_file.hpp

SOURCES += pfm_input_file.cpp pfm_output_file.cpp
