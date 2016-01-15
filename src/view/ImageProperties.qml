import QtQuick 2.3
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
								
		Slider {						
			id: 'scaleSlider'
			Layout.fillWidth: true
			minimumValue: 0
			maximumValue: images.current.maxScaleIndex
			stepSize: 1
			value: images.current.scaleIndex
			onValueChanged: images.current.scaleIndex = scaleSlider.value;
		}
					
		Text { text: images.current.scale + 'x'; Layout.minimumWidth: 30 }

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
		}

		Text { text: gammaSlider.value.toFixed(1) }
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
	}
}