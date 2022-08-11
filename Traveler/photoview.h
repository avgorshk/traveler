#ifndef PHOTOVIEW_H
#define PHOTOVIEW_H

#include <QGraphicsView>

class PhotoView : public QGraphicsView
{
    Q_OBJECT
public:
    PhotoView();
    void enable();
    void disable();
    void load(const QString& filename);

    QString filename() {
        return m_filename;
    }

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_filename;
};

#endif // PHOTOVIEW_H
