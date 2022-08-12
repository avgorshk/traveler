#include "mainwindow.h"

#include <QGroupBox>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScreen>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow{parent}, m_view(new MapView), m_photo(new PhotoView),
          m_currentRegion(nullptr), m_currentPoint(nullptr) {
    // Make menu

    QMenuBar* menuBar = this->menuBar();
    QMenu* menu = new QMenu("&File");
    QAction* russiaAction = menu->addAction("&Russia", this, SLOT(selectRussia()));
    russiaAction->setCheckable(true);
    russiaAction->setChecked(true);
    QAction* worldAction = menu->addAction("&World", this, SLOT(selectWorld()));
    worldAction->setCheckable(true);
    worldAction->setChecked(false);
    menu->addSeparator();
    menu->addAction("&Exit", this, SLOT(close()));
    menuBar->addMenu(menu);

    m_russiaAction = russiaAction;
    m_worldAction = worldAction;

    // Make layout

    //// Properties

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
    propsLayout->addWidget(m_photo);
    propsLayout->addWidget(saveButton);

    QGroupBox* propsBox = new QGroupBox("Properties");
    Q_ASSERT(propsBox != nullptr);
    propsBox->setMinimumWidth(512);
    propsBox->setLayout(propsLayout);

    //// Statistics

    QLabel* regionsVisited = new QLabel("Regions Visited:");
    Q_ASSERT(regionsVisited != nullptr);
    QLabel* pointsVisited = new QLabel("Points Visited:");
    Q_ASSERT(pointsVisited != nullptr);
    QVBoxLayout* statsLayout = new QVBoxLayout();
    Q_ASSERT(statsLayout != nullptr);
    statsLayout->setAlignment(Qt::AlignTop);
    statsLayout->addWidget(regionsVisited);
    statsLayout->addWidget(pointsVisited);

    QGroupBox* statsBox = new QGroupBox("Statistics");
    Q_ASSERT(statsBox != nullptr);
    statsBox->setLayout(statsLayout);

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

    m_regionsVisited = regionsVisited;
    m_pointsVisited = pointsVisited;

    resetPanels();

    // Center window

    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(1800, 1000),
            screen()->availableGeometry()));

    // Set title
    setWindowTitle("Russia");

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

    QObject::connect(
        m_view, SIGNAL(statsChanged(uint,uint,uint)),
        this, SLOT(statsChanged(uint,uint,uint)));

    m_view->updateStats();
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
    m_photo->disable();
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
                m_currentPoint->setPhoto(
                            m_photo->filename(),
                            getMapPrefix());
            } else {
                m_view->removePoint(m_currentPoint);
            }
        } else {
            if (m_flag->isChecked()) {
                MapPoint* point = m_view->addNewPoint(m_name->text());
                point->setPhoto(
                            m_photo->filename(),
                            getMapPrefix());
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
    m_photo->enable();
}

void MainWindow::pointChecked(MapPoint* point) {
    Q_ASSERT(point != nullptr);

    Q_ASSERT(m_currentPoint == nullptr);
    m_currentPoint = point;
    m_currentPoint->setChecked(true);

    setPanels("Point:", point->getName(), true);
    m_photo->enable();
    m_photo->load(m_currentPoint->getPhotoFilePath(getMapPrefix()));
}

void MainWindow::pointUnchecked() {
    if (m_currentPoint != nullptr) {
        m_currentPoint->setChecked(false);
        m_currentPoint = nullptr;
    }

    resetPanels();
}

void MainWindow::selectRussia() {
    m_worldAction->setChecked(false);
    m_russiaAction->setChecked(true);
    m_view->selectLocation(Location::Russia);
    setWindowTitle("Russia");
    resetPanels();
    m_currentPoint = nullptr;
    m_currentRegion = nullptr;
}

void MainWindow::selectWorld() {
    m_worldAction->setChecked(true);
    m_russiaAction->setChecked(false);
    m_view->selectLocation(Location::World);
    setWindowTitle("World");
    resetPanels();
    m_currentPoint = nullptr;
    m_currentRegion = nullptr;
}

void MainWindow::statsChanged(
        uint regionsTotal,
        uint regionsVisited,
        uint pointsVisited) {
    QString regions = "Regions Visited: ";
    regions += QString::number(regionsVisited) + " of " +
            QString::number(regionsTotal);
    float percent = 100.0f * regionsVisited / regionsTotal;
    regions += " (" + QString::number(percent, 'f', 2) + "%)";
    m_regionsVisited->setText(regions);

    QString points = "Points Visited: ";
    points += QString::number(pointsVisited);
    m_pointsVisited->setText(points);
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

    m_photo->disable();
}

QString MainWindow::getMapPrefix() const {
    return m_russiaAction->isChecked() ? "russia" : "world";
}
