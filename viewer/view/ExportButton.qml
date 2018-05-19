import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.4

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

  property Component subButtonStyle: Component {
    ButtonStyle {
      background: Rectangle {
        implicitWidth: 40
        border.color: control.pressed ? '#888' : 'transparent'
        gradient: Gradient {
          GradientStop { position: 0; color: control.pressed ? '#ddd' : 'transparent' }
          GradientStop { position: 1; color: control.pressed ? '#ccc' : 'transparent' }
        }
      }
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
      style: ButtonStyle {
        background: Rectangle { color: 'transparent' }
      }
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
      style: subButtonStyle
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
      style: subButtonStyle
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
      style: subButtonStyle
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
      style: subButtonStyle
    }

    FileDialog {
      id: storeImageDialog
      title: 'Choose a filename'
      folder: images.current.directory
      nameFilters: [ 'Radiance HDR (*.hdr)', 'PFM image (*.pfm)', 'OpenEXR image (*.exr)', 'PNG image (*.png)' ]
      selectExisting: false
      onAccepted: images.current.store(storeImageDialog.fileUrl);
    }
  }
}
