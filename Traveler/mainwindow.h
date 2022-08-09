#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>

#include "mapview.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected slots:
    void regionChecked(MapRegion* region);
    void regionUnchecked();

    void saved();

    void pointAdded();
    void pointChecked(MapPoint* point);
    void pointUnchecked();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setPanels(const QString& label, const QString& text, bool flag);
    void resetPanels();

private:
    MapView* m_view;
    MapRegion* m_currentRegion;
    MapPoint* m_currentPoint;

    QLineEdit* m_name;
    QCheckBox* m_flag;
    QPushButton* m_save;
    QLabel* m_label;
};

#endif // MAINWINDOW_H
