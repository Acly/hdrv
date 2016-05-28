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

The only prerequisite is that Qt 5.5 or newer is installed on the system.

### Checkout

```
git clone https://github.com/Acly/hdrv.git hdrv
cd hdrv
git submodule update --init
mkdir build
cd build   
```

Updating submodules will download the required [boost](http://www.boost.org/) modules. You
can skip this step and use an existing boost installation by passing `"BOOST_ROOT=/path/to/boost/include"`
to qmake in the next step. 

### Windows - VC14
```
qmake -tp vc -r ../hdrv.pro
```

This generates a Visual Studio solution which can be used to build the application.
After building, runtime libraries can be copied with `windeployqt --qmldir ../viewer/view viewer/release/hdrv.exe`.

Tested using Visual Studio 2015 Update 2 x64.

### Linux

Building requires a C++14 compiler and [OpenEXR 2.2.0](http://www.openexr.com/)
to be installed. Most distributions have precompiled packages available. To build run:

```
qmake .. && make
```

Tested using GCC 5.

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
* High quality tone mapping
* Better support for existing formats (encodings, layouts)
