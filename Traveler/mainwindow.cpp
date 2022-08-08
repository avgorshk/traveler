#include "mainwindow.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
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

    QHBoxLayout* regionNameLayout = new QHBoxLayout();
    Q_ASSERT(regionNameLayout != nullptr);
    regionNameLayout->addWidget(regionNameLabel);
    regionNameLayout->addWidget(regionNameEdit);

    QLabel* regionVisitedLabel = new QLabel("Visited:");
    Q_ASSERT(regionVisitedLabel != nullptr);
    regionVisitedLabel->setMinimumWidth(100);
    QCheckBox* regionVisitedCheckBox = new QCheckBox();
    Q_ASSERT(regionVisitedCheckBox != nullptr);

    QHBoxLayout* regionVisitedLayout = new QHBoxLayout();
    Q_ASSERT(regionVisitedLayout != nullptr);
    regionVisitedLayout->setAlignment(Qt::AlignLeft);
    regionVisitedLayout->addWidget(regionVisitedLabel);
    regionVisitedLayout->addWidget(regionVisitedCheckBox);

    QPushButton* saveButton = new QPushButton("Save");
    Q_ASSERT(saveButton != nullptr);

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
    m_label = regionNameLabel;

    resetPanels();

    // Center window

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(1400, 800),
            screen()->availableGeometry()));

    // Set signals

    QObject::connect(
        m_view, SIGNAL(regionChecked(MapRegion*)),
        this, SLOT(regionChecked(MapRegion*)));
    QObject::connect(
        m_view, SIGNAL(regionUnchecked()),
        this, SLOT(regionUnchecked()));
    QObject::connect(
        m_save, SIGNAL(clicked()),
        this, SLOT(regionSaved()));
    QObject::connect(
        m_view, SIGNAL(pointAdded(QPointF)),
        this, SLOT(pointAdded(QPointF)));
    QObject::connect(
        m_view, SIGNAL(pointUnchecked()),
        this, SLOT(pointUnchecked()));
}

// Protected Signals

void MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);
    m_view->store();
}

// Protected Slots

void MainWindow::regionChecked(MapRegion* region) {
    Q_ASSERT(region != nullptr);

    m_currentRegion = region;
    m_currentRegion->setChecked(true);

    setPanels("Region", m_currentRegion->getName(), m_currentRegion->isVisited());
}

void MainWindow::regionUnchecked() {
    if (m_currentRegion != nullptr) {
        m_currentRegion->setChecked(false);
        m_currentRegion = nullptr;
    }

    resetPanels();
}

void MainWindow::regionSaved() {
    Q_ASSERT(m_currentRegion != nullptr);

    m_currentRegion->setName(m_name->text());
    m_currentRegion->setVisited(m_flag->isChecked());

    regionUnchecked();
    m_view->updateScene();
}

void MainWindow::pointAdded(QPointF point) {
    setPanels("Point:", "", true);
}

void MainWindow::pointUnchecked() {
    resetPanels();
}

// Private Methods

void MainWindow::setPanels(const QString& label, const QString& text, bool flag) {
    m_name->setEnabled(true);
    m_name->setText(text);

    m_flag->setEnabled(true);
    m_flag->setCheckState(flag ? Qt::Checked : Qt::Unchecked);

    m_save->setEnabled(true);

    m_label->setText(label);
}

void MainWindow::resetPanels() {
    m_name->setEnabled(false);
    m_name->setText("");

    m_flag->setEnabled(false);
    m_flag->setCheckState(Qt::Unchecked);

    m_save->setEnabled(false);

    m_label->setText("Region/Point:");
}
