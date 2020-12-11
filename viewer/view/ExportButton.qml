import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform 1.1

Rectangle {
  border.width: exportButton.activeFocus ? 2 : 1
  border.color: '#888'
  radius: 4
  height: 22
  gradient: Gradient {
    GradientStop { position: 0 ; color: exportButton.pressed ? '#ddd' : '#eee' }
    GradientStop { position: 1 ; color: exportButton.pressed ? '#ccc' : '#ddd' }
  }

  property bool exportable: images.current.isFloat && !images.current.isComparison

  property Rectangle subButtonStyle: Rectangle {
    implicitWidth: 40
    border.color: control.pressed ? '#888' : 'transparent'
    gradient: Gradient {
      GradientStop { position: 0; color: control.pressed ? '#ddd' : 'transparent' }
      GradientStop { position: 1; color: control.pressed ? '#ccc' : 'transparent' }
    }
  }

  RowLayout {
    anchors.fill: parent
    spacing: 0

    Button {
      id: exportButton
      text: 'Export'
      Layout.fillWidth: true
      enabled: exportable
      onClicked: storeImageDialog.open()
      background: Rectangle { color: 'transparent' }
    }

    Rectangle {
      width: 1
      Layout.fillHeight: true
      color: '#888'
    }

    Button {
      id: exportHDRButton
      text: 'HDR'
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter = 'Radiance HDR (*.hdr)'
        storeImageDialog.open()
      }
      background: subButtonStyle
    }

    Rectangle {
      width: 1
      Layout.fillHeight: true
      color: '#888'
    }

    Button {
      id: exportPFMButton
      text: 'PFM'
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter = 'PFM image (*.pfm)'
        storeImageDialog.open()
      }
      background: subButtonStyle
    }
    
    Rectangle {
      width: 1
      Layout.fillHeight: true
      color: '#888'
    }
    
    Button {
      id: exportEXRButton
      text: 'EXR'
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter = 'OpenEXR image (*.exr)'
        storeImageDialog.open()
      }
      background: subButtonStyle
    }

    Rectangle {
      width: 1
      Layout.fillHeight: true
      color: '#888'
    }

    Button {
      id: exportPNGButton
      text: 'PNG'
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter = 'PNG image (*.png)'
        storeImageDialog.open()
      }
      background: subButtonStyle
    }

    FileDialog {
      id: storeImageDialog
      title: 'Choose a filename'
      folder: images.current.directory
      fileMode: FileDialog.SaveFile
      nameFilters: [ 'Radiance HDR (*.hdr)', 'PFM image (*.pfm)', 'OpenEXR image (*.exr)', 'PNG image (*.png)' ]
      onAccepted: images.current.store(storeImageDialog.fileUrl);
    }
  }
}
