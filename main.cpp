#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "src/models/schoolmodel.h"
#include "src/models/timetablemodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    SchoolModel *schoolModel = new SchoolModel(&engine);
    engine.rootContext()->setContextProperty("schoolModel", schoolModel);

    qmlRegisterType<TimetableModel>("App", 1, 0, "TimetableModel");

    engine.loadFromModule("timesheet", "Main");
    if (engine.rootObjects().isEmpty())
    {
        qDebug() << "Failed to load QML!";
        return -1;
    }

    return app.exec();
}
