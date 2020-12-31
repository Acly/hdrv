import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Material
import Hdrv 1.0


ApplicationWindow {
  width: 1200
  height: 800
  visible: true
  color: 'black'
  title: images.current.name + ' - hdrv 0.6'
  Material.accent: Material.DeepOrange

  function loadNextFile(prev) {
    var url = images.nextFile(prev);
    if (url != "") {
      images.replace(images.currentIndex, url);
    }
  }

  ColumnLayout {
    anchors.fill: parent
    spacing: 0

    Rectangle {
      color: '#404040'
      Layout.preferredHeight: 28
      Layout.fillWidth: true

      RowLayout {
        anchors.fill: parent

        TabNav {
          Layout.fillWidth: true
          Layout.fillHeight: true
        }

        ToolButton {
          id: imagePropertiesButton
          Layout.alignment: Qt.AlignBottom
          Layout.preferredHeight: parent.height - 4
          Layout.preferredWidth: settingsButton.height
          checkable: true
          contentItem: Image {
            source: 'qrc:/hdrv/media/Properties.png'
          }
          background: Rectangle {
            color: parent.hovered || parent.checked ? '#FAFAFA' : '#C0C0C0'
          }
          onClicked: if (imagePropertiesButton.checked) settingsButton.checked = false;
        }

        ToolButton {
          id: settingsButton
          Layout.alignment: Qt.AlignBottom
          Layout.preferredHeight: parent.height - 4
          Layout.preferredWidth: settingsButton.height
          Layout.rightMargin: 4
          checkable: true
          contentItem: Image {
            source: 'qrc:/hdrv/media/Settings.png'
          }
          background: Rectangle {
            color: parent.hovered || parent.checked ? '#FAFAFA' : '#C0C0C0'
          }
          onClicked: if (settingsButton.checked) imagePropertiesButton.checked = false;
        }
      }
    }

    Rectangle {
      Layout.preferredHeight: 4
      Layout.fillWidth: true
      color: '#FAFAFA'
    }

    RowLayout {
      Layout.fillWidth: true
      Layout.fillHeight: true
      spacing: 0

      ImageArea {
        color: 'black'
        model: images
        Layout.fillHeight: true
        Layout.fillWidth: true

        DropArea {
            id: dropArea;
            anchors.fill: parent;
            onEntered: drag.accept(Qt.CopyAction);
            onDropped: {
              for (var i = 0; i < drop.urls.length; ++i) {
                images.load(drop.urls[i]);
              }
            }
        }

        Rectangle {
          id: errorBox
          anchors.left: parent.left
          anchors.right: parent.right
          anchors.top: parent.top
          anchors.margins: 10
          height: Math.max(errorTextLabel.contentHeight, errorCloseButton.height) + 20
          color: '#C0C0C0'
          radius: 10
          visible: images.current.errorText.join('').length > 0

          Text {
            id: errorTextLabel
            anchors.left: parent.left
            anchors.right: errorCloseButton.left
            anchors.top: parent.top
            height: parent.height
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: images.current.errorText.join('').replace(/^\n+|\n+$/g, '')
            color: '#000000'
            styleColor: '#CCCCCC'
          }

          Button {
            id: errorCloseButton
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.rightMargin: 10
            text: "Close"
            onClicked: images.current.resetError()
          }
        }
      }

      Rectangle {
        Layout.fillHeight: true
        Layout.preferredWidth: 1
        color: 'darkgrey'
      }

      ImageProperties {
        id: imageProperties
        visible: imagePropertiesButton.checked
        enabled: imagePropertiesButton.checked
        focus: imagePropertiesButton.checked
        Layout.fillHeight: true
        Layout.minimumWidth: 280
      }

      AppSettings {
        id: settingsPane
        visible: settingsButton.checked
        Layout.fillHeight: true
        Layout.minimumWidth: 280
      }
    }

    Shortcut {
      sequence: '+'
      context: Qt.ApplicationShortcut
      onActivated: images.current.brightness = Math.round(images.current.brightness + 1.0)
    }

    Shortcut {
      sequence: '-'
      context: Qt.ApplicationShortcut
      onActivated: images.current.brightness = Math.round(images.current.brightness - 1.0)
    }

    Shortcut {
      sequence: 'R'
      context: Qt.ApplicationShortcut
      onActivated: {
        images.current.position = Qt.point(0, 0)
        images.current.scale = 1.0
      }
    }

    Shortcut {
      sequence: StandardKey.Close
      context: Qt.ApplicationShortcut
      onActivated: {
        if (images.items.length > 1) images.remove(images.currentIndex);
        else Qt.quit();
      }
    }

    Shortcut {
      sequence: StandardKey.MoveToPreviousChar
      context: Qt.ApplicationShortcut
      onActivated: loadNextFile(true)
    }

    Shortcut {
      sequence: StandardKey.MoveToNextChar
      context: Qt.ApplicationShortcut
      onActivated: loadNextFile(false)
    }

    Shortcut {
      sequence: 'C'
      context: Qt.ApplicationShortcut
      onActivated: {
        if(images.current.isComparison) {
          images.current.comparisonMode = ImageDocument.Difference;
        } else {
          if(images.recentItems.length > 1) {
            var i = images.recentItems[images.recentItems.length-2];
            if(!images.items[i].isComparison) {
              images.compare(i);
            }
          }
        }
      }
    }

    Shortcut {
      sequence: 'S'
      context: Qt.ApplicationShortcut
      onActivated: {
        if(images.recentItems.length > 1) {
          var i = images.recentItems[images.recentItems.length-2];
          images.currentIndex = i;
        }
      }
    }
  }
}
