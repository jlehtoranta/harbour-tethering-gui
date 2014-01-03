#include "tetheringqdbus.h"
#include <QDebug>
#include <unistd.h>

// Initialization
TetheringQdbus::TetheringQdbus()
{
    dif_cellular_service = NULL;
    dif_wifi_service = NULL;
    changing_state = false;

    settings = new QSettings("harbour-tethering-gui", "harbour.tethering.gui");
    dcon = new QDBusConnection(QDBusConnection::connectToBus(QDBusConnection::SystemBus, "connman"));
    dif_manager = new QDBusInterface("net.connman",
                                     "/",
                                     "net.connman.Manager",
                                     *dcon,
                                     this);
    dif_session = new QDBusInterface("net.connman",
                                     "/",
                                     "net.connman.Session",
                                     *dcon,
                                     this);
    get_technology_ifs();
    get_service_ifs();
    save_connection_state();

    connect(dif_wifi, SIGNAL(PropertyChanged(QString, QDBusVariant)),
            this, SLOT(wifi_property_changed(QString, QDBusVariant)));
    connect(dif_bt, SIGNAL(PropertyChanged(QString, QDBusVariant)),
            this, SLOT(bt_property_changed(QString, QDBusVariant)));
    connect(dif_manager, SIGNAL(PropertyChanged(QString, QDBusVariant)),
            this, SLOT(connection_state_changed(QString, QDBusVariant)));
}

TetheringQdbus::~TetheringQdbus()
{
    // Disable tethering on exit
    disable("wifi");
    disable("bluetooth");
    // Restore connection state
    restore_connection_state();

    // Disconnect
    disconnect(dif_wifi, SIGNAL(PropertyChanged(QString, QDBusVariant)),
               this, SLOT(wifi_property_changed(QString, QDBusVariant)));
    disconnect(dif_bt, SIGNAL(PropertyChanged(QString, QDBusVariant)),
               this, SLOT(bt_property_changed(QString, QDBusVariant)));
    disconnect(dif_manager, SIGNAL(PropertyChanged(QString, QDBusVariant)),
               this, SLOT(connection_state_changed(QString, QDBusVariant)));
    // Free reserved memory
    delete dif_manager;
    delete dif_session;
    delete dif_wifi;
    delete dif_bt;
    delete dif_cellular;
    delete dif_cellular_service;
    delete dif_wifi_service;
    delete dcon;
    delete settings;

}

void TetheringQdbus::get_technology_ifs()
{
    QDBusMessage rmsg = dif_manager->call("GetTechnologies");
    QVariant var = rmsg.arguments().at(0);
    QDBusArgument arg = var.value<QDBusArgument>();
    QMap<QDBusObjectPath, QVariantMap> reply;
    arg >> reply;
    QMap<QDBusObjectPath, QVariantMap>::iterator i;
    for (i = reply.begin(); i != reply.end(); ++i) {
        QDBusObjectPath dpath = i.key();
        QString tech = i.value().value("Type").toString();
        if (QString::compare(tech, "wifi") == 0) {
            dif_wifi = new QDBusInterface("net.connman",
                                          dpath.path(),
                                          "net.connman.Technology",
                                          *dcon,
                                          this);
        }
        else if (QString::compare(tech, "bluetooth") == 0) {
            dif_bt = new QDBusInterface("net.connman",
                                        dpath.path(),
                                        "net.connman.Technology",
                                        *dcon,
                                        this);
        }
        else if (QString::compare(tech, "cellular") == 0) {
            dif_cellular = new QDBusInterface("net.connman",
                                            dpath.path(),
                                            "net.connman.Technology",
                                            *dcon,
                                            this);
        }
    }
}

