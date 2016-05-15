import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import Qt.labs.folderlistmodel 1.0
import Qt.labs.settings 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import Hdrv 1.0

ListView {
  property color activeColor: '#FAFAFA'
  property color inactiveColor: '#C0C0C0'

  id: imageTabList
  anchors.fill: parent
  orientation: Qt.Horizontal
  spacing: 2
  focus: false
  model: images.items
  delegate: imageTabDelegate
  currentIndex: images.currentIndex

  Component {
    id: imageTabDelegate
    Item {
      id: imageTabItem
      height: 24
      width: imageTabText.width + 77

      function closeTab(index) {
        if (images.items.length > 1) images.remove(index);
        else Qt.quit();
      }

      Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 4
        anchors.topMargin: 4
        color: imageTabItem.ListView.isCurrentItem ? activeColor : inactiveColor
        height: parent.height - 4

        BusyIndicator {
          id: busyIndicator
          anchors.left: parent.left
          anchors.top: parent.top
          anchors.leftMargin: 3
          anchors.topMargin: 2
          width: imageTabIcon.width
          height: imageTabIcon.height
          running: busy
        }

        Image {
          id: imageTabIcon
          anchors.left: parent.left
          anchors.top: parent.top
          anchors.leftMargin: 3
          anchors.topMargin: 2
          source: 'qrc:/hdrv/media/' + fileType + '.png'
          visible: !busy
        }

        Text {
          id: imageTabText
          anchors.left: imageTabIcon.right
          anchors.top: parent.top
          anchors.leftMargin: 4
          anchors.topMargin: 3
          text: name;
        }

        RowLayout {
          id: buttonArea
          anchors.verticalCenter: parent.verticalCenter
          anchors.right: parent.right
          anchors.rightMargin: 3
          spacing: 0

          Button {
            id: compareImageButton
            visible: !isComparison && !images.current.isComparison && images.current.url != url
            Layout.alignment: Qt.AlignVCenter
            style: ButtonStyle {
              background: Rectangle {
                implicitWidth: 13
                implicitHeight: 13
                radius: 6
                color: control.hovered ? '#0080F0' : 'transparent'
                Image {
                  anchors.fill: parent
                  anchors.margins: 2
                  source: control.hovered ? 'qrc:/hdrv/media/CompareActive.png' : 'qrc:/hdrv/media/Compare.png'
                }
              }
            }
            onClicked: images.compare(index)
          }

          Button {
            id: closeImageButton
            Layout.alignment: Qt.AlignVCenter
            style: ButtonStyle {
              background: Rectangle {
                implicitWidth: 13
                implicitHeight: 13
                radius: 6
                color: control.hovered ? '#F00000' : 'transparent'
                Image {
                  anchors.fill: parent
                  anchors.margins: 2
                  source: control.hovered ? 'qrc:/hdrv/media/CloseActive.png' : 'qrc:/hdrv/media/Close.png'
                }
              }
            }
            onClicked: closeTab(index)
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
          anchors.right: buttonArea.left
          anchors.top: parent.top
          anchors.bottom: parent.bottom
          acceptedButtons: Qt.LeftButton | Qt.MiddleButton
          onClicked: {
            if(mouse.button == Qt.MiddleButton) closeTab(index)
            else images.currentIndex = index
          }
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
      nameFilters: [ 'Supported image files (' + images.supportedFormats().join(' ') + ')', 'Image files (*.png *.jpg *.bmp *.gif)', 'HDR image files (*.hdr *.pic *.pfm *.ppm *.exr)', 'All files (*)' ]
      selectMultiple: true
      onAccepted: {
        for (var i = 0; i < loadImageDialog.fileUrls.length; ++i) {
          images.load(loadImageDialog.fileUrls[i]);
        }
      }
    }
  }
}
