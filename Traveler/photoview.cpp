#include "photoview.h"

#include <QFileDialog>
#include <QMouseEvent>

PhotoView::PhotoView() {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto scene = new QGraphicsScene(this);
    setScene(scene);
    scene->setBackgroundBrush(QPalette().window());
    scene->addText("Add Photo");
}

void PhotoView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QFileDialog dialog(this);
        dialog.setWindowTitle("Open Image File");
        dialog.setNameFilter(tr("Images (*.png *.jpg *.jpeg *.bmp)"));
        if (dialog.exec()) {
            auto fileList = dialog.selectedFiles();
            if (!fileList.isEmpty()) {
                QImage image(fileList.front());
                image = image.scaled(
                            this->size(),
                            Qt::KeepAspectRatio,
                            Qt::SmoothTransformation);
                scene()->clear();
                scene()->setSceneRect(QRectF(QPointF(0, 0), image.size()));
                scene()->addPixmap(QPixmap::fromImage(image));
            }
        }
    }

    QGraphicsView::mousePressEvent(event);
}
