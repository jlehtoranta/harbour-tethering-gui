// CoverPage.qml

import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    function onWifiPropertyChanged(name, value) {
        if (name === "Tethering") {
            if (value) {
                wifitether.text = "Enabled"
                wifitether.color = Theme.highlightColor
            }
            else {
                wifitether.text = "Disabled"
                wifitether.color = Theme.secondaryColor
            }
        }
    }
    function onBtPropertyChanged(name, value) {
        if (name === "Tethering") {
            if (value) {
                bttether.text = "Enabled"
                bttether.color = Theme.highlightColor
            }
            else {
                bttether.text = "Disabled"
                bttether.color = Theme.secondaryColor
            }
        }
    }
    function onChangingTetheringStatus(tech, status) {
        if (tech === "wifi") {
            wifitether.color = Theme.secondaryHighlightColor
            wifitether.text = status
        }
        else if (tech === "bluetooth") {
            bttether.color = Theme.secondaryHighlightColor
            bttether.text = status
        }
    }

    function update_status() {
        var s = tethering.status()

        if (s["wifi"]) {
            wifitether.text = "Enabled"
            wifitether.color = Theme.highlightColor
        }
        else {
            wifitether.text = "Disabled"
            wifitether.color = Theme.secondaryColor
        }
        if (s["bluetooth"]) {
            bttether.text = "Enabled"
            bttether.color = Theme.highlightColor
        }
        else {
            bttether.text = "Disabled"
            bttether.color = Theme.secondaryColor
        }
    }

    Label {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingLarge
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge
        verticalAlignment: Text.AlignTop
        font.family: Theme.fontFamilyHeading
        font.pixelSize: Theme.fontSizeLarge
        text: "Tethering"
    }

    Label {
        id: wifitethername
        anchors.left: label.left
        anchors.leftMargin: Theme.paddingMedium
        anchors.top: label.bottom
        anchors.topMargin: Theme.paddingLarge
        verticalAlignment: Text.AlignTop
        font.pixelSize: Theme.fontSizeMedium
        text: "Wifi"
    }
    Label {
        id: wifitether
        anchors.left: wifitethername.left
        anchors.leftMargin: Theme.paddingLarge
        anchors.top: wifitethername.bottom
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium
        verticalAlignment: Text.AlignTop
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryColor
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        maximumLineCount: 2
        text: "Unknown"
    }
    Label {
        id: bttethername
        anchors.left: label.left
        anchors.leftMargin: Theme.paddingMedium
        anchors.top: wifitether.bottom
        anchors.topMargin: Theme.paddingMedium
        verticalAlignment: Text.AlignTop
        font.pixelSize: Theme.fontSizeMedium
        text: "Bluetooth"
    }
    Label {
        id: bttether
        anchors.left: bttethername.left
        anchors.leftMargin: Theme.paddingLarge
        anchors.top: bttethername.bottom
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingMedium
        verticalAlignment: Text.AlignTop
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryColor
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        maximumLineCount: 2
        text: "Unknown"
    }

    Component.onCompleted: {
        tethering.wifi_property_changed_signal.connect(onWifiPropertyChanged)
        tethering.bt_property_changed_signal.connect(onBtPropertyChanged)
        tethering.changing_tethering_status.connect(onChangingTetheringStatus)
        update_status()
    }
}

