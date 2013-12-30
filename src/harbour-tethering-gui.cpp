#include <sailfishapp.h>
#include <QDebug>
#include <QtQuick>
#include "tetheringqdbus.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QScopedPointer<TetheringQdbus> tethering(new TetheringQdbus());
    view->rootContext()->setContextProperty("tethering", tethering.data());

    view->setSource(SailfishApp::pathTo("qml/harbour-tethering-gui.qml"));

    view->showFullScreen();

    return app->exec();
}
