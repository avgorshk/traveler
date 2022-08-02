#include "mainwindow.h"

#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow{parent}, m_view(new MapView) {
    setCentralWidget(m_view);

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(1200, 800),
            screen()->availableGeometry()));
}
