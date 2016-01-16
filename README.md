# HDR Viewer

GUI which displays HDR images, no bullshit. Need to come up with a proper name still.

## Build

* Install Qt 5.5
* Download [boost](http://www.boost.org/) (headers are sufficient, no need to build)
* Clone the repository from `https://github.com/Acly/hdrv.git`

### Windows
```
mkdir build
cd build   
qmake -tp vc ../hdrv.pro "BOOST_ROOT=/path/to/boost"
```

This generates a project file which can be used to build the application with Visual Studio.
After building, runtime libraries can be copied with `windeployqt --qmldir ../src/view release/hdrv.exe`.

Tested using Visual Studio 2015 Update 1 x64.

### Other Platforms

When building for a different platform the following dependencies will have to be installed or built from source and provided where the linker can find them:
* [OpenEXR 2.2.0](http://www.openexr.com/)
* [libpic 0.1](http://people.cs.kuleuven.be/~ares.lagae/libpic/)
* [libpfm 0.1](http://people.cs.kuleuven.be/~ares.lagae/libpfm/)

Entirely untested, but there is no platform-dependent code or anything else any modern compiler shouldn't be able to handle.

## Use

It's a self-explanatory and intuitive GUI. Duh.

## TODO

* Show current pixel location and color under cursor
* Show more stats (average / maximum / minimum color)
* Better tone mapping
* Better support for existing formats (encodings, layouts)
* Support for OpenEXR formats
* Conversion between formats (image export)
