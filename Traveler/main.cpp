#include <QApplication>

#include "mainwindow.h"

#define NAME "Traveler"
#define VERSION "1.0"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QString appName(NAME);
    appName = appName + " [" + VERSION + "]";
    QCoreApplication::setApplicationName(appName);
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());

    MainWindow window;
    window.show();

    return a.exec();
}