void TetheringQdbus::get_service_ifs()
{
    if (dif_wifi_service) {
        delete dif_wifi_service;
        dif_wifi_service = NULL;
    }
    if (dif_cellular_service) {
        delete dif_cellular_service;
        dif_cellular_service = NULL;
    }
    wifi_cstate = false;
    QDBusMessage rmsg = dif_manager->call("GetServices");
    QVariant var = rmsg.arguments().at(0);
    QDBusArgument arg = var.value<QDBusArgument>();
    QMap<QDBusObjectPath, QVariantMap> reply;
    arg >> reply;
    QMap<QDBusObjectPath, QVariantMap>::iterator i;
    for (i = reply.begin(); i != reply.end(); ++i) {
        QDBusObjectPath dpath = i.key();
        QString tech = i.value().value("Type").toString();
        QString name = i.value().value("Name").toString();
        QString state = i.value().value("State").toString();
        bool autoc = i.value().value("AutoConnect").toBool();
        if (QString::compare(tech, "wifi") == 0 &&
                (QString::compare(state, "online") == 0 ||
                 QString::compare(state, "ready") == 0)) {
            dif_wifi_service = new QDBusInterface("net.connman",
                                                dpath.path(),
                                                "net.connman.Service",
                                                *dcon,
                                                this);
            wifi_cstate = true;
        }
        else if (QString::compare(tech, "cellular") == 0) {
            dif_cellular_service = new QDBusInterface("net.connman",
                                                    dpath.path(),
                                                    "net.connman.Service",
                                                    *dcon,
                                                    this);
            cel_name = name;
            if (QString::compare(state, "online") == 0 ||
                    QString::compare(state, "ready") == 0) {
                cel_cstate = true;
            }
            else {
                cel_cstate = false;
            }
            cel_astate_restore = autoc;
        }
    }
}

// States
void TetheringQdbus::save_connection_state()
{
    QDBusMessage rmsg = dif_manager->call("GetTechnologies");
    QVariant var = rmsg.arguments().at(0);
    QDBusArgument arg = var.value<QDBusArgument>();
    QMap<QDBusObjectPath, QVariantMap> reply;
    arg >> reply;
    QMap<QDBusObjectPath, QVariantMap>::iterator i;
    for (i = reply.begin(); i != reply.end(); ++i) {
        QString tech = i.value().value("Type").toString();
        if (QString::compare(tech, "wifi") == 0) {
            wifi_pstate = i.value().value("Powered").toBool();
            wifi_pstate_restore = wifi_pstate;
            wifi_tstate = i.value().value("Tethering").toBool();
        }
        else if (QString::compare(tech, "bluetooth") == 0) {
            bt_pstate = i.value().value("Powered").toBool();
            bt_tstate = i.value().value("Tethering").toBool();
        }
        else if (QString::compare(tech, "cellular") == 0) {
            cel_cstate_restore = i.value().value("Connected").toBool();
        }
    }
}

void TetheringQdbus::restore_connection_state()
{
    if (!cel_cstate_restore && !cel_astate_restore) {
        dif_cellular_service->call(QDBus::NoBlock, "Disconnect");
    }
    if (cel_astate_restore) {
        dif_cellular_service->call(QDBus::NoBlock, "SetProperty", QString("AutoConnect"),
                                   QVariant::fromValue(QDBusVariant(true)));
    }
    else {
        dif_cellular_service->call(QDBus::NoBlock, "SetProperty", QString("AutoConnect"),
                                   QVariant::fromValue(QDBusVariant(false)));
    }
}

void TetheringQdbus::power_on(const QString &tech)
{
    QDBusMessage reply;

    if (QString::compare(tech, "wifi") == 0) {
        if (wifi_pstate == false) {
            // Power on
            emit changing_tethering_status(tech, "Switching " + tech + " on...");
            reply = dif_wifi->call(QDBus::Block, "SetProperty", QString("Powered"),
                           QVariant::fromValue(QDBusVariant(true)));
        }
    }
    else if (QString::compare(tech, "bluetooth") == 0) {
        if (bt_pstate == false) {
            // Power on
            emit changing_tethering_status(tech, "Switching " + tech + " on...");
            reply = dif_bt->call(QDBus::Block, "SetProperty", QString("Powered"),
                         QVariant::fromValue(QDBusVariant(true)));
        }
    }

    if (reply.type() != QDBusMessage::InvalidMessage) {
        if  (QString::compare(reply.errorName(), "") == 0) {
            return;
        }
        else if (QString::compare(reply.errorName(), "net.connman.Error.AlreadyEnabled") == 0) {
            enable_tethering(tech);
        }
        else {
            changing_state = false;
            emit changing_tethering_status(tech, "Error: " + reply.errorMessage());
        }
    }
    else {
        enable_tethering(tech);
    }

}

