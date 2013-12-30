# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = harbour-tethering-gui

CONFIG += sailfishapp

SOURCES += \
    src/tetheringqdbus.cpp \
    src/harbour-tethering-gui.cpp

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    qml/pages/SettingsDialog.qml \
    qml/pages/MainPage.qml \
    LICENSE \
    qml/harbour-tethering-gui.qml \
    rpm/harbour-tethering-gui.spec \
    rpm/harbour-tethering-gui.yaml \
    harbour-tethering-gui.desktop

QT += dbus

HEADERS += \
    src/tetheringqdbus.h

