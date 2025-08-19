#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "CAN_Communication/CanHandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

     // 1. Tạo đối tượng CanHandler và mở CAN
    CanHandler canHandler("can0");

    if (!canHandler.isOpen()) {
        qWarning("Không mở được CAN interface!");
    }

    // 2. Gán vào QML context để QML gọi được
    engine.rootContext()->setContextProperty("canHandler", &canHandler);
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
