// MainPage.qml

import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page
    property bool show_psk: tethering.settings_load_bool("show_psk")

    function onWifiPropertyChanged(name, value) {
        if (name === "Tethering" || name === "TetheringIdentifier"
                || name === "TetheringPassphrase") {
            update_status()
        }
    }
    function onBtPropertyChanged(name, value) {
        if (name === "Tethering") {
            update_status()
        }
    }
    function onChangingTetheringStatus(tech, status) {
        // Tethering not allowed in flightmode
        if (tech === "flightmode") {
            if (status === "enabled") {
                flightmode(true)
            }
            else {
                update_status()
            }
            return
        }

        // Menu entries disabled, when changing tethering status
        if (status.search("Error") !== -1) {
            mainMenu.enabled = true
        }
        else {
            mainMenu.enabled = false
        }

        if (tech === "wifi") {
            wifiStatus.color = Theme.secondaryHighlightColor
            wifiStatus.text = status
        }
        else if (tech === "bluetooth") {
            btStatus.color = Theme.secondaryHighlightColor
            btStatus.text = status
        }
    }

    function update_status() {
        var s = tethering.status()

        mainMenu.enabled = true
        wifiItems.visible = true
        btItems.visible = true
        flightmodeLabel.visible = false

        if (s["wifi"]) {
            wifi_tether_disable.visible = true
            wifi_tether_disable.enabled = true
            wifi_tether_enable.visible = false
            bt_tether_enable.enabled = false
            wifiStatus.text = "Enabled"
            wifiStatus.color = Theme.highlightColor
            wifiSSID.color = Theme.secondaryHighlightColor
            wifiPSK.color = Theme.secondaryHighlightColor
        }
        else {
            wifi_tether_disable.visible = false
            wifi_tether_disable.enabled = false
            wifi_tether_enable.visible = true
            bt_tether_enable.enabled = true
            wifiStatus.text = "Disabled"
            wifiStatus.color = Theme.secondaryColor
            wifiSSID.color = Theme.secondaryColor
            wifiPSK.color = Theme.secondaryColor
        }
        if (s["bluetooth"]) {
            bt_tether_disable.visible = true
            bt_tether_disable.enabled = true
            bt_tether_enable.visible = false
            wifi_tether_enable.enabled = false
            btStatus.text = "Enabled"
            btStatus.color = Theme.highlightColor
        }
        else {
            bt_tether_disable.visible = false
            bt_tether_disable.enabled = false
            bt_tether_enable.visible = true
            wifi_tether_enable.enabled = true
            btStatus.text = "Disabled"
            btStatus.color = Theme.secondaryColor
        }

        // Settings dialog disabled, when tethering is enabled
        if (s["wifi"] || s["bluetooth"]) {
            settingsMenu.enabled = false
        }
        else {
            settingsMenu.enabled = true
        }
        // Wifi tethering prohibited, when ssid or psk isn't set
        if (s["wifi_ssid"] === "" || s["wifi_psk"] === "") {
            wifi_tether_enable.enabled = false
            wifiNetworkLabel.visible = false
            wifiSSID.visible = false
            wifiPSKLabel.visible = false
            wifiPSK.visible = false
            wifiDefaultSettingsLabel.visible = true
        }
        else {
            wifiDefaultSettingsLabel.visible = false
            wifiNetworkLabel.visible = true
            wifiSSID.visible = true
            wifiSSID.text = s["wifi_ssid"]
            wifiPSKLabel.visible = show_psk
            wifiPSK.visible = show_psk
            wifiPSK.text = s["wifi_psk"]
        }

        if (s["flightmode"]) {
            flightmode(true)
        }
    }

    function flightmode(state) {
        bt_tether_disable.enabled = !state
        bt_tether_enable.enabled = !state
        wifi_tether_disable.enabled = !state
        wifi_tether_enable.enabled = !state
        wifiItems.visible = !state
        btItems.visible = !state
        flightmodeLabel.visible = state
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: parent.height
        contentWidth: parent.width

        PageHeader {
            id: pHeader
            title: "Tethering"
        }

        Label {
            id: flightmodeLabel
            visible: false
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin: Theme.paddingMedium
            anchors.rightMargin: Theme.paddingMedium
            width: parent.width
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: "Disable flight mode in order to use tethering."
        }

        Item {
            id: wifiItems
            anchors.top: pHeader.bottom
            anchors.topMargin: Theme.paddingMedium
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingMedium
            height: wifiColumn.height
            width: parent.width
            Column {
                id: wifiColumn
                width: parent.width
                Label {
                    id: wifiLabel
                    font.pixelSize: Theme.fontSizeLarge
                    text: "Wifi"
                }
                Label {
                    id: wifiStatusLabel
                    anchors.left: wifiLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    text: "Tethering status:"
                }
                Label {
                    id: wifiStatus
                    anchors.left: wifiStatusLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.right: parent.right
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: "Unknown"
                }
                Label {
                    id: wifiDefaultSettingsLabel
                    anchors.left: wifiLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    visible: false
                    wrapMode: Text.WordWrap
                    width: parent.width
                    horizontalAlignment: Text.AlignLeft
                    text: "Set network name and pre-shared key in settings."
                }

                Label {
                    id: wifiNetworkLabel
                    anchors.left: wifiLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    text: "Network name:"
                }
                Label {
                    id: wifiSSID
                    anchors.left: wifiNetworkLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.right: parent.right
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: ""
                }
                Label {
                    id: wifiPSKLabel
                    anchors.left: wifiLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    text: "Network pre-shared key:"
                }
                Label {
                    id: wifiPSK
                    anchors.left: wifiPSKLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.right: parent.right
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: ""
                }
            }
        }

        Item {
            id: btItems
            anchors.top: wifiItems.bottom
            anchors.topMargin: Theme.paddingLarge
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingMedium
            anchors.right: parent.right
            anchors.rightMargin: Theme.paddingMedium
            height: btColumn.height
            width: parent.width
            Column {
                id: btColumn
                width: parent.width
                Label {
                    id: btLabel
                    font.pixelSize: Theme.fontSizeLarge
                    text: "Bluetooth"
                }
                Label {
                    id: btStatusLabel
                    anchors.left: btLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    text: "Tethering status:"
                }
                Label {
                    id: btStatus
                    anchors.left: btStatusLabel.left
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.right: parent.right
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: "Unknown"
                }
            }
        }
        PullDownMenu {
            id: mainMenu
            MenuItem {
                id: settingsMenu
                text: "Settings"
                onClicked: {
                    var dialog = pageStack.push("SettingsDialog.qml",
                                                {wifi_ssid: wifiSSID.text,
                                                wifi_psk: wifiPSK.text,
                                                show_psk: show_psk})
                    dialog.accepted.connect(function() {
                        var attrs = {"wifi_ssid": dialog.wifi_ssid,
                                    "wifi_psk": dialog.wifi_psk}
                        show_psk = dialog.show_psk
                        wifiPSKLabel.visible = show_psk
                        wifiPSK.visible = show_psk
                        tethering.send_settings(attrs)
                        tethering.settings_save("show_psk", dialog.show_psk)
                    })
                }
            }
            MenuItem {
                id: bt_tether_enable
                enabled: false
                visible: false
                text: "Enable Bluetooth Tethering"
                onClicked: {
                    mainMenu.close(false)
                    tethering.enable("bluetooth")
                }
            }
            MenuItem {
                id: bt_tether_disable
                enabled: false
                visible: false
                text: "Disable Bluetooth Tethering"
                onClicked: {
                    mainMenu.close(false)
                    tethering.disable("bluetooth")
                }
            }
            MenuItem {
                id: wifi_tether_enable
                enabled: false
                visible: false
                text: "Enable Wifi Tethering"
                onClicked: {
                    mainMenu.close(false)
                    tethering.enable("wifi")
                }
            }
            MenuItem {
                id: wifi_tether_disable
                enabled: false
                visible: false
                text: "Disable Wifi Tethering"
                onClicked: {
                    mainMenu.close(false)
                    tethering.disable("wifi")
                }
            }
        }
    }

    Component.onCompleted: {
        tethering.wifi_property_changed_signal.connect(onWifiPropertyChanged)
        tethering.bt_property_changed_signal.connect(onBtPropertyChanged)
        tethering.changing_tethering_status.connect(onChangingTetheringStatus)
        update_status()
    }
}


