import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import Qt.labs.folderlistmodel 1.0
import Qt.labs.settings 1.0
import QtQuick.Dialogs 1.2
import Hdrv 1.0

ListView {
  property color activeColor: '#FAFAFA'
  property color inactiveColor: '#C0C0C0'

  id: imageTabList
  anchors.fill: parent
  orientation: Qt.Horizontal
  spacing: 2
  focus: true
  model: images.items
  delegate: imageTabDelegate
  currentIndex: images.currentIndex

  Component {
    id: imageTabDelegate
    Item {
      id: imageTabItem
      height: 24
      width: imageTabText.width + 32

      Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 4
        anchors.topMargin: 4
        color: imageTabItem.ListView.isCurrentItem ? activeColor : inactiveColor
        height: parent.height - 4

        Text {
          id: imageTabText
          anchors.left: parent.left
          anchors.top: parent.top
          anchors.leftMargin: 4
          anchors.topMargin: 4
          text: name;
        }

        Button {
          id: closeImageButton
          anchors.verticalCenter: parent.verticalCenter
          anchors.right: parent.right
          anchors.rightMargin: 3
          width: 14
          height: 14
          text: 'x'
          style: ButtonStyle {
            background: Rectangle {
              implicitWidth: 14
              implicitHeight: 14
              radius: 7
              color: control.hovered ? '#F00000' : 'transparent'
            }
            label: Item {
              implicitWidth: buttonText.implicitWidth
              implicitHeight: buttonText.implicitHeight
              baselineOffset: buttonText.y + buttonText.baselineOffset
              Text {
                id: buttonText
                color: control.hovered ? 'white' : 'black'
                anchors.centerIn: parent
                text: control.text
              }
            }
          }
          onClicked: {
            if (images.items.length > 1) images.remove(index);
            else Qt.quit();
          }
        }

        Shortcut {
          sequence: (index + 1).toString()
          enabled: (index + 1) <= 9
          context: Qt.ApplicationShortcut
          onActivated: images.currentIndex = index
        }

        MouseArea {
          anchors.left: parent.left
          anchors.right: closeImageButton.left
          anchors.top: parent.top
          anchors.bottom: parent.bottom
          onClicked: images.currentIndex = index
        }
      }
    }
  }

  footer: Item {
    Button {
      anchors.left: parent.left
      anchors.top: parent.top
      anchors.leftMargin: 5
      anchors.topMargin: 4
      width: 25
      height: 20
      iconSource: 'qrc:/hdrv/media/Open.png'
      style: ButtonStyle {
        background: Rectangle {
          color: control.hovered ? '#FFCF10' : inactiveColor
        }
      }
      onClicked: loadImageDialog.open()
    }

    FileDialog {
      id: loadImageDialog
      title: 'Choose an image file'
      folder: shortcuts.pictures
      nameFilters: [ 'Image files (*.png *.jpg *.bmp *.gif)', 'HDR image files (*.hdr *.pic *.pfm *.ppm *.exr)', 'All files (*)' ]
      selectMultiple: true
      onAccepted: {
        for (var i = 0; i < loadImageDialog.fileUrls.length; ++i) {
          if (!images.load(loadImageDialog.fileUrls[i])) {
            errorMessageDialog.open();
          }
        }
      }
    }

    ErrorMessage { id: errorMessageDialog; show: false }
  }
}
