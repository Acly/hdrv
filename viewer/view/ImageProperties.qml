import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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

  ColumnLayout {
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.top: parent.top
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.topMargin: 10

    spacing: 10

    ColumnLayout {
      visible: images.current.hasLayers
      Layout.fillWidth: true
      Text { text: '<b>Image</b>' }
      ComboBox {
        Layout.fillWidth: true
        model: images.current.layers
        onCurrentIndexChanged: images.current.layer = currentIndex
        currentIndex: images.current.layer
      }
    }

    ColumnLayout {
      Layout.fillWidth: true
      Text {
        id: renderingHeader
        text: '<b>Rendering</b>'
      }

      Text {
        text: 'Zoom: ' + images.current.scale + 'x'
        MouseArea {
          anchors.fill: parent
          onDoubleClicked: images.current.scale = 1.0
        }
      }
      Slider {
        id: scaleSlider
        Layout.fillWidth: true
        from: -4
        to: 4
        stepSize: 1.0
        value: Math.log2(images.current.scale)
        onValueChanged: images.current.scale = Math.pow(2, scaleSlider.value)
      }

      Text {
        text: 'Brightness: ' + (brightnessSlider.value >= 0 ? '+' : '') + brightnessSlider.value.toFixed(1)
        MouseArea {
          anchors.fill: parent
          onDoubleClicked: images.current.brightness = 0.0
        }
      }
      Slider {
        id: brightnessSlider
        Layout.fillWidth: true
        from: images.current.minBrightness
        to: images.current.maxBrightness
        stepSize: 0.1
        value: images.current.brightness
        onValueChanged: images.current.brightness = brightnessSlider.value;
      }

      Text {
        text: 'Gamma: ' + gammaSlider.value.toFixed(1)
        visible: images.current.isFloat
        MouseArea {
          anchors.fill: parent
          onDoubleClicked: images.current.gamma = 2.2
        }
      }
      Slider {
        id: gammaSlider
        Layout.fillWidth: true
        from: images.current.minGamma
        to: images.current.maxGamma
        stepSize: 0.1
        value: images.current.gamma
        onValueChanged: images.current.gamma = gammaSlider.value;
        visible: images.current.isFloat
      }

      Text { text: 'Display Mode' }
      ComboBox {
        id: 'displayModeSelection'
        Layout.fillWidth: true
        model: [ "Default", "No Alpha", "Alpha Only" ]
        onCurrentIndexChanged: images.current.displayMode = currentIndex;
        currentIndex: images.current.displayMode
      }
    }

    ColumnLayout {
      Text {
        text: '<b>Comparison</b>'
        visible: images.current.isComparison
      }
      Row {
        RadioButton {
          id: differenceButton
          text: 'Difference'
          visible: images.current.isComparison
          ButtonGroup.group: comparisonModeGroup
        }
        RadioButton {
          id: sideBySideButton
          text: 'Side by side'
          visible: images.current.isComparison
          ButtonGroup.group: comparisonModeGroup
        }
      }
      ButtonGroup {
        id: comparisonModeGroup
        checkedButton: images.current.comparisonMode == ImageDocument.Difference ? differenceButton : sideBySideButton
        onClicked: {
          if (differenceButton.checked) images.current.comparisonMode = ImageDocument.Difference;
          else images.current.comparisonMode = ImageDocument.SideBySide;
        }
      }
    }

    ColumnLayout {
      Layout.fillWidth: true
      Text { text: '<b>Export</b>' }
      ExportButton {
        Layout.fillWidth: true
      }
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

    Text { text: 'Resolution:' }
    Text { text: images.current.size.width + ' x ' + images.current.size.height }
    Text { text: 'Cursor position:' }
    Text { text: images.current.pixelPosition.x + ', ' + images.current.pixelPosition.y }
    Text { text: 'Cursor texel value'; Layout.columnSpan: 2 }
    Repeater {
      model: images.current.channels
      Text {
        text: channelName(index, images.current.channels, channelFormat.checkedButton.formatType) + ': '
              + format(images.current.pixelValue, index, images.current.channels, images.current.isFloat, channelFormat.checkedButton.formatType);
        Layout.columnSpan: 2
        Layout.leftMargin: 10
      }
    }
    ButtonGroup {
      id: channelFormat
      buttons: formatButtons.children
    }
    RowLayout {
      id: formatButtons
      Layout.columnSpan: 2
      Layout.leftMargin: 10

      ToolButton {
        property string formatType: 'auto'
        text: 'Auto'
        checkable: true
        checked: true
      }
      ToolButton {
        property string formatType: 'float'
        text: 'Float'
        checkable: true
      }
      ToolButton {
        property string formatType: 'byte'
        text: 'Byte'
        checkable: true
      }
      ToolButton {
        property string formatType: 'normal'
        text: 'Normal'
        visible: images.current.channels >= 3
        checkable: true
      }
    }
  }
}
