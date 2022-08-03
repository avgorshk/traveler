#include "mainwindow.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow{parent}, m_view(new MapView), m_currentRegion(nullptr) {
    // Make layout

    QLabel* regionNameLabel = new QLabel("Region/Point:");
    Q_ASSERT(regionNameLabel != nullptr);
    regionNameLabel->setMinimumWidth(100);
    QLineEdit* regionNameEdit = new QLineEdit();
    Q_ASSERT(regionNameEdit != nullptr);
    regionNameEdit->setEnabled(false);

    QHBoxLayout* regionNameLayout = new QHBoxLayout();
    Q_ASSERT(regionNameLayout != nullptr);
    regionNameLayout->addWidget(regionNameLabel);
    regionNameLayout->addWidget(regionNameEdit);

    QLabel* regionVisitedLabel = new QLabel("Visited:");
    Q_ASSERT(regionVisitedLabel != nullptr);
    regionVisitedLabel->setMinimumWidth(100);
    QCheckBox* regionVisitedCheckBox = new QCheckBox();
    Q_ASSERT(regionVisitedCheckBox != nullptr);
    regionVisitedCheckBox->setEnabled(false);

    QHBoxLayout* regionVisitedLayout = new QHBoxLayout();
    Q_ASSERT(regionVisitedLayout != nullptr);
    regionVisitedLayout->setAlignment(Qt::AlignLeft);
    regionVisitedLayout->addWidget(regionVisitedLabel);
    regionVisitedLayout->addWidget(regionVisitedCheckBox);

    QPushButton* saveButton = new QPushButton("Save");
    Q_ASSERT(saveButton != nullptr);
    saveButton->setEnabled(false);

    QVBoxLayout* propsLayout = new QVBoxLayout();
    Q_ASSERT(propsLayout != nullptr);
    propsLayout->setAlignment(Qt::AlignTop);
    propsLayout->addLayout(regionNameLayout);
    propsLayout->addLayout(regionVisitedLayout);
    propsLayout->addWidget(saveButton);

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

    m_name = regionNameEdit;
    m_flag = regionVisitedCheckBox;
    m_save = saveButton;

    // Center window

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(1400, 800),
            screen()->availableGeometry()));

    // Set signals

    QObject::connect(
        m_view, SIGNAL(regionChecked(MapArea*)),
        this, SLOT(regionChecked(MapArea*)));
    QObject::connect(
        m_view, SIGNAL(regionUnchecked()),
        this, SLOT(regionUnchecked()));
    QObject::connect(
        m_save, SIGNAL(clicked()),
        this, SLOT(regionSaved()));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);
    m_view->store();
}

void MainWindow::regionChecked(MapArea* area) {
    Q_ASSERT(area != nullptr);
    if (m_currentRegion != nullptr) {
        regionUnchecked();
    }
    m_currentRegion = area;

    m_currentRegion->setChecked(true);

    m_name->setEnabled(true);
    m_name->setText(m_currentRegion->getName());

    m_flag->setEnabled(true);
    m_flag->setCheckState(m_currentRegion->isVisited() ? Qt::Checked : Qt::Unchecked);

    m_save->setEnabled(true);

    m_view->updateScene();
}

void MainWindow::regionUnchecked() {
    m_name->setEnabled(false);
    m_name->setText("");

    m_flag->setEnabled(false);
    m_flag->setCheckState(Qt::Unchecked);

    m_save->setEnabled(false);

    m_currentRegion->setChecked(false);
    m_currentRegion = nullptr;

    m_view->updateScene();
}

void MainWindow::regionSaved() {
    Q_ASSERT(m_currentRegion != nullptr);

    m_currentRegion->setVisited(m_flag->isChecked());

    regionUnchecked();
}
