#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QDomDocument>
#include <QGraphicsView>
#include <QVector>

#include "mapobject.h"

enum Location {
    Russia,
    World
};

class MapView : public QGraphicsView {
    Q_OBJECT
public:
    explicit MapView(
            QWidget *parent = nullptr);
    ~MapView();

    qreal zoomFactor() const;
    void updateScene();
    void store() const;

    void addNewPoint(const QString& name);
    void unsetNewPoint();
    void removePoint(MapPoint* point);

    void selectLocation(Location location);

signals:
    void regionChecked(MapRegion* region);
    void regionUnchecked();

    void pointAdded();
    void pointChecked(MapPoint* point);
    void pointUnchecked();

    void statsChanged(
            uint regionsTotal,
            uint regionsVisited,
            uint pointsVisited);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void zoomBy(qreal factor);
    void setNewPoint(QPointF point);
    void releaseMap();

private:
    MapObject* m_map;
    QString m_filePath;

    QPointF* m_newPoint;
};

#endif // MAPVIEW_H
