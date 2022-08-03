#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>

#include "mapview.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected slots:
    void regionChecked(MapArea* area);
    void regionUnchecked();
    void regionSaved();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    MapView* m_view;
    MapArea* m_currentRegion;

    QLineEdit* m_name;
    QCheckBox* m_flag;
    QPushButton* m_save;
};

#endif // MAINWINDOW_H
