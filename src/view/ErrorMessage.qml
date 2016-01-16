import QtQuick 2.3
import Qt.labs.folderlistmodel 1.0
import Qt.labs.settings 1.0
import QtQuick.Dialogs 1.2

MessageDialog {
  property bool show: true

  id: errorMessageDialog
  title: 'Error'
  icon: StandardIcon.Critical
  standardButtons: StandardButton.Ok
  text: 'Could not load image file.'
  informativeText: images.errorMessage
  Component.onCompleted: visible = show
  onAccepted: if (show) Qt.quit()
}
