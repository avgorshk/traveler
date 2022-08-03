#ifndef MAPAREA_H
#define MAPAREA_H

#include <QDomElement>
#include <QPolygon>
#include <QSize>
#include <QVector>

class MapArea {
public:
    MapArea(QDomElement element)
        : m_element(element),
          m_visited(false),
          m_checked(false) {}

    void addPolygon(QPolygonF polygon) {
        m_area.push_back(polygon);
    }

    const QVector<QPolygonF>& getPolygonList() const {
        return m_area;
    }

    const QString& getName() const {
        return m_name;
    }

    void setVisited(bool visited) {
        m_visited = visited;
        if (m_visited) {
            m_element.setAttribute("fill", "#90ee90");
        } else {
            m_element.removeAttribute("fill");
        }
    }

    bool isVisited() const {
        return m_visited;
    }

    void setChecked(bool checked) {
        m_checked = checked;
    }

    bool isChecked() const {
        return m_checked;
    }

private:
    QVector<QPolygonF> m_area;
    QString m_name;
    QDomElement m_element;
    bool m_visited;
    bool m_checked;
};

#endif // MAPAREA_H
