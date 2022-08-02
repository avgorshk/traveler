#include "mapview.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QMouseEvent>

const char* RUSSIA_BASE_FILE_NAME = "data/russia-base.svg";
const char* RUSSIA_FILE_NAME = "data/russia.svg";

MapView::MapView(QWidget *parent)
        : QGraphicsView{parent}, m_map(nullptr) {
    auto scene = new QGraphicsScene(this);
    setScene(scene);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(ScrollHandDrag);
    setViewportUpdateMode(FullViewportUpdate);

    auto execPath = QCoreApplication::applicationDirPath();
    m_filePath = QDir::cleanPath(execPath + QDir::separator() + RUSSIA_FILE_NAME);
    if (QFileInfo::exists(m_filePath)) {
        load(m_filePath);
    } else {
        auto filePath = QDir::cleanPath(execPath + QDir::separator() + RUSSIA_BASE_FILE_NAME);
        load(filePath);
        store(m_filePath);
    }

    updateScene();
}

MapView::~MapView() {
    if (m_map != nullptr) {
        delete m_map;
    }
}

qreal MapView::zoomFactor() const {
    return transform().m11();
}

static float getValue(const QString& path, uint& start) {
    Q_ASSERT(!path.isEmpty());
    Q_ASSERT(start < path.size());

    uint i = start;
    QChar c = path[i];
    while (c.isSpace()) {
        ++i;
        Q_ASSERT(i < path.size());
        c = path[i];
    }

    QString x = "";
    if (c == '-') {
        x.push_back(c);
        ++i;
        Q_ASSERT(i < path.size());
        c = path[i];
    }
    while (c.isDigit() || c == '.') {
        x.push_back(c);
        ++i;
        Q_ASSERT(i < path.size());
        c = path[i];
    }
    if (c == 'e') {
        x.push_back(c);
        ++i;
        Q_ASSERT(i < path.size());
        c = path[i];

        if (c == '-') {
            x.push_back(c);
            ++i;
            Q_ASSERT(i < path.size());
            c = path[i];
        }
        while (c.isDigit()) {
            x.push_back(c);
            ++i;
            Q_ASSERT(i < path.size());
            c = path[i];
        }
    }

    bool ok = false;
    x.toFloat(&ok);
    Q_ASSERT(ok);

    start = i;
    return x.toFloat();
}

static QPointF getPoint(const QString& path, uint& start) {
    Q_ASSERT(!path.isEmpty());
    Q_ASSERT(start < path.size());

    float x = 0, y = 0;

    x = getValue(path, start);

    QChar c = path[start];
    Q_ASSERT(c.isSpace() || c == '-');
    while (c.isSpace()) {
        ++start;
        Q_ASSERT(start < path.size());
        c = path[start];
    }

    y = getValue(path, start);

    return QPointF(x, y);
}

static QPolygonF getPolygon(const QString& path, QPointF& base) {
    Q_ASSERT(!path.isEmpty());
    Q_ASSERT(path[0] == 'm');

    QPolygonF polygon;

    uint id = 1;
    QPointF start_point = base;
    start_point += getPoint(path, id);
    polygon.push_back(start_point);

    while (path[id].isSpace() || path[id] == '-') {
        Q_ASSERT(id < path.size());
        QPointF point = getPoint(path, id);
        QPointF prev = polygon.back();
        polygon.push_back(prev + point);
    }

    while (id < path.size()) {
        switch (path[id].unicode()) {
        case u'v': {
            ++id;
            do {
                Q_ASSERT(id < path.size());
                float value = getValue(path, id);
                QPointF prev = polygon.back();
                polygon.push_back(QPointF(prev.x(), prev.y() + value));
            } while (path[id].isSpace() || path[id] == '-');
            break;
        }
        case u'h': {
            ++id;
            do {
                Q_ASSERT(id < path.size());
                float value = getValue(path, id);
                QPointF prev = polygon.back();
                polygon.push_back(QPointF(prev.x() + value, prev.y()));
            } while (path[id].isSpace() || path[id] == '-');
            break;
        }
        case u'l': {
            ++id;
            do {
                Q_ASSERT(id < path.size());
                QPointF point = getPoint(path, id);
                QPointF prev = polygon.back();
                polygon.push_back(prev + point);
            } while (path[id].isSpace() || path[id] == '-');
            break;
        }
        case u'z': {
            ++id;
            Q_ASSERT(id == path.size());
            break;
        }
        default: {
            Q_ASSERT(false);
            break;
        }
        }
    }

    base = start_point;
    return polygon;
}

void MapView::load(const QString& filename) {
    Q_ASSERT(!filename.isEmpty());

    QFile file(filename);
    bool ok = file.open(QFile::ReadOnly);
    Q_ASSERT(ok);
    m_doc.setContent(&file);
    file.close();

    QDomElement root = m_doc.documentElement();
    Q_ASSERT(root.tagName() == "svg");

    QDomAttr width = root.attributeNode("width");
    Q_ASSERT(!width.isNull());
    auto w = width.value().toUInt();

    QDomAttr height = root.attributeNode("height");
    Q_ASSERT(!height.isNull());
    auto h = height.value().toUInt();

    m_map = new MapObject(w, h);
    Q_ASSERT(m_map != nullptr);

    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        QDomElement element = node.toElement();
        auto name = element.tagName();
        if (name == "g") {
            QDomNode sub_node = node.firstChild();
            while (!sub_node.isNull()) {
                QDomElement sub_element = sub_node.toElement();
                auto sub_name = sub_element.tagName();
                if (sub_name == "path") {
                    MapArea area;
                    area.name = "";
                    area.element = &sub_element;
                    area.visited = false;

                    QPointF base(0.0f, 0.0f);
                    auto content = sub_element.attribute("d");
                    auto path_list = content.split('z');
                    for (auto& path : path_list) {
                        if (path.isEmpty()) {
                            continue;
                        }
                        path.push_back('z');
                        auto polygon = getPolygon(path, base);
                        Q_ASSERT(!polygon.isEmpty());

                        area.area.push_back(polygon);
                    }

                    if (!area.area.empty()) {
                        m_map->addArea(area);
                    }
                }
                sub_node = sub_node.nextSibling();
            }
        }
        node = node.nextSibling();
    }
}

void MapView::store(const QString& filename) {
    Q_ASSERT(!filename.isEmpty());

    QFile file(filename);
    bool ok = file.open(QFile::WriteOnly | QFile::Text);
    Q_ASSERT(ok);

    QTextStream stream(&file);
    stream << m_doc.toString();
    file.close();
}

void MapView::updateScene() const {
    QGraphicsScene *s = scene();
    s->clear();

    const QVector<MapArea>& area_list = m_map->getAreaList();
    for (const MapArea& area : area_list) {
        for (const QPolygonF& p : area.area) {
            s->addPolygon(p, QPen(), QBrush(QColorConstants::Gray));
        }
    }
}

void MapView::zoomBy(qreal factor) {
    const qreal currentZoom = zoomFactor();
    if ((factor < 1 && currentZoom < 0.1) || (factor > 1 && currentZoom > 10)) {
        return;
    }
    scale(factor, factor);
}

void MapView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);
}

void MapView::wheelEvent(QWheelEvent *event) {
    zoomBy(qPow(1.2, event->angleDelta().y() / 240.0));
}

void MapView::mousePressEvent(QMouseEvent *event) {
    qDebug() << mapToScene(event->pos());
    QGraphicsView::mousePressEvent(event);
}
