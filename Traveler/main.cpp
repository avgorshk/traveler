#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Traveler");
    QGuiApplication::setApplicationDisplayName(QCoreApplication::applicationName());

    MainWindow window;
    window.show();

    return a.exec();
}
