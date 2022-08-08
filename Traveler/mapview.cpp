#include "mapview.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QMouseEvent>

const char* RUSSIA_BASE_FILE_NAME = "data/russia-base.svg";
const char* RUSSIA_FILE_NAME = "data/russia.svg";

const float POINT_RADIUS = 16.0f;

// Public Methods

MapView::MapView(QWidget *parent)
        : QGraphicsView{parent}, m_map(nullptr), m_point(nullptr) {
    auto scene = new QGraphicsScene(this);
    setScene(scene);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);

    auto execPath = QCoreApplication::applicationDirPath();
    m_filePath = QDir::cleanPath(execPath + QDir::separator() + RUSSIA_FILE_NAME);
    if (QFileInfo::exists(m_filePath)) {
        m_map = new MapObject(m_filePath);
        updateScene();
    } else {
        auto filePath = QDir::cleanPath(execPath + QDir::separator() + RUSSIA_BASE_FILE_NAME);
        if (QFileInfo::exists(filePath)) {
            QMessageBox msgBox;
            msgBox.setText("Unable to find Russia map file: " + m_filePath + ". Base file will be used");
            msgBox.setWindowTitle("Warning");
            msgBox.exec();

            m_map = new MapObject(filePath);
            updateScene();
        } else {
            QMessageBox msgBox;
            msgBox.setText("Unable to find Russia base map file: " + filePath);
            msgBox.setWindowTitle("Warning");
            msgBox.exec();
        }
    }
}

MapView::~MapView() {
    if (m_map != nullptr) {
        delete m_map;
    }
    if (m_point != nullptr) {
        delete m_point;
    }
}

qreal MapView::zoomFactor() const {
    return transform().m11();
}

void MapView::updateScene() const {
    QGraphicsScene *s = scene();
    s->clear();

    const QVector<MapRegion>& region_list = m_map->getRegionList();
    for (const MapRegion& region : region_list) {
        QBrush brush(QColorConstants::Svg::lightgray);
        if (region.isVisited()) {
            brush.setColor(QColorConstants::Svg::lightgreen);
        }
        if (region.isChecked()) {
            brush.setColor(QColorConstants::Svg::lightyellow);
        }

        for (const QPolygonF& p : region.getPolygonList()) {
            s->addPolygon(p, QPen(), brush);
        }
    }

    if (m_point) {
        QBrush brush(QColorConstants::Svg::orange);
        s->addEllipse(
            m_point->x(), m_point->y(), POINT_RADIUS, POINT_RADIUS,
            QPen(), brush);
    }
}

void MapView::store() const {
    Q_ASSERT(m_map != nullptr);
    Q_ASSERT(m_filePath != nullptr);
    m_map->store(m_filePath);
}

void MapView::unsetPoint() {
    if (m_point != nullptr) {
        delete m_point;
    }
    m_point = nullptr;
}

// Private Methods

void MapView::zoomBy(qreal factor) {
    const qreal currentZoom = zoomFactor();
    if ((factor < 1 && currentZoom < 0.1) || (factor > 1 && currentZoom > 10)) {
        return;
    }
    scale(factor, factor);
}

void MapView::setPoint(QPointF point) {
    m_point = new QPointF(point);
}

// Protected Signals

void MapView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);
}

void MapView::wheelEvent(QWheelEvent *event) {
    zoomBy(qPow(1.2, event->angleDelta().y() / 240.0));
}

void MapView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        QPointF point = mapToScene(event->pos());
        MapRegion* region = m_map->getRegion(point);
        if (region != nullptr) {
            unsetPoint();
            emit pointUnchecked();
            emit regionUnchecked();

            emit regionChecked(region);
        } else {
            unsetPoint();
            emit pointUnchecked();
            emit regionUnchecked();
        }
        updateScene();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void MapView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF point = mapToScene(event->pos());
        unsetPoint();
        emit pointUnchecked();
        emit regionUnchecked();

        MapRegion* region = m_map->getRegion(point);
        if (region != nullptr) {
            setPoint(point);
            emit pointAdded(point);
        }

        updateScene();
    }
}
