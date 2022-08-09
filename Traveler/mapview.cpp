#include "mapview.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QMouseEvent>

const char* RUSSIA_BASE_FILE_NAME = "data/russia-base.svg";
const char* RUSSIA_FILE_NAME = "data/russia.svg";

// Public Methods

MapView::MapView(QWidget *parent)
        : QGraphicsView{parent}, m_map(nullptr), m_newPoint(nullptr) {
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

    viewport()->setCursor(Qt::ArrowCursor);
}

MapView::~MapView() {
    if (m_map != nullptr) {
        delete m_map;
    }
    if (m_newPoint != nullptr) {
        delete m_newPoint;
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

    const QVector<MapPoint>& point_list = m_map->getPointList();
    for (const MapPoint& point : point_list) {
        QBrush brush(QColorConstants::Svg::firebrick);
        if (point.isChecked()) {
            brush.setColor(QColorConstants::Svg::orange);
        }
        s->addEllipse(
            point.getPoint().x() - POINT_RADIUS,
            point.getPoint().y() - POINT_RADIUS,
            2.0f * POINT_RADIUS, 2.0f * POINT_RADIUS,
            QPen(), brush);
    }

    if (m_newPoint) {
        QBrush brush(QColorConstants::Svg::orange);
        s->addEllipse(
            m_newPoint->x() - POINT_RADIUS,
            m_newPoint->y() - POINT_RADIUS,
            2.0f * POINT_RADIUS, 2.0f * POINT_RADIUS,
            QPen(), brush);
    }
}

void MapView::store() const {
    Q_ASSERT(m_map != nullptr);
    Q_ASSERT(m_filePath != nullptr);
    m_map->store(m_filePath);
}

void MapView::unsetNewPoint() {
    if (m_newPoint != nullptr) {
        delete m_newPoint;
    }
    m_newPoint = nullptr;
}

void MapView::addNewPoint(const QString& name) {
    Q_ASSERT(m_newPoint != nullptr);
    m_map->addPoint(*m_newPoint, name);
}

void MapView::removePoint(MapPoint* point) {
    Q_ASSERT(point != nullptr);
    m_map->removePoint(point);
}

// Private Methods

void MapView::zoomBy(qreal factor) {
    const qreal currentZoom = zoomFactor();
    if ((factor < 1 && currentZoom < 0.1) || (factor > 1 && currentZoom > 10)) {
        return;
    }
    scale(factor, factor);
}

void MapView::setNewPoint(QPointF point) {
    m_newPoint = new QPointF(point);
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

        unsetNewPoint();
        emit pointUnchecked();
        emit regionUnchecked();

        MapPoint* p = m_map->getPoint(point);
        if (p != nullptr) {
            emit pointChecked(p);
        } else {
            MapRegion* region = m_map->getRegion(point);
            if (region != nullptr) {
                emit regionChecked(region);
            }
        }

        updateScene();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void MapView::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPointF point = mapToScene(event->pos());
        unsetNewPoint();
        emit pointUnchecked();
        emit regionUnchecked();

        MapRegion* region = m_map->getRegion(point);
        if (region != nullptr) {
            setNewPoint(point);
            emit pointAdded();
        }

        updateScene();
    }
}

void MapView::mouseReleaseEvent(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
    viewport()->setCursor(Qt::ArrowCursor);
}
