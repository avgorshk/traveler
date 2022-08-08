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

    qreal zoomFactor() const;
    void updateScene() const;
    void store() const;
    void unsetPoint();

signals:
    void regionChecked(MapRegion* region);
    void regionUnchecked();
    void pointAdded(QPointF point);
    void pointUnchecked();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void zoomBy(qreal factor);
    void setPoint(QPointF point);

private:
    MapObject* m_map;
    QString m_filePath;

    QPointF* m_point;
};

#endif // MAPVIEW_H
