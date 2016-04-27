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

  RowLayout {
    anchors.fill: parent
    spacing: 0

    Button {
      id: exportButton
      text: 'Export'
      Layout.fillWidth: true
      enabled: images.current.isFloat
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
      enabled: images.current.isFloat
      onClicked: {
        storeImageDialog.selectedNameFilter = 'Radiance HDR (*.hdr)'
        storeImageDialog.open()
      }
      style: ButtonStyle {
        background: Rectangle {
          implicitWidth: 45
          border.color: control.pressed ? '#888' : 'transparent'
          gradient: Gradient {
            GradientStop { position: 0; color: control.pressed ? '#ddd' : 'transparent' }
            GradientStop { position: 1; color: control.pressed ? '#ccc' : 'transparent' }
          }
        }
      }
    }

    Rectangle {
      width: 1
      Layout.fillHeight: true
      color: '#888'
    }

    Button {
      id: exportPFMButton
      text: 'PFM'
      enabled: images.current.isFloat
      onClicked: {
        storeImageDialog.selectedNameFilter = 'PFM image (*.pfm)'
        storeImageDialog.open()
      }
      style: ButtonStyle {
        background: Rectangle {
          implicitWidth: 45
          border.color: control.pressed ? '#888' : 'transparent'
          gradient: Gradient {
            GradientStop { position: 0; color: control.pressed ? '#ddd' : 'transparent' }
            GradientStop { position: 1; color: control.pressed ? '#ccc' : 'transparent' }
          }
        }
      }
    }

    FileDialog {
      id: storeImageDialog
      title: 'Choose a filename'
      folder: images.current.directory
      nameFilters: [ 'Radiance HDR (*.hdr)', 'PFM image (*.pfm)' ]
      selectExisting: false
      onAccepted: {
        if (!images.current.store(storeImageDialog.fileUrl)) {
          errorMessageDialog.open();
        }
      }
    }

    ErrorMessage { id: errorMessageDialog; show: false }
  }
}
