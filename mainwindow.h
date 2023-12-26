#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include "udisk.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;
    UDisk* m_UDisk = nullptr;

    void setListWidget();
signals:
    void UDiskSelected(QString rootPath);
};
#endif // MAINWINDOW_H
