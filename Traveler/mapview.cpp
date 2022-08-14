#include "mapview.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolTip>

const char* RUSSIA_BASE_FILE_NAME = "data/russia-base.svg";
const char* RUSSIA_FILE_NAME = "data/russia.svg";
const char* WORLD_BASE_FILE_NAME = "data/world-base.svg";
const char* WORLD_FILE_NAME = "data/world.svg";

// Public Methods

MapView::MapView(QWidget *parent)
        : QGraphicsView{parent}, m_map(nullptr), m_newPoint(nullptr) {
    auto scene = new QGraphicsScene(this);
    setScene(scene);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);
    selectLocation(Location::Russia);
    viewport()->setCursor(Qt::ArrowCursor);
}

MapView::~MapView() {
    releaseMap();
}

qreal MapView::zoomFactor() const {
    return transform().m11();
}

void MapView::updateScene() {
    QGraphicsScene *s = scene();
    s->clear();
    s->setSceneRect(QRectF(QPointF(0, 0), m_map->getSize()));
    QPen pen(QBrush(QColorConstants::Black), 0.25f);

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
            s->addPolygon(p, pen, brush);
        }
    }

    float radius = m_map->getPointRadius();
    const QVector<MapPoint>& point_list = m_map->getPointList();
    for (const MapPoint& point : point_list) {
        QBrush brush(QColorConstants::Svg::firebrick);
        if (point.isChecked()) {
            brush.setColor(QColorConstants::Svg::orange);
        }
        s->addEllipse(
            point.getPoint().x() - radius,
            point.getPoint().y() - radius,
            2.0f * radius, 2.0f * radius,
            pen, brush);
    }

    if (m_newPoint) {
        QBrush brush(QColorConstants::Svg::orange);
        s->addEllipse(
            m_newPoint->x() - radius,
            m_newPoint->y() - radius,
            2.0f * radius, 2.0f * radius,
            pen, brush);
    }

    updateStats();
}

void MapView::store() const {
    if (m_map != nullptr) {
        Q_ASSERT(m_filePath != nullptr);
        m_map->store(m_filePath);
    }
}

void MapView::unsetNewPoint() {
    if (m_newPoint != nullptr) {
        delete m_newPoint;
    }
    m_newPoint = nullptr;
}

MapPoint* MapView::addNewPoint(const QString& name) {
    Q_ASSERT(m_newPoint != nullptr);
    return m_map->addPoint(*m_newPoint, name);
}

void MapView::removePoint(MapPoint* point) {
    Q_ASSERT(point != nullptr);
    m_map->removePoint(point);
}

void MapView::selectLocation(Location location) {
    const char* filename =
        location == Location::Russia ?
                    RUSSIA_FILE_NAME : WORLD_FILE_NAME;
    const char* baseFilename =
        location == Location::Russia ?
                    RUSSIA_BASE_FILE_NAME : WORLD_BASE_FILE_NAME;

    auto execPath = QCoreApplication::applicationDirPath();
    auto filePath = QDir::cleanPath(execPath + QDir::separator() + filename);
    if (m_filePath == filePath) {
        return;
    } else {
        store();
        releaseMap();
        m_filePath = filePath;
    }

    if (QFileInfo::exists(m_filePath)) {
        m_map = new MapObject(m_filePath);
        updateScene();
    } else {
        filePath = QDir::cleanPath(execPath + QDir::separator() + baseFilename);
        if (QFileInfo::exists(filePath)) {
            m_map = new MapObject(filePath);
            updateScene();
        } else {
            QMessageBox msgBox;
            msgBox.setText("Unable to find base map file: " + filePath);
            msgBox.setWindowTitle("Warning");
            msgBox.exec();
        }
    }
}

void MapView::updateStats() {
    uint regionsTotal = 0;
    uint regionsVisited = 0;
    uint poinsVisited = 0;
    m_map->getStats(regionsTotal, regionsVisited, poinsVisited);
    emit statsChanged(regionsTotal, regionsVisited, poinsVisited);
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

void MapView::releaseMap() {
    if (m_map != nullptr) {
        delete m_map;
        m_map = nullptr;
    }
    if (m_newPoint != nullptr) {
        delete m_newPoint;
        m_newPoint = nullptr;
    }
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

void MapView::mouseMoveEvent(QMouseEvent *event) {
    QGraphicsView::mouseMoveEvent(event);

    QString text;
    QPointF p = mapToScene(event->pos());
    auto point = m_map->getPoint(p);
    if (point != nullptr) {
        text = point->getName();
    } else {
        auto region = m_map->getRegion(p);
        if (region != nullptr) {
            text = region->getName();
        }
    }

    if (!text.isEmpty()) {
        QToolTip::showText(event->globalPosition().toPoint(), text);
    }
}
