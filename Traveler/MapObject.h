#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <QDomElement>
#include <QPoint>
#include <QVector>

struct MapArea {
    QVector<QVector<QPoint>> area;
    QString name;
    QDomElement *element = nullptr;
    bool visited = false;
};

class MapObject {
public:
    MapObject(quint32 width, quint32 height)
        : m_width(width), m_height(height) {}
private:
    quint32 m_width;
    quint32 m_height;
    QVector<MapArea> m_area_list;
};

#endif // MAPOBJECT_H