void TetheringQdbus::enable_tethering(const QString &tech)
{
    QDBusMessage reply;

    emit changing_tethering_status(tech, "Enabling tethering...");
    if (QString::compare(tech, "wifi") == 0) {
        // Enable tethering
        // Try 5 times, tethering might not be available straight after powering
        for (int i = 0; i < 5; ++i) {
            reply = dif_wifi->call(QDBus::Block, "SetProperty", QString("Tethering"),
                                    QVariant::fromValue(QDBusVariant(true)));
            if (QString::compare(reply.errorName(), "net.connman.Error.NotSupported") == 0) {
                usleep(50*1000);
                continue;
            }
            break;
        }
    }
    else if (QString::compare(tech, "bluetooth") == 0) {
        // Enable tethering
        // Try 5 times, tethering might not be available straight after powering
        for (int i = 0; i < 5; ++i) {
            reply = dif_bt->call(QDBus::Block, "SetProperty", QString("Tethering"),
                                 QVariant::fromValue(QDBusVariant(true)));
            if (QString::compare(reply.errorName(), "net.connman.Error.NotSupported") == 0) {
                usleep(50*1000);
                continue;
            }
            break;
        }
    }

    if  (QString::compare(reply.errorName(), "") == 0 ||
         QString::compare(reply.errorName(), "net.connman.Error.AlreadyEnabled") == 0) {
        changing_state = false;
        if (QString::compare(tech, "wifi") == 0) {
            emit wifi_property_changed_signal("Tethering", true);
        }
        else if (QString::compare(tech, "bluetooth") == 0) {
            emit bt_property_changed_signal("Tethering", true);
        }
        return;
    }
    else {
        changing_state = false;
        emit changing_tethering_status(tech, "Error: " + reply.errorMessage());
        return;
    }
}

// Slots
void TetheringQdbus::wifi_property_changed(QString name, QDBusVariant dval)
{
    QVariant value = dval.variant();

    if (QString::compare(name, "Powered") == 0) {
        wifi_pstate = value.toBool();
        // Enable tethering after the adapter has been powered
        if (changing_state && value.toBool()) {
            enable_tethering("wifi");
        }
        // Save powered state, when tethering is disabled
        else if (!wifi_tstate && !changing_state) {
            wifi_pstate_restore = wifi_pstate;
        }
    }
    if (QString::compare(name, "Tethering") == 0) {
        wifi_tstate = value.toBool();
        // Restore previous connection state, if tethering is disabled
        if (!bt_tstate && !wifi_tstate) {
            restore_connection_state();
        }
    }
    emit wifi_property_changed_signal(name, value);
}

void TetheringQdbus::bt_property_changed(QString name, QDBusVariant dval)
{
    QVariant value = dval.variant();

    if (QString::compare(name, "Powered") == 0) {
        // Enable tethering after the adapter has been powered
        if (changing_state && value.toBool()) {
            enable_tethering("bluetooth");
        }
        // Save powered state, when tethering is disabled
        else if (!bt_tstate) {
            bt_pstate = value.toBool();
        }
    }
    if (QString::compare(name, "Tethering") == 0) {
        bt_tstate = value.toBool();
        // Restore previous connection state, if tethering is disabled
        if (!bt_tstate && !wifi_tstate) {
            restore_connection_state();
        }
    }
    emit bt_property_changed_signal(name, value);
}

