#ifndef UDISK_H
#define UDISK_H

#include <QObject>
#include <QWidget>
#include <QAbstractNativeEventFilter>
#include <QStorageInfo>
#include <windows.h>
#include <dbt.h>


#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

/**
 * @brief U盘/磁盘扫描监听,读取指定类型文件,返回文件路径字符串
 * @author 王宾
 * @date 创建时间 2023-12-26
 * @version 1.0
 * @property ：功能接口, 扫描桌面和U盘两个默认路径是否存在指定类型文件,如果不存在可以选择指定路径,在选择路径之后返回该文件所在路径为QString类型
 */


class UDisk : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit UDisk(QWidget *parent = nullptr);
    ~UDisk();

signals:
    void sigUDiskCome(QString uDiskName);
    void sigUDiskRemove();
protected:
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result);
private:
    char FirstDriveFromMask(ULONG unitmask);
public:
    QVector<QStorageInfo> ScanHD();
};

#endif // UDISK_H
