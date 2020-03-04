#ifndef GDRIVEABOUT_H
#define GDRIVEABOUT_H
#include <QObject>
#include <QNetworkReply> //for network reply error
#include "gdriveaboutresource.h"

class QOAuth2AuthorizationCodeFlow;
namespace GDrive {
class GDriveAbout : public QObject
{
    Q_OBJECT
public:
    enum AboutArg{
        DISPLAYNAME = 0x01,
        EMAILADDRESS = 0x02,
        PERMISSIONID = 0x04
    };
    Q_DECLARE_FLAGS(AboutArgs,AboutArg)
    /// Constructor,the parent object use to send request
    explicit GDriveAbout(QOAuth2AuthorizationCodeFlow *parent,AboutArgs args);
    /// use direct url to send request
    explicit GDriveAbout(QOAuth2AuthorizationCodeFlow *parent,const QString &aboutUrls);
    virtual ~GDriveAbout() final;
    /// return resource, if data not received, return null if parse error
    GDriveAboutResource getResource();

signals:
    void received(bool success);

private:
    /// save reply data
    QByteArray m_data;
    /// Turn AboutArgs to request Url
    QUrl aboutArgToUrl(const AboutArgs& args) const;

private slots:
    /// process network reply finish
    void onReplyFinished();
    /// process network reply error
    void onReplyError(QNetworkReply::NetworkError);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(GDriveAbout::AboutArgs)
}
#endif // GDRIVEABOUT_H

/*!
 * \class GDriveAbout 負責發送並保存使用者的About資訊
 * 發送的reply由QOAuth2AuthorizationCodeFlow持有，
 * 因此當QOAuth2AuthorizationCodeFlow物件消失會產生錯誤
 *
 * GDriveAbout 負責發送about請求，接收回傳、解析資料並保存
 *
 * GDriveAbout 是 async 的，收到received信號且內容true時才可以正確取出資料
 */
