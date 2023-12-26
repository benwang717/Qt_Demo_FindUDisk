#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_UDisk = new UDisk();
    qApp->installNativeEventFilter(m_UDisk);
    setListWidget();
    connect(m_UDisk, &UDisk::sigUDiskCome, [=](QString uDiskName) {
        qDebug() << "uDiskName:" << uDiskName;
        setListWidget();
    });
    connect(m_UDisk, &UDisk::sigUDiskRemove, [=]() {
        qDebug() << "U Disk Remove!";
        setListWidget();
    });
}

MainWindow::~MainWindow()
{
    if (m_UDisk) {
        delete m_UDisk;
        m_UDisk = nullptr;
    }
    delete ui;
}

void MainWindow::setListWidget()
{
    QVector<QStorageInfo> uDiskList = m_UDisk->ScanHD();

    ui->treeWidget->clear();
    if (uDiskList.size() < 1) {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("请插入U盘"));
        item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
        ui->treeWidget->addTopLevelItem(item);
        ui->treeWidget->setEnabled(false);
        ui->treeWidget->setRootIsDecorated(false);
    }
    else {
        foreach(const QStorageInfo & storage, uDiskList) {
            QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(storage.rootPath().replace("/", " ") + storage.displayName()));
            item->setTextAlignment(0,Qt::AlignLeft | Qt::AlignVCenter);
            ui->treeWidget->addTopLevelItem(item);
            QDir dir(item->text(0).left(2));
            QStringList nameFilters;
            nameFilters << "record*.db";
            QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
            for (QString val : files) {
                QTreeWidgetItem* itemChild = new QTreeWidgetItem(QStringList(val));
                itemChild->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
                itemChild->setData(0, Qt::UserRole, dir.path());
                qDebug() << "测试显示12_____________:" << dir.path();
                item->addChild(itemChild);
            }
        }
        ui->treeWidget->setEnabled(true);
        ui->treeWidget->setRootIsDecorated(true);
    }


    QDir desktopDir = QDir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    QStringList nameFilters;
    nameFilters << "record*.db";
    QStringList fileLists = desktopDir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);

    if (fileLists.size() > 0) {

        //qDebug() << "测试显示1_________:" << fileLists.size() << ",测试显示2___________:" << desktopDir.path();
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("desktop/:"));
        item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
        ui->treeWidget->addTopLevelItem(item);
        QDir dir(item->text(0).left(2));
        for (const QString& fileName : fileLists) {

            QString filePath = desktopDir.absoluteFilePath(fileName);
            QStorageInfo storageInfo(filePath);
            QTreeWidgetItem* itemChild = new QTreeWidgetItem(QStringList(fileName));
            itemChild->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            itemChild->setData(0, Qt::UserRole, desktopDir.path());
            //ui->treeWidget->addTopLevelItem(itemChild);
            item->addChild(itemChild);
            //qDebug() << "测试显示9(____________:" << storageInfo;
            uDiskList.append(storageInfo);

        }
        ui->treeWidget->setEnabled(true);
        ui->treeWidget->setRootIsDecorated(true);
    }
    else {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList("选择指定路径"));
        item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
        ui->treeWidget->addTopLevelItem(item);
        QDir dir(item->text(0).left(2));


        ui->treeWidget->setEnabled(true);
        ui->treeWidget->setRootIsDecorated(true);
    }
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    qDebug()<< "test_"<<column;
    if ("选择指定路径" == item->text(0)) {
        //默认U盘和桌面两个路径都不存在指定类型文件的时候执行以下逻辑
        //QString defaultPath = QDir::homePath(); // 默认位置为桌面
        QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QString filter = "Record Files (*.db)"; // 指定文件类型为record*.db

        QString filePath = QFileDialog::getOpenFileName(nullptr, "选择文件", defaultPath, filter);

        if (!filePath.isEmpty()) {
            qDebug() << "选择的文件路径：" << filePath;
            // 在这里可以对选择的文件进行处理
            emit UDiskSelected(filePath);//F:
            //this->hide();
        }
    }
    else {
        if (item->data(0, Qt::UserRole).toString() == "")return;
        QString path = item->data(0, Qt::UserRole).toString() + "/" + item->text(0);
        //qDebug() << "测试显示路径10__第一部分____:" << item->data(0, Qt::UserRole).toString()<<",第二部分:"<< item->text(0);
        qDebug() << "文件路径：" << path;
        emit UDiskSelected(path);//F:
        //this->hide();
    }
}

