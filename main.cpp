#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFont>
#include <QIcon>

#include "GetSettings.h"
#include "Firebase.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<GetSettings>("GetSettings", 1, 0, "GetSettings");
    qmlRegisterType<Firebase>("Firebase", 1, 0, "Firebase");

    app.setWindowIcon(QIcon("qrc:/icons/cellphone-arrow-down"));

    app.setOrganizationName("Pixyl");
    app.setOrganizationDomain("gopixyl.com");
    app.setApplicationName("PixylBooth");

    QFont font("Helvetica");
    font.setStyleHint(QFont::OldEnglish);
    font.setWeight(QFont::Light);
    app.setFont(font);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
