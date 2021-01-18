import QtQuick
import QtQuick.Controls
import Qt.labs.folderlistmodel
import Qt.labs.platform 1.1
import Qt.labs.settings
import QtQuick.Layouts
import Hdrv 1.0

ListView {
  property color activeColor: '#FAFAFA'
  property color inactiveColor: '#C0C0C0'

  id: imageTabList
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
      height: 28
      width: imageTabText.width + 85

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
          anchors.bottom: parent.bottom
          running: busy
        }

        Image {
          id: imageTabIcon
          anchors.left: parent.left
          anchors.top: parent.top
          anchors.leftMargin: 3
          anchors.topMargin: 4
          source: 'qrc:/hdrv/media/' + fileType + '.png'
          visible: !busy
        }

        Text {
          id: imageTabText
          anchors.left: imageTabIcon.right
          anchors.top: parent.top
          anchors.leftMargin: 4
          anchors.topMargin: 5
          text: name;
        }

        RowLayout {
          id: buttonArea
          anchors.top: parent.top
          anchors.topMargin: 2
          anchors.right: parent.right
          anchors.rightMargin: 3
          spacing: 0

          ToolButton {
            id: compareImageButton
            Layout.alignment: Qt.AlignVCenter
            visible: !isComparison && !images.current.isComparison && images.current.url != url
            icon.source: 'qrc:/hdrv/media/Compare.png'
            icon.color: compareImageButton.hovered ? 'black' : '#505050'
            background: null
            onClicked: images.compare(index)
            ToolTip.visible: hovered
            ToolTip.delay: 500
            ToolTip.text: 'Compare current image to this image.'
          }

          ToolButton {
            id: closeImageButton
            Layout.alignment: Qt.AlignVCenter
            icon.source: 'qrc:/hdrv/media/Close.png'
            icon.color: closeImageButton.hovered ? 'black' : '#505050'
            background: null
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
    ToolButton {
      anchors.left: parent.left
      anchors.top: parent.top
      anchors.leftMargin: 5
      anchors.topMargin: 4
      height: 28
      icon.source: 'qrc:/hdrv/media/Open.png'
      background: Rectangle {
        color: parent.hovered ? '#F0C020' : inactiveColor
      }
      onClicked: loadImageDialog.open()
    }

    FileDialog {
      id: loadImageDialog
      title: 'Choose an image file'
      folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)
      fileMode: FileDialog.OpenFiles
      nameFilters: [ 'Supported image files (' + images.supportedFormats().join(' ') + ')',
                     'Image files (*.png *.jpg *.bmp *.gif)',
                     'HDR image files (*.hdr *.pic *.pfm *.ppm *.exr)',
                     'All files (*)' ]
      onAccepted: {
        for (var i = 0; i < loadImageDialog.files.length; ++i) {
          images.load(loadImageDialog.files[i]);
        }
      }
    }
  }
}
