#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow{parent}, m_view(new MapView) {
    setCentralWidget(m_view);
}
