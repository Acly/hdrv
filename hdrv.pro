TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = pic \
          pfm \
          viewer

pic.subdir = dependencies/pic
pfm.subdir = dependencies/pfm
viewer.subdir = viewer
viewer.depends = pic pfm
