#include "mainwindow.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow{parent}, m_view(new MapView),
          m_currentRegion(nullptr), m_currentPoint(nullptr) {
    // Make layout

    QLabel* nameLabel = new QLabel("Region/Point:");
    Q_ASSERT(nameLabel != nullptr);
    nameLabel->setMinimumWidth(100);
    QLineEdit* nameEdit = new QLineEdit();
    Q_ASSERT(nameEdit != nullptr);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    Q_ASSERT(nameLayout != nullptr);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);

    QLabel* visitedLabel = new QLabel("Visited:");
    Q_ASSERT(visitedLabel != nullptr);
    visitedLabel->setMinimumWidth(100);
    QCheckBox* visitedCheckBox = new QCheckBox();
    Q_ASSERT(visitedCheckBox != nullptr);

    QHBoxLayout* visitedLayout = new QHBoxLayout();
    Q_ASSERT(visitedLayout != nullptr);
    visitedLayout->setAlignment(Qt::AlignLeft);
    visitedLayout->addWidget(visitedLabel);
    visitedLayout->addWidget(visitedCheckBox);

    QPushButton* saveButton = new QPushButton("Save");
    Q_ASSERT(saveButton != nullptr);

    QVBoxLayout* propsLayout = new QVBoxLayout();
    Q_ASSERT(propsLayout != nullptr);
    propsLayout->setAlignment(Qt::AlignTop);
    propsLayout->addLayout(nameLayout);
    propsLayout->addLayout(visitedLayout);
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

    m_name = nameEdit;
    m_flag = visitedCheckBox;
    m_save = saveButton;
    m_label = nameLabel;

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
        this, SLOT(saved()));

    QObject::connect(
        m_view, SIGNAL(pointAdded()),
        this, SLOT(pointAdded()));
    QObject::connect(
        m_view, SIGNAL(pointChecked(MapPoint*)),
        this, SLOT(pointChecked(MapPoint*)));
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

    Q_ASSERT(m_currentRegion == nullptr);
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

void MainWindow::saved() {
    if (m_currentRegion != nullptr) {
        m_currentRegion->setName(m_name->text());
        m_currentRegion->setVisited(m_flag->isChecked());
        regionUnchecked();
    } else {
        if (m_currentPoint != nullptr) {
            if (m_flag->isChecked()) {
                m_currentPoint->setName(m_name->text());
            } else {
                m_view->removePoint(m_currentPoint);
            }
        } else {
            if (m_flag->isChecked()) {
                m_view->addNewPoint(m_name->text());
            }
        }

        m_view->unsetNewPoint();
        pointUnchecked();
    }

    m_view->updateScene();
}

void MainWindow::pointAdded() {
    Q_ASSERT(m_currentPoint == nullptr);
    setPanels("Point:", "", true);
}

void MainWindow::pointChecked(MapPoint* point) {
    Q_ASSERT(point != nullptr);

    Q_ASSERT(m_currentPoint == nullptr);
    m_currentPoint = point;
    m_currentPoint->setChecked(true);

    setPanels("Point:", point->getName(), true);
}

void MainWindow::pointUnchecked() {
    if (m_currentPoint != nullptr) {
        m_currentPoint->setChecked(false);
        m_currentPoint = nullptr;
    }

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