void TetheringQdbus::connection_state_changed(QString name, QDBusVariant dval)
{
    QVariant value = dval.variant();
    if (QString::compare(name, "State") == 0 && !changing_state) {
        // Try to reconnect, if tethering is enabled
        if (!(QString::compare(value.toString(), "online") == 0
             || QString::compare(value.toString(), "ready") == 0)
                && (wifi_tstate || bt_tstate)) {
            dif_session->call(QDBus::NoBlock, "Connect");
        }
    }
    if (QString::compare(name, "OfflineMode") == 0) {
        if (value.toBool()) {
            emit changing_tethering_status("flightmode", "enabled");
        }
        else {
            emit changing_tethering_status("flightmode", "disabled");
        }
    }
}

// D-BUS callback SLOTS
void TetheringQdbus::callback_wifi_error(QDBusError err)
{
    changing_state = false;
    emit changing_tethering_status("wifi", "Error: " + err.message());
}
void TetheringQdbus::callback_bt_error(QDBusError err)
{
    changing_state = false;
    emit changing_tethering_status("bluetooth", "Error: " + err.message());
}
void TetheringQdbus::callback_bt_cellular_connection(QDBusMessage msg)
{
    qDebug(qPrintable(msg.service()));
    power_on("bluetooth");
}
void TetheringQdbus::callback_wifi_cellular_connection(QDBusMessage msg)
{
    qDebug(qPrintable(msg.service()));
    power_on("wifi");
}
void TetheringQdbus::callback_wifi_connection(QDBusMessage msg)
{
    qDebug(qPrintable(msg.service()));
    dif_wifi->call(QDBus::Block, "SetProperty", QString("Powered"),
                   QVariant::fromValue(QDBusVariant(false)));
    wifi_pstate = false;
    if (cel_cstate) {
        power_on("wifi");
    }
    else {
        emit changing_tethering_status("wifi", "Connecting to " + cel_name + "...");
        QList<QVariant> args;
        dif_cellular_service->callWithCallback("Connect", args, this,
                                               SLOT(callback_wifi_cellular_connection(QDBusMessage)),
                                               SLOT(callback_wifi_error(QDBusError)));
    }
}
void TetheringQdbus::callback_bt_wifi_connection(QDBusMessage msg)
{
    qDebug(qPrintable(msg.service()));
    dif_wifi->call(QDBus::Block, "SetProperty", QString("Powered"),
                   QVariant::fromValue(QDBusVariant(false)));
    wifi_pstate = false;
    if (cel_cstate) {
        power_on("bluetooth");
    }
    else {
        emit changing_tethering_status("bluetooth", "Connecting to " + cel_name + "...");
        QList<QVariant> args;
        dif_cellular_service->callWithCallback("Connect", args, this,
                                               SLOT(callback_bt_cellular_connection(QDBusMessage)),
                                               SLOT(callback_bt_error(QDBusError)));
    }
}



// User actions
void TetheringQdbus::enable(const QString &tech)
{
    QList<QVariant> args;

    changing_state = true;
    if (!wifi_tstate && !bt_tstate) {
        save_connection_state();
    }
    get_service_ifs();

    if (QString::compare(tech, "wifi") == 0) {
        if (wifi_cstate && dif_wifi_service) {
            // Disconnect and power off wifi
            emit changing_tethering_status(tech, "Powering off Wifi...");
            dif_wifi_service->callWithCallback("Disconnect", args, this,
                                               SLOT(callback_wifi_connection(QDBusMessage)),
                                               SLOT(callback_wifi_error(QDBusError)));
        }
        else if (cel_cstate) {
            power_on(tech);
        }
        else {
            emit changing_tethering_status(tech, "Connecting to " + cel_name + "...");
            dif_cellular_service->callWithCallback("Connect", args, this,
                                                   SLOT(callback_wifi_cellular_connection(QDBusMessage)),
                                                   SLOT(callback_wifi_error(QDBusError)));
        }
    }
    else if (QString::compare(tech, "bluetooth") == 0) {
        if (wifi_cstate && dif_wifi_service) {
            // Disconnect and power off wifi
            emit changing_tethering_status(tech, "Powering off Wifi...");
            dif_wifi_service->callWithCallback("Disconnect", args, this,
                                               SLOT(callback_bt_wifi_connection(QDBusMessage)),
                                               SLOT(callback_bt_error(QDBusError)));
        }
        else if (cel_cstate) {
            power_on(tech);
        }
        else {
            emit changing_tethering_status(tech, "Connecting to " + cel_name + "...");
            dif_cellular_service->callWithCallback("Connect", args, this,
                                                   SLOT(callback_bt_cellular_connection(QDBusMessage)),
                                                   SLOT(callback_bt_error(QDBusError)));
        }
    }
}

