#include "udisk.h"

#include <QDebug>
#include <QDir>
#include <QStorageInfo>
#include <QFileInfo>
#include <QFileInfoList>

UDisk::UDisk(QWidget *parent)
    : QWidget{parent}
{

}

UDisk::~UDisk()
{
}

char UDisk::FirstDriveFromMask(ULONG unitmask)
{
    char i;
    for (i = 0; i < 26; ++i) {
        if (unitmask & 0x1) {
            break;
        }
        unitmask = unitmask >> 1;
    }
    return (i + 'A');
}
bool UDisk::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    int msgType = msg->message;
    if (msgType == WM_DEVICECHANGE) {
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        switch (msg->wParam) {
        case DBT_DEVICEARRIVAL://已插入设备，现在可以使用
            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv->dbcv_flags == 0) {
                    QString USBDisk = QString(this->FirstDriveFromMask(lpdbv->dbcv_unitmask));
                    emit sigUDiskCome(USBDisk);
                }
            }
            break;
        case DBT_DEVICEREMOVECOMPLETE://已移除设备
            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv->dbcv_flags == 0) {
                    emit sigUDiskRemove();
                }
            }
            break;
        case DBT_DEVNODES_CHANGED:
            break;
        default:
            break;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

QVector<QStorageInfo> UDisk::ScanHD()
{
    qDebug() << "U盘扫描";
    QVector<QStorageInfo> udisk_lists;
    QFileInfoList list = QDir::drives();  //获取当前系统的盘符

    foreach(const QStorageInfo& storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady()) {
            if (!storage.isReadOnly()) {
                QString diskPath = storage.rootPath();
                WCHAR* RootPathName = (WCHAR*)QString(diskPath).utf16();
                if (GetDriveType(RootPathName) == DRIVE_REMOVABLE)
                {
                    if (GetVolumeInformation(RootPathName, 0, 0, 0, 0, 0, 0, 0)) //判断驱动是否准备就绪
                    {
                        qDebug() << "U盘准备就绪!" + diskPath;
                                        //qDebug() << "测试显示3______________:" << storage;
                                        udisk_lists.push_back(storage);
                    }
                    else
                    {
                        qDebug() << "U盘未就绪!" + diskPath;
                    }
                }
            }
        }
    }
    return udisk_lists;
}
