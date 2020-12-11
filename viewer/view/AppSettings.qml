import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Hdrv 1.0

Rectangle {
  color: '#FAFAFA'

  Column {
    spacing: 10
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.topMargin: 10
    anchors.leftMargin: 10
    anchors.rightMargin: 10

    Text {
      text: '<b>Thumbnails</b>'
      visible: settings.thumbnailsAvailable
    }
    Text {
      text: 'Provides thumbnails for supported<br> HDR formats in Windows Explorer.'
      visible: settings.thumbnailsAvailable
    }

    RowLayout {
      Layout.fillWidth: true
      spacing: 10
      visible: settings.thumbnailsAvailable

      Button {
        Layout.fillWidth: true
        text: 'Install'
        onClicked: settings.install()
      }

      Button {
        Layout.fillWidth: true
        text: 'Uninstall'
        onClicked: settings.uninstall()
      }
    }

    Text {
      text: '<b>Startup</b>'
    }

    CheckBox {
      Layout.fillWidth: true
      text: 'Start application in single-instance mode.'
      checked: settings.singleInstance
      onClicked: settings.singleInstance = this.checked
    }
  }
}
