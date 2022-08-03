#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include "maparea.h"

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

    const QVector<MapArea>& getAreaList() const {
        return m_area_list;
    }

    MapArea* getArea(QPointF point) {
        MapArea* result = nullptr;
        for (auto& area : m_area_list) {
            for (auto& polygon : area.getPolygonList()) {
                if (polygon.containsPoint(point, Qt::OddEvenFill)) {
                    result = &area;
                }
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
                        MapArea area(sub_element);
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

                            area.addPolygon(polygon);
                        }

                        if (!area.getPolygonList().isEmpty()) {
                            m_area_list.push_back(area);
                        }
                    }
                    sub_node = sub_node.nextSibling();
                }
            }
            node = node.nextSibling();
        }
    }

private:
    uint m_width;
    uint m_height;
    QVector<MapArea> m_area_list;
    QDomDocument m_doc;
};

#endif // MAPOBJECT_H