void TetheringQdbus::disable(const QString &tech)
{
    QDBusMessage reply;

    emit changing_tethering_status(tech, "Disabling...");

    if (QString::compare(tech, "wifi") == 0) {
        reply = dif_wifi->call(QDBus::Block, "SetProperty", QString("Tethering"),
                               QVariant::fromValue(QDBusVariant(false)));
    }
    else if (QString::compare(tech, "bluetooth") == 0) {
        reply = dif_bt->call(QDBus::Block, "SetProperty", QString("Tethering"),
                             QVariant::fromValue(QDBusVariant(false)));
    }

    if (QString::compare(reply.errorName(), "net.connman.Error.AlreadyDisabled") == 0 ||
            QString::compare(reply.errorName(), "") == 0) {
        // Give some time before restoring powered state
        usleep(500*1000);
        if (wifi_pstate != wifi_pstate_restore) {
            dif_wifi->call(QDBus::Block, "SetProperty", QString("Powered"),
                           QVariant::fromValue(QDBusVariant(wifi_pstate_restore)));
        }
        if (QString::compare(tech, "bluetooth") == 0 && !bt_pstate) {
            dif_bt->call(QDBus::Block, "SetProperty", QString("Powered"),
                         QVariant::fromValue(QDBusVariant(bt_pstate)));
        }
    }
    else {
        emit changing_tethering_status(tech, reply.errorMessage());
    }
}

QVariantMap TetheringQdbus::status()
{
    QVariantMap s;

    QDBusMessage rmsg = dif_manager->call("GetTechnologies");
    QVariant var = rmsg.arguments().at(0);
    QDBusArgument arg = var.value<QDBusArgument>();
    QMap<QDBusObjectPath, QVariantMap> reply;
    arg >> reply;
    QMap<QDBusObjectPath, QVariantMap>::iterator i;
    for (i = reply.begin(); i != reply.end(); ++i) {
        QString tech = i.value().value("Type").toString();
        if (QString::compare(tech, "wifi") == 0) {
            s["wifi"] = i.value().value("Tethering").toBool();
            if (i.value().contains("TetheringIdentifier")) {
                s["wifi_ssid"] = i.value().value("TetheringIdentifier").toString();
            }
            else {
                s["wifi_ssid"] = QString("");
            }
            if (i.value().contains("TetheringPassphrase")) {
                s["wifi_psk"] = i.value().value("TetheringPassphrase").toString();
            }
            else {
                s["wifi_psk"] = QString("");
            }
        }
        else if (QString::compare(tech, "bluetooth") == 0) {
            s["bluetooth"] = i.value().value("Tethering").toBool();
        }
    }

    rmsg = dif_manager->call("GetProperties");
    var = rmsg.arguments().at(0);
    arg = var.value<QDBusArgument>();
    QVariantMap reply2;
    arg >> reply2;
    s["flightmode"] = reply2.value("OfflineMode").toBool();

    return s;
}

bool TetheringQdbus::send_settings(const QVariantMap &values)
{
    QDBusMessage reply;

    reply = dif_wifi->call("SetProperty", "TetheringIdentifier",
                           QVariant::fromValue(QDBusVariant(values.value("wifi_ssid").toString())));
    reply = dif_wifi->call("SetProperty", "TetheringPassphrase",
                           QVariant::fromValue(QDBusVariant(values.value("wifi_psk").toString())));

    return true;
}

bool TetheringQdbus::settings_load_bool(const QString &name)
{
    return settings->value(name, true).toBool();
}

void TetheringQdbus::settings_save(const QString &name, const QVariant &value)
{
    settings->setValue(name, value);
}
