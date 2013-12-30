// SettingsDialog.qml

import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    property alias wifi_ssid: ssidField.text
    property alias wifi_psk: pskField.text
    property alias show_psk: pskShow.checked

    id: dialog
    canAccept: false

    Column {
        id: column

        width: dialog.width
        anchors.fill: parent
        anchors.rightMargin: Theme.paddingMedium
        spacing: Theme.paddingMedium

        DialogHeader {
            title: "Tethering Settings"
            acceptText: "Save"
        }
        Label {
            id: wifiname
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            text: "Wifi network name:"
        }
        TextField {
            id: ssidField
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingLarge
            anchors.bottomMargin: Theme.paddingLarge
            width: parent.width
            placeholderText: "SSID (1-32 characters)"
            validator: RegExpValidator {regExp: /.{1,32}/}
            onTextChanged: {
                set_accepted("ssidField")
            }
        }

        Label {
            id: pskname
            anchors.topMargin: Theme.PaddingLarge
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            text: "Wifi network pre-shared key:"
        }
        TextField {
            id: pskField
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingLarge
            width: parent.width
            placeholderText: "PSK (8-63 characters)"
            validator: RegExpValidator {regExp: /.{8,63}/}
            onTextChanged: {
                set_accepted("pskField")
            }
        }
        TextSwitch {
            id: pskShow
            text: "Show pre-shared key in main window"
        }
    }

    function set_accepted(field) {
        if (ssidField.acceptableInput && pskField.acceptableInput) {
            canAccept = true
        }
        else {
            canAccept = false
        }
    }

    onDone: {
        if (result === DialogResult.Accepted) {
            wifi_ssid = ssidField.text
            wifi_psk = pskField.text
            show_psk = pskShow.checked
        }
    }

}
