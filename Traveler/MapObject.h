#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include "mappoint.h"
#include "mapregion.h"

#include <QDomDocument>
#include <QFile>
#include <QTextStream>

class MapObject {
public:
    MapObject(const QString& filename) {
        Q_ASSERT(!filename.isEmpty());
        load(filename);
    }

    QSize getSize() const {
        return QSize(m_width, m_height);
    }

    float getPointRadius() const {
        return m_pointRadius;
    }

    const QVector<MapRegion>& getRegionList() const {
        return m_region_list;
    }

    MapRegion* getRegion(QPointF point) {
        MapRegion* result = nullptr;
        for (auto& region : m_region_list) {
            for (auto& polygon : region.getPolygonList()) {
                if (polygon.containsPoint(point, Qt::OddEvenFill)) {
                    result = &region;
                    break;
                }
            }
        }
        return result;
    }

    const QVector<MapPoint>& getPointList() const {
        return m_point_list;
    }

    MapPoint* getPoint(QPointF point) {
        MapPoint* result = nullptr;
        for (auto& p : m_point_list) {
            float dx = p.getPoint().x() - point.x();
            float dy = p.getPoint().y() - point.y();
            if (dx * dx + dy * dy <= m_pointRadius * m_pointRadius) {
                result = &p;
                break;
            }
        }
        return result;
    }

    void store(const QString& filename) const {
        Q_ASSERT(!filename.isEmpty());

        QFile file(filename);
        bool ok = file.open(QFile::WriteOnly | QFile::Text);
        Q_ASSERT(ok);

        QTextStream stream(&file);
        stream << m_doc.toString();
        file.close();
    }

    MapPoint* addPoint(QPointF point, const QString& name) {
        QDomElement element = m_doc.createElement("circle");
        element.setAttribute("cx", point.x());
        element.setAttribute("cy", point.y());
        element.setAttribute("r", m_pointRadius);

        QDomElement title_element = m_doc.createElement("title");
        QDomText title_text = m_doc.createTextNode(name);
        title_element.appendChild(title_text);
        element.appendChild(title_element);
        m_points_group.appendChild(element);

        m_point_list.emplace_back(
            m_doc, m_points_group, element, point, name);
        return &m_point_list.back();
    }

    void removePoint(MapPoint* point) {
        Q_ASSERT(point != nullptr);

        point->remove();
        bool ok = m_point_list.removeOne(*point);
        Q_ASSERT(ok);
    }

    void getStats(
            uint& regionsTotal,
            uint& regionsVisited,
            uint& pointsVisited) const {
        regionsVisited = 0;
        for (auto& region : m_region_list) {
            if (region.isVisited()) {
                ++regionsVisited;
            }
        }

        regionsTotal = m_region_list.size();
        pointsVisited = m_point_list.size();
    }

private:
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
        Q_ASSERT(path[0] == 'm' || path[0] == 'M');

        QPolygonF polygon;

        uint id = 1;
        QPointF start_point(0.0f, 0.0f);
        if (path[0] == 'm') {
            start_point = base;
        }
        start_point += getPoint(path, id);
        polygon.push_back(start_point);

        if (path[0] == 'm') {
            while (path[id].isSpace() || path[id] == '-') {
                Q_ASSERT(id < path.size());
                QPointF point = getPoint(path, id);
                QPointF prev = polygon.back();
                polygon.push_back(prev + point);
            }
        } else { // 'M'
            while (path[id].isSpace() || path[id] == '-') {
                Q_ASSERT(id < path.size());
                QPointF point = getPoint(path, id);
                polygon.push_back(point);
            }
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

    void load(const QString& filename) {
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
        m_width = width.value().toUInt();

        QDomAttr height = root.attributeNode("height");
        Q_ASSERT(!height.isNull());
        m_height = height.value().toUInt();

        m_pointRadius = qMax(m_width, m_height) / 1024.0f;

        // Paths

        QDomNode node = root.firstChild();
        Q_ASSERT(!node.isNull());

        QDomElement element = node.toElement();
        auto name = element.tagName();
        Q_ASSERT(name == "g");

        QDomNode sub_node = node.firstChild();
        while (!sub_node.isNull()) {
            QDomElement sub_element = sub_node.toElement();
            auto sub_name = sub_element.tagName();
            if (sub_name == "path") {
                bool visited = false;
                if (sub_element.hasAttribute("fill")) {
                    visited = true;
                }

                QString name("");
                if (sub_element.hasChildNodes()) {
                    QDomNode title_node = sub_node.firstChild();
                    QDomElement title_element = title_node.toElement();
                    name = title_element.text();
                } else {
                    if (sub_element.hasAttribute("name")) {
                        name = sub_element.attribute("name");
                    }
                }

                MapRegion region(m_doc, sub_element, name, visited);

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

                    region.addPolygon(polygon);
                }

                if (!region.getPolygonList().isEmpty()) {
                    m_region_list.push_back(region);
                }
            }
            sub_node = sub_node.nextSibling();
        }

        // Points

        node = node.nextSibling();
        element = node.toElement();
        name = element.tagName();
        Q_ASSERT(name == "g");
        m_points_group = element;

        sub_node = node.firstChild();
        while (!sub_node.isNull()) {
            QDomElement sub_element = sub_node.toElement();
            auto sub_name = sub_element.tagName();
            if (sub_name == "circle") {
                bool ok = false;

                Q_ASSERT(sub_element.hasAttribute("cx"));
                float x = sub_element.attribute("cx").toFloat(&ok);
                Q_ASSERT(ok);

                Q_ASSERT(sub_element.hasAttribute("cy"));
                float y = sub_element.attribute("cy").toFloat(&ok);
                Q_ASSERT(ok);

                QString name("");
                if (sub_element.hasChildNodes()) {
                    QDomNode title_node = sub_node.firstChild();
                    QDomElement title_element = title_node.toElement();
                    name = title_element.text();
                }

                m_point_list.emplace_back(
                    m_doc, m_points_group, sub_element, QPointF(x, y), name);
            }

            sub_node = sub_node.nextSibling();
        }
    }

private:
    uint m_width;
    uint m_height;
    float m_pointRadius;

    QVector<MapRegion> m_region_list;
    QVector<MapPoint> m_point_list;

    QDomDocument m_doc;
    QDomElement m_points_group;
};

#endif // MAPOBJECT_H
