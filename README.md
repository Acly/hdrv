# HDR Viewer

GUI which displays HDR images, no bullshit.

![Screenshots](/media/Screenshots.png?raw=true)

## Features

* Opens popular HDR image formats: Radiance PIC (_*.pic, *.hdr_), PFM (_*.pfm, *.ppm_), OpenEXR (_*.exr_)
* Exports images in Radiance PIC, PFM or OpenEXR format
* Fast zoom, pan and brightness control
* Manage multiple image documents in tabs
* Compare opened images (absolute difference or side-by-side)
* Thumbnails in Windows shell

## Build

[![TravisBuildStatus](https://travis-ci.org/Acly/hdrv.svg?branch=master)](https://travis-ci.org/Acly/hdrv)
![AppVeyorBuildStatus](https://ci.appveyor.com/api/projects/status/github/Acly/hdrv?branch=master&svg=true)

### Prerequisites
* C++ 17 compiler
* CMake
* Qt 6

### Checkout

```
git clone https://github.com/Acly/hdrv.git hdrv
```

### Windows - Visual Studio 2019
Open the folder in Visual Studio to configure and build using its CMake/Ninja integration.

Depending on where Qt6 is installed you may have to configure CMake with `-DCMAKE_PREFIX_PATH=<QT_ROOT_DIR>`.

After building, runtime libraries can be copied with `windeployqt --qmldir viewer/view build/release/hdrv.exe`.


### Linux
Build from the command line using the typical CMake workflow.
```
mkdir build
cd build
cmake ..
make
```


## Use

Load images by supplying them as arguments to the hdrv executable, drag-and-drop them into the viewer or
use the _Open image_ button in the tab bar.

### Mouse controls

* \[ **Pan** \] Hold the left mouse button to view different regions of the image if it does not fit on the screen.
* \[ **Zoom** \] Use the mouse wheel to scale the image.
* \[ **Compare** \] Hold the right mouse button in comparison mode to move the image comparison separator.

### Keyboard shortcuts

* \[ **+**/**-** \] Increase / decrease the image brightness.
* \[ **Left**/**Right** \] Iterate through images in the current folder.
* \[ **1**/**2**/**3**/... \] Switch to image tab 1, 2, 3, ...
* \[ **S** \] Toggle between the last two image tabs.
* \[ **C** \] Open comparison mode for the last two images.
* \[ **R** \] Reset positioning and scaling of the image.

#### Thumbnails

A thumbnail shell integration is present for Windows in the subproject _thumbnails_.
On x86 Windows installations the project must be compiled as x86 target and vice versa for x64 Windows installations to be compatible.

You can use the buttons in the UI to (un)install the thumbnail handler.
Note that you should not move/delete the `hdrv_thumbnails.dll` without uninstalling it beforehand.

To register the extension manually use an **elevated shell** and type:
```
regsvr32 hdrv_thumbnails.dll
```
Removing the extension works by adding `/u` to the command:
```
regsvr32 /u hdrv_thumbnails.dll
```

## TODO

* Show more stats (average / maximum / minimum color)
* More options for tone mapping
* EXR: support tiled images
