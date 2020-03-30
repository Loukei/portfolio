#ifndef GDRIVEFILEGET_H
#define GDRIVEFILEGET_H

#include <QObject>
#include <QNetworkReply>
#include "gdrivefiletask.h"

class QOAuth2AuthorizationCodeFlow;
namespace GDrive {
/*!
 * \class GDriveFileGet
 * \brief The GDriveFileGet class is implement Google Drive Api `Files: get` method
 *
 * - `GDriveFileGet` 用來實作`Files: get`方法，取得目標檔案的matadata
 *
 * ## Reference 參考資料
 * - [Files: get](https://developers.google.com/drive/api/v3/reference/files/get)
 */
class GDriveFileGet : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileGet(QOAuth2AuthorizationCodeFlow *parent,
                           const QString &fileId,
                           const QString &fields);
    /// destructor
    ~GDriveFileGet();
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;

private:
    void request_FilesGet(const QString &fileId,const QString &fields);
private slots:
    void on_Request_FilesGet_ReplyFinished();
    void on_Request_FilesGet_ReplyError(QNetworkReply::NetworkError);

private:
    /// save network reply json,usually file matadata
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILEGET_H
