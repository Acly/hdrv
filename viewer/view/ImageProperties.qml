import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import Hdrv 1.0

Rectangle {
  color: '#FAFAFA'

  function presentFloat(num, maxDecimals) {
    var str = num.toString();
    var i = str.indexOf('.');
    if (i != -1) {
      str = str.substr(0, Math.min(str.length, i + maxDecimals + 1));
    }
    return str;
  }

  GridLayout {
    columns: 3
    columnSpacing: 5
    rowSpacing: 8
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.topMargin: 10

    Text { text: '<b>Rendering</b>'; Layout.columnSpan: 3 }

    Text { text: 'Zoom:' }
    TextField {
      id: 'scaleText'
      Layout.columnSpan: 2
      validator: DoubleValidator {
        notation: DoubleValidator.StandardNotation
        decimals: 3
        bottom: 0.01
      }
      focus: true
      text: presentFloat(images.current.scale, 4)
      onEditingFinished: images.current.scale = scaleText.text
      Text {
        text: 'x'
        anchors.right: parent.right
        anchors.bottom: parent.baseline
        anchors.rightMargin: 5
      }
    }

    Text { text: 'Brightness:' }
    Slider {
      id: 'brightnessSlider'
      Layout.fillWidth: true
      Layout.minimumHeight: scaleText.height
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

    Text { text: 'Gamma:'; visible: images.current.isFloat }
    Slider {
      id: 'gammaSlider'
      Layout.fillWidth: true
      Layout.minimumHeight: scaleText.height
      minimumValue: images.current.minGamma
      maximumValue: images.current.maxGamma
      stepSize: 0.1
      value: images.current.gamma
      property bool initialized: false // workaround, slider sets 1.0 at start up (?)
      onValueChanged: if (initialized) images.current.gamma = gammaSlider.value;
      Component.onCompleted: initialized = true
      visible: images.current.isFloat
    }
    Text {
      text: gammaSlider.value.toFixed(1)
      MouseArea {
        anchors.fill: parent
        onDoubleClicked: images.current.gamma = 2.2
      }
      visible: images.current.isFloat
    }

    Text {
      text: '<b>Comparison</b>'
      Layout.columnSpan: 3
      visible: images.current.isComparison
    }

    Text {
      text: 'Mode:'
      visible: images.current.isComparison
    }
    Row {
      Layout.columnSpan: 2
      visible: images.current.isComparison
      spacing: 10

      RadioButton {
        id: differenceButton
        text: 'Difference'
        exclusiveGroup: comparisonModeGroup
      }
      RadioButton {
        id: sideBySideButton
        text: 'Side by side'
        exclusiveGroup: comparisonModeGroup
      }
      ExclusiveGroup {
        id: comparisonModeGroup
        current: images.current.comparisonMode == ImageDocument.Difference ? differenceButton : sideBySideButton
        onCurrentChanged: {
          if (current == differenceButton) images.current.comparisonMode = ImageDocument.Difference;
          else images.current.comparisonMode = ImageDocument.SideBySide;
        }
      }
    }

    Text { text: '<b>Export</b>'; Layout.columnSpan: 3 }

    ExportButton {
      Layout.columnSpan: 3
      Layout.fillWidth: true
    }

  }

  function channelName(i, numChannels, displayFormat) {
    if(numChannels < 3) {
      return ['L','A'][i];
    } else {
      if(displayFormat == 'normal') {
        return ['X','Y','Z','A'][i];
      } else {
        return ['R','G','B','A'][i];
      }
    }
  }

  function format(vec, i, numChannels, imageIsFloat, displayFormat) {
    var vec = [vec.x, vec.y, vec.z, vec.w];
    var round = function(x) { return Math.floor(x * 1000.0) / 1000.0; };

    if(displayFormat == 'float') {
      if(imageIsFloat) {
        return round(vec[i]);
      } else {
        return round(vec[i] / 255.0);
      }
    } else if(displayFormat == 'byte') {
      if(imageIsFloat) {
        var v = Math.floor(vec[i] * 255.0);
        return (v > 255) ? ('<font color="red">' + v + '</font>') : v;
      } else {
        return vec[i];
      }
    } else if(displayFormat == 'normal') {
      if(imageIsFloat) {
        if(i < 3) {
          var v = round(2.0 * vec[i] - 1.0);
          return (v < -1.0 || v > 1.0) ? ('<font color="red">' + v + '</font>') : v;
        } else {
          return round(vec[i]);
        }
      } else {
        if(i < 3) {
          if(vec[i] == 0) {
            return '<font color="red">---</font>';
          } else {
            return round((vec[i] - 128.0)/127.0);
          }
        } else {
          return round(Math.floor(vec[i] / 255.0));
        }
      }
    } else {
      return round(vec[i]);
    }
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

    Text { text: '<b>Thumbnail Handler</b>'; Layout.columnSpan: 2; visible: images.current.thumbnailsAvailable }

    RowLayout {
      Layout.columnSpan: 2
      Layout.fillWidth: true
      spacing: 10
      visible: images.current.thumbnailsAvailable

      Button {
        Layout.fillWidth: true
        text: 'Install'
        onClicked: images.current.changeThumbnailHandler(false)
      }

      Button {
        Layout.fillWidth: true
        text: 'Uninstall'
        onClicked: images.current.changeThumbnailHandler(true)
      }
    }

    Text { text: 'Resolution:' }
    Text { text: images.current.size.width + ' x ' + images.current.size.height }
    Text { text: 'Cursor position:' }
    Text { text: images.current.pixelPosition.x + ', ' + images.current.pixelPosition.y }
    Text { text: 'Cursor texel value'; Layout.columnSpan: 2 }
    Repeater {
      model: images.current.channels
      Text {
        text: channelName(index, images.current.channels, channelFormat.current.formatType) + ': ' + format(images.current.pixelValue, index, images.current.channels, images.current.isFloat, channelFormat.current.formatType);
        Layout.columnSpan: 2
        Layout.leftMargin: 10
      }
    }
    RowLayout {
      Layout.columnSpan: 2
      Layout.leftMargin: 10
      ExclusiveGroup {
        id: channelFormat
      }
      RadioButton {
        property string formatType: 'auto'
        text: 'Auto'
        exclusiveGroup: channelFormat
        checked: true
      }
      RadioButton {
        property string formatType: 'float'
        text: 'Float'
        exclusiveGroup: channelFormat
      }
      RadioButton {
        property string formatType: 'byte'
        text: 'Byte'
        exclusiveGroup: channelFormat
      }
      RadioButton {
        property string formatType: 'normal'
        text: 'Normal'
        exclusiveGroup: channelFormat
        visible: image.current.channels >= 3
      }
    }
  }
}
