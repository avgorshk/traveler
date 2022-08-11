#include "photoview.h"

#include <QFileDialog>
#include <QGraphicsTextItem>
#include <QMouseEvent>

// Public Methods

PhotoView::PhotoView() {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto scene = new QGraphicsScene(this);
    setScene(scene);
    disable();
}

void PhotoView::enable() {
    auto scene = this->scene();
    Q_ASSERT(scene != nullptr);
    scene->clear();
    scene->setSceneRect(QRectF());
    scene->addText("Add Photo");
    setEnabled(true);
}

void PhotoView::disable() {
    auto scene = this->scene();
    Q_ASSERT(scene != nullptr);
    scene->clear();
    scene->setSceneRect(QRectF());
    scene->setBackgroundBrush(QPalette().window());
    scene->addText("Add Photo")->setDefaultTextColor(QColorConstants::Gray);
    setEnabled(false);
    m_filename = "";
}

void PhotoView::load(const QString& filename) {
    Q_ASSERT(!filename.isEmpty());

    if (QFileInfo::exists(filename)) {
        m_filename = filename;
        QImage image(m_filename);
        image = image.scaled(
                    this->size(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);
        auto scene = this->scene();
        Q_ASSERT(scene != nullptr);
        scene->clear();
        scene->setSceneRect(QRectF(QPointF(0, 0), image.size()));
        scene->addPixmap(QPixmap::fromImage(image));
    }
}

// Protected Methods

void PhotoView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (m_filename.isEmpty()) {
            QFileDialog dialog(this);
            dialog.setWindowTitle("Open JPEG Image File");
            dialog.setNameFilter(tr("Images (*.jpg *.jpeg)"));
            if (dialog.exec()) {
                auto fileList = dialog.selectedFiles();
                if (!fileList.isEmpty()) {
                    load(fileList.front());
                }
            }
        } else {
            m_filename = "";
            enable();
        }
    }

    QGraphicsView::mousePressEvent(event);
}
