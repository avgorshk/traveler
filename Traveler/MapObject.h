#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <QDomElement>
#include <QPoint>
#include <QVector>

struct MapArea {
    QVector<QVector<QPointF>> area;
    QString name;
    QDomElement *element = nullptr;
    bool visited = false;
};

class MapObject {
public:
    MapObject(quint32 width, quint32 height)
        : m_width(width), m_height(height) {}

    void addArea(const MapArea& area) {
        m_area_list.push_back(area);
    }

private:
    uint m_width;
    uint m_height;
    QVector<MapArea> m_area_list;
};

#endif // MAPOBJECT_H
