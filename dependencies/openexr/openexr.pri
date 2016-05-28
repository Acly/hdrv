# OpenEXR library.
# Windows: includes and binaries for version 2.2 are checked in
# Linux: searches for version 2.2 on the system by default. Set
#        OPENEXR_VERSION to eg. 1.6 to search for older versions.

isEmpty(OPENEXR_VERSION) {
  OPENEXR_VERSION = "2.2"
}
equals(OPENEXR_VERSION, "2.2") {
  OPENEXR_INCLUDES = $$PWD/include
  win32:Debug:OPENEXR_LIBS += -L$$PWD/lib_win64_vc14_debug
  win32:Release:OPENEXR_LIBS += -L$$PWD/lib_win64_vc14_release
  OPENEXR_LIBS += -lHalf -lIlmThread-2_2 -lIlmImf-2_2
} else {
  # some older version, need to fix broken include directives on linux
  unix:OPENEXR_INCLUDES = /usr/include/OpenEXR
  OPENEXR_LIBS = -lHalf -lIlmImf
}

