#ifndef MAPREGION_H
#define MAPREGION_H

#include <QDomElement>
#include <QPolygon>
#include <QSize>
#include <QVector>

class MapRegion {
public:
    MapRegion(
        QDomDocument doc, QDomElement element,
        const QString& name, bool visited)
            : m_doc(doc), m_element(element), m_name(name),
              m_visited(visited), m_checked(false) {}

    void addPolygon(QPolygonF polygon) {
        m_region.push_back(polygon);
    }

    const QVector<QPolygonF>& getPolygonList() const {
        return m_region;
    }

    const QString& getName() const {
        return m_name;
    }

    void setName(const QString& name) {
        m_name = name;

        if (m_element.hasChildNodes()) {
            QDomNode title_node = m_element.firstChild();
            QDomElement title_element = title_node.toElement();

            QDomElement new_title_element = m_doc.createElement("title");
            QDomText title_text = m_doc.createTextNode(m_name);
            new_title_element.appendChild(title_text);
            m_element.replaceChild(new_title_element, title_element);
        } else {
            QDomElement title_element = m_doc.createElement("title");
            QDomText title_text = m_doc.createTextNode(m_name);
            title_element.appendChild(title_text);
            m_element.appendChild(title_element);
        }
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
    QDomDocument m_doc;
    QDomElement m_element;

    QVector<QPolygonF> m_region;
    QString m_name;
    bool m_visited;
    bool m_checked;
};

#endif // MAPREGION_H
