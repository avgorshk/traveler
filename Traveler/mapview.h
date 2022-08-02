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

protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void load(const QString& filename);
    void updateScene() const;
    void zoomBy(qreal factor);

private:
    QDomDocument m_doc;
    MapObject* m_map;
};

#endif // MAPVIEW_H
