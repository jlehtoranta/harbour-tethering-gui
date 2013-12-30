#ifndef TETHERINGQDBUS_H
#define TETHERINGQDBUS_H

#include <QObject>
#include <QtDBus>
#include <QSettings>

class TetheringQdbus : public QObject
{
    Q_OBJECT
    QDBusConnection *dcon;
    QDBusInterface *dif_manager;
    QDBusInterface *dif_session;
    QDBusInterface *dif_wifi;
    QDBusInterface *dif_bt;
    QDBusInterface *dif_cellular;
    QDBusInterface *dif_cellular_service;
    QDBusInterface *dif_wifi_service;
    bool wifi_pstate;
    bool wifi_cstate;
    bool wifi_tstate;
    bool bt_pstate;
    bool bt_tstate;
    bool cel_cstate;
    bool cel_cstate_restore;
    bool cel_astate_restore;
    bool changing_state;
    QString cel_name;
    QSettings *settings;

public:
    explicit TetheringQdbus();
    ~TetheringQdbus();

signals:
    void wifi_property_changed_signal(QString name, QVariant value);
    void bt_property_changed_signal(QString name, QVariant value);
    void changing_tethering_status(QString tech, QString status);

public slots:
    void enable(const QString &tech);
    void disable(const QString &tech);
    QVariantMap status();
    bool send_settings(const QVariantMap &values);
    void wifi_property_changed(QString name, QDBusVariant dval);
    void bt_property_changed(QString name, QDBusVariant dval);
    void connection_state_changed(QString name, QDBusVariant dval);
    bool settings_load_bool(const QString &name);
    void settings_save(const QString &name, const QVariant &value);

private slots:
    void callback_wifi_error(QDBusError err);
    void callback_bt_error(QDBusError err);
    void callback_bt_cellular_connection(QDBusMessage msg);
    void callback_wifi_cellular_connection(QDBusMessage msg);
    void callback_wifi_connection(QDBusMessage msg);
    void callback_bt_wifi_connection(QDBusMessage msg);

private:
    void get_technology_ifs();
    void get_service_ifs();
    void save_connection_state();
    void restore_connection_state();
    void power_on(const QString &tech);
    void enable_tethering(const QString &tech);

};

#endif // TETHERINGQDBUS_H
