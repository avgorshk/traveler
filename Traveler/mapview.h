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

    void markChanged();
    void store();

    MapPoint* addNewPoint(const QString& name);
    void unsetNewPoint();
    void removePoint(MapPoint* point);

    void selectLocation(Location location);
    void updateStats();

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
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void zoomBy(qreal factor);
    void setNewPoint(QPointF point);
    void releaseMap();

private:
    MapObject* m_map;
    QString m_filePath;

    QPointF* m_newPoint;
    bool m_changed;
};

#endif // MAPVIEW_H
