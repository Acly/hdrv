import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import Hdrv 1.0

Rectangle {
  color: '#FAFAFA'

  GridLayout {
    columns: 2
    columnSpacing: 5
    rowSpacing: 5
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.topMargin: 10

    Text { text: 'Zoom'; Layout.columnSpan: 2 }

    TextField {
      id: 'scaleText'
      Layout.fillWidth: true
      validator: DoubleValidator {
        notation: DoubleValidator.StandardNotation
        decimals: 3
      }
      focus: true
      text: Math.round(images.current.scale * 10000.0) / 10000.0
      onEditingFinished: images.current.scale = scaleText.text
    }

    Text {
      text: 'x'
      MouseArea {
        anchors.fill: parent
        onDoubleClicked: images.current.scale = 1.0
      }
    }

    Text { text: 'Brightness'; Layout.columnSpan: 2 }

    Slider {
      id: 'brightnessSlider'
      Layout.fillWidth: true
      minimumValue: images.current.minBrightness
      maximumValue: images.current.maxBrightness
      stepSize: 0.1
      value: images.current.brightness
      property bool initialized: false // workaround, slider sets 1.0 at start up (?)
      onValueChanged: if (initialized) images.current.brightness = brightnessSlider.value;
      Component.onCompleted: initialized = true
    }

    Text {
      text: brightnessSlider.value.toFixed(1)
      MouseArea {
        anchors.fill: parent
        onDoubleClicked: images.current.brightness = 0.0
      }
    }

    Text { text: 'Gamma'; Layout.columnSpan: 2 }

    Slider {
      id: 'gammaSlider'
      Layout.fillWidth: true
      minimumValue: images.current.minGamma
      maximumValue: images.current.maxGamma
      stepSize: 0.1
      value: images.current.gamma
      property bool initialized: false // workaround, slider sets 1.0 at start up (?)
      onValueChanged: if (initialized) images.current.gamma = gammaSlider.value;
      Component.onCompleted: initialized = true
      enabled: images.current.isFloat
    }

    Text {
      text: gammaSlider.value.toFixed(1)
      MouseArea {
        anchors.fill: parent
        onDoubleClicked: images.current.gamma = 2.2
      }
    }
  }

  function vectorGet(vec, i) {
    return i == 0 ? vec.x : (i == 1 ? vec.y : (i == 2 ? vec.z : vec.w));
  }

  GridLayout {
    columns: 2
    columnSpacing: 5
    rowSpacing: 5
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.bottomMargin: 10

    Text { text: 'Resolution:' }
    Text { text: images.current.size.width + ' x ' + images.current.size.height }
    Text { text: 'Cursor position:' }
    Text { text: images.current.pixelPosition.x + ', ' + images.current.pixelPosition.y }
    Text { text: 'Cursor texel value'; Layout.columnSpan: 2 }
    Repeater {
      model: images.current.channels
      Text {
        text: '  Channel ' + (index+1) + ': ' + vectorGet(images.current.pixelValue, index);
        Layout.columnSpan: 2
      }
    }
  }

}
