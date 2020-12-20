import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform 1.1

Control {
  property bool exportable: images.current.isFloat && !images.current.isComparison

  RowLayout {
    anchors.fill: parent
    spacing: 0

    ToolButton {
      id: exportButton
      Layout.fillWidth: true
      text: 'Export'
      enabled: exportable
      onClicked: storeImageDialog.open()
    }

    ToolSeparator {}

    ToolButton {
      id: exportHDRButton
      Layout.fillWidth: true
      text: 'HDR'
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter.index = 0
        storeImageDialog.open()
      }
    }

    ToolButton {
      id: exportPFMButton
      Layout.fillWidth: true
      text: 'PFM'
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter.index = 1
        storeImageDialog.open()
      }
    }
    
    ToolButton {
      id: exportEXRButton
      Layout.fillWidth: true
      text: 'EXR'
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter.index = 2
        storeImageDialog.open()
      }
    }

    ToolButton {
      id: exportPNGButton
      text: 'PNG'
      Layout.fillWidth: true
      enabled: exportable
      onClicked: {
        storeImageDialog.selectedNameFilter.index = 3
        storeImageDialog.open()
      }
    }

    FileDialog {
      id: storeImageDialog
      title: 'Choose a filename'
      folder: images.current.directory
      fileMode: FileDialog.SaveFile
      nameFilters: [ 'Radiance HDR (*.hdr)', 'PFM image (*.pfm)', 'OpenEXR image (*.exr)', 'PNG image (*.png)' ]
      onAccepted: images.current.store(storeImageDialog.file);
    }
  }
}
