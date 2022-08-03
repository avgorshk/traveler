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

signals:
    void regionChecked(MapArea* area);
    void regionUnchecked();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void zoomBy(qreal factor);

private:
    MapObject* m_map;
    QString m_filePath;
};

#endif // MAPVIEW_H
