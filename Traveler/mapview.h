#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QDomDocument>
#include <QGraphicsView>
#include <QVector>

#include "mapobject.h"

class MapView : public QGraphicsView {
    Q_OBJECT
public:
    explicit MapView(QWidget *parent = nullptr);
    ~MapView();

private:
    void load(const QString& filename);
    QVector<QPoint> getPolygon(const QString& path) const;

private:
    QDomDocument m_doc;
    MapObject* m_map;
};

#endif // MAPVIEW_H
