#ifndef PHOTOVIEW_H
#define PHOTOVIEW_H

#include <QGraphicsView>

class PhotoView : public QGraphicsView
{
    Q_OBJECT
public:
    PhotoView();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // PHOTOVIEW_H
