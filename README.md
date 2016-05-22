# HDR Viewer

GUI which displays HDR images, no bullshit.

![Screenshots](/media/Screenshots.png?raw=true)

## Features

* Opens popular HDR image formats: Radiance PIC (_*.pic, *.hdr_), PFM (_*.pfm, *.ppm_), OpenEXR (_*.exr_)
* Exports images in Radiance PIC or PFM format
* Fast zoom, pan and brightness control
* Manage multiple image documents in tabs
* Compare opened images (absolute difference or side-by-side)

## Build

* Install Qt 5.5
* Download [boost](http://www.boost.org/) (headers are sufficient, no need to build)
* Clone the repository from `https://github.com/Acly/hdrv.git`

### Windows - VC14
```
mkdir build
cd build   
qmake -tp vc ../hdrv.pro "BOOST_ROOT=/path/to/boost"
```

This generates a project file which can be used to build the application with Visual Studio.
After building, runtime libraries can be copied with `windeployqt --qmldir ../src/view release/hdrv.exe`.

Tested using Visual Studio 2015 Update 2 x64.

### Other Platforms

When building for a different platform the following dependencies will have to be installed or built from source and provided where the linker can find them:
* [OpenEXR 2.2.0](http://www.openexr.com/)
* [zlib 1.2.8](http://www.zlib.net)
* [libpic 0.1](http://people.cs.kuleuven.be/~ares.lagae/libpic/)
* [libpfm 0.1](http://people.cs.kuleuven.be/~ares.lagae/libpfm/)

Entirely untested, but there is no platform-dependent code or anything else any modern compiler shouldn't be able to handle.

## Use

Load images by supplying them as arguments to the hdrv executable, drag-and-drop them into the viewer or
use the _Open image_ button in the tab bar.

### Mouse controls

* \[ **Pan** \] Hold the left mouse button to view different regions of the image if it does not fit on the screen.
* \[ **Zoom** \] Use the mouse wheel to scale the image.
* \[ **Compare** \] Hold the right mouse button in comparison mode to move the image comparison separator.

### Keybaord shortcuts

* \[ **+**/**-** \] Increase / decrease the image brightness.
* \[ **Left**/**Right** \] Iterate through images in the current folder.
* \[ **1**/**2**/**3**/... \] Switch to image tab 1, 2, 3, ...
* \[ **S** \] Toggle between the last two image tabs.
* \[ **C** \] Open comparison mode for the last two images.
* \[ **R** \] Reset positioning and scaling of the image.

## TODO

* Show more stats (average / maximum / minimum color)
* Hight quality tone mapping
* Better support for existing formats (encodings, layouts)
