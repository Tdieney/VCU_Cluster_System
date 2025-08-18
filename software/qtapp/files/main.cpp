#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "communication/canhandler.h"
#include <QQmlContext>
#include <QDir>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    // qDebug() << "Current working dir:" << QDir::currentPath();
    CanHandler canHandler; // Create an instance of CanHandler

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [](){
        return 0; // Ensure the application exits cleanly
    });

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("canHandler", &canHandler); // Expose CanHandler to QML

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
