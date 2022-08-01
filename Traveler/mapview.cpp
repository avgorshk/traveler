#include "mapview.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>

const char* RUSSIA_FILE_NAME = "data/russia-base.svg";

MapView::MapView(QWidget *parent)
        : QGraphicsView{parent}, m_map(nullptr) {
    auto execPath = QCoreApplication::applicationDirPath();
    auto filePath = QDir::cleanPath(execPath + QDir::separator() + RUSSIA_FILE_NAME);
    if (QFileInfo::exists(filePath)) {
        load(filePath);
    }
}

MapView::~MapView() {
    if (m_map != nullptr) {
        delete m_map;
    }
}

QVector<QPoint> MapView::getPolygon(const QString& path) const {

}

void MapView::load(const QString& filename) {
    QFile file(filename);
    bool ok = file.open(QFile::ReadOnly);
    Q_ASSERT(ok);
    m_doc.setContent(&file);
    file.close();

    QDomElement root = m_doc.documentElement();
    Q_ASSERT(root.tagName() == "svg");

    QDomAttr width = root.attributeNode("width");
    Q_ASSERT(!width.isNull());
    auto w = width.value().toUInt();

    QDomAttr height = root.attributeNode("height");
    Q_ASSERT(!height.isNull());
    auto h = height.value().toUInt();

    m_map = new MapObject(w, h);
    Q_ASSERT(m_map != nullptr);

    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        QDomElement element = node.toElement();
        auto name = element.tagName();
        if (name == "g") {
            QDomNode sub_node = node.firstChild();
            while (!sub_node.isNull()) {
                QDomElement sub_element = sub_node.toElement();
                auto sub_name = sub_element.tagName();
                if (sub_name == "path") {
                    QVector<QVector<QPoint>> area;

                    auto content = sub_element.attribute("d");
                    auto path_list = content.split('z');
                    for (auto& path : path_list) {
                        if (path.isEmpty()) {
                            continue;
                        }
                        auto polygon = getPolygon(path);
                        Q_ASSERT(!polygon.isEmpty());

                        area.push_back(polygon);
                        // Add to map
                    }
                }
                sub_node = sub_node.nextSibling();
            }
        }
        node = node.nextSibling();
    }
}
