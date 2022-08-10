#ifndef MAPPOINT_H
#define MAPPOINT_H

#include <QDomElement>
#include <QPointF>

class MapPoint {
public:
    MapPoint(
        QDomDocument doc, QDomElement root, QDomElement element,
        QPointF point, const QString& name)
            : m_doc(doc), m_root(root), m_element(element),
              m_point(point), m_name(name), m_checked(false) {}

    const QPointF& getPoint() const {
        return m_point;
    }

    bool isChecked() const {
        return m_checked;
    }

    void setChecked(bool checked) {
        m_checked = checked;
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

    void remove() {
        m_root.removeChild(m_element);
    }

    bool operator==(const MapPoint& right) const {
        return m_point == right.m_point;
    }

private:
    QDomDocument m_doc;
    QDomElement m_root;
    QDomElement m_element;

    QPointF m_point;
    QString m_name;
    bool m_checked;
};

#endif // MAPPOINT_H
