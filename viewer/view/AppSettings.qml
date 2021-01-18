import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Hdrv 1.0

Rectangle {
  color: '#FAFAFA'

  ColumnLayout {
    spacing: 10
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.topMargin: 10
    anchors.leftMargin: 10
    anchors.rightMargin: 10

    Text { text: '<b>Single Instance</b>' }

    Switch {
      id: singleInstanceSwitch
      Layout.fillWidth: true
      text: singleInstanceSwitch.checked ? 'Enabled' : 'Disabled'
      checked: server.running
      onClicked: {
        if(server.running) {
          server.stop();
        } else {
          client.remoteStopServer();
          server.start();
        }
      }
    }

    Text {
      Layout.fillWidth: true
      text: 'Images are always opened in a new tab if HDRV is already running.'
      wrapMode: Text.Wrap
    }

    CheckBox {
      Layout.fillWidth: true
      text: 'Enable on startup'
      checked: settings.singleInstance
      onClicked: settings.singleInstance = this.checked
    }

    Text {
      text: '<b>Thumbnails</b>'
      visible: settings.thumbnailsAvailable
    }
    Text {
      Layout.fillWidth: true
      text: 'Provides thumbnails for supported HDR formats in Windows Explorer.'
      visible: settings.thumbnailsAvailable
      wrapMode: Text.Wrap
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

  }
}
