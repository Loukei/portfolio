#ifndef GDRIVEABOUTRESOURCE_H
#define GDRIVEABOUTRESOURCE_H
#include <QJsonDocument>
namespace GDrive {
class GDriveAboutResource
{
public:
    /// construct by QBytearray data
    explicit GDriveAboutResource(const QByteArray &data);
    ~GDriveAboutResource();
    /// return User dispalyName,if it's not String, a null QString will be returned.
    QString displayName() const;
    /// User emailAddress,if it's not String, a null QString will be returned.
    QString emailAddress() const;
    /// User permissionId,if it's not String, a null QString will be returned.
    QString permissionId() const;
    /// returns true if this document is null.
    bool isNull();
    /// return current error string
    QString errorString() const;
private:
    /// save data form constructer resource
    QJsonDocument m_data;
    /// save parse error use human readable string
    QString m_errorString;
};
}

#endif // GDRIVEABOUTRESOURCE_H

/* GDriveAboutResource 負責解析封包回傳的json檔成為可讀取的資料
 * 如果取值得結果是空值，請嘗試取得errorString()獲得錯誤
 * 因為有RVO的關係所以不用做copy construct
*/
