#include "mainwindow.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QStyle>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow{parent}, m_view(new MapView) {
    QLabel* regionNameLabel = new QLabel("Region:");
    Q_ASSERT(regionNameLabel != nullptr);
    regionNameLabel->setMinimumWidth(50);
    QLineEdit* regionNameEdit = new QLineEdit();
    Q_ASSERT(regionNameEdit != nullptr);

    QHBoxLayout* regionNameLayout = new QHBoxLayout();
    Q_ASSERT(regionNameLayout != nullptr);
    regionNameLayout->addWidget(regionNameLabel);
    regionNameLayout->addWidget(regionNameEdit);

    QLabel* regionVisitedLabel = new QLabel("Visited:");
    Q_ASSERT(regionVisitedLabel != nullptr);
    regionVisitedLabel->setMinimumWidth(50);
    QCheckBox* regionVisitedCheckBox = new QCheckBox();
    Q_ASSERT(regionVisitedCheckBox != nullptr);

    QHBoxLayout* regionVisitedLayout = new QHBoxLayout();
    Q_ASSERT(regionVisitedLayout != nullptr);
    regionVisitedLayout->setAlignment(Qt::AlignLeft);
    regionVisitedLayout->addWidget(regionVisitedLabel);
    regionVisitedLayout->addWidget(regionVisitedCheckBox);

    QVBoxLayout* propsLayout = new QVBoxLayout();
    Q_ASSERT(propsLayout != nullptr);
    propsLayout->setAlignment(Qt::AlignTop);
    propsLayout->addLayout(regionNameLayout);
    propsLayout->addLayout(regionVisitedLayout);

    QGroupBox* propsBox = new QGroupBox("Properties");
    Q_ASSERT(propsBox != nullptr);
    propsBox->setMinimumWidth(300);
    propsBox->setLayout(propsLayout);

    QGroupBox* statsBox = new QGroupBox("Statistics");
    Q_ASSERT(statsBox != nullptr);

    QVBoxLayout* panelLayout = new QVBoxLayout();
    Q_ASSERT(panelLayout != nullptr);
    panelLayout->addWidget(propsBox);
    panelLayout->addWidget(statsBox);

    QWidget* panel = new QWidget();
    Q_ASSERT(panel != nullptr);
    panel->setLayout(panelLayout);

    QHBoxLayout* windowLayout = new QHBoxLayout();
    Q_ASSERT(windowLayout != nullptr);
    windowLayout->addWidget(m_view);
    windowLayout->addWidget(panel);

    QWidget* window = new QWidget();
    Q_ASSERT(window != nullptr);
    window->setLayout(windowLayout);

    setCentralWidget(window);

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(1400, 800),
            screen()->availableGeometry()));
}
