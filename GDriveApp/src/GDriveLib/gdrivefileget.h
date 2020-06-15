#ifndef GDRIVEFILEGET_H
#define GDRIVEFILEGET_H

#include <QObject>
#include <QNetworkReply>
#include "gdrivefiletask.h"

QT_BEGIN_NAMESPACE
class QOAuth2AuthorizationCodeFlow;
QT_END_NAMESPACE

namespace GDrive {
/*!
 * \class GDriveFileGet
 * \brief The GDriveFileGet class is implement Google Drive Api `Files: get` method
 *
 * - `GDriveFileGet` 用來實作`Files: get`方法，取得目標檔案的matadata
 * - `Files: get`方法不使用acknowledgeAbuse參數，只有download才使用
 *
 * ## Reference 參考資料
 * - [Files: get](https://developers.google.com/drive/api/v3/reference/files/get)
 */
class GDriveFileGet : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileGet(const QString &fileId,
                           const QString &fields,
                           QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileGet(const QString &fileId,
                           const QUrlQuery &args,
                           QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileGet();
    /// return File resource to JSON string data format
    QByteArray getReplyString() const;
    /// a convience function to pass url parameter.
    /// This is NOT INCLUDE ALL ACTUAL PARAMETERS
    /// see buildUrl(QUrlQuery args)
    static QUrlQuery buildUrlArgs(const QString &fields = QString(),
                                  bool supportsAllDrives = false);
private:
    void request_FilesGet(const QUrl &url);
    QUrl buildUrl(const QString &fileId,const QString &key,const QString &fields) const;
    QUrl buildUrl(const QString &fileId,const QString &key,QUrlQuery args) const;

private slots:
    void on_Request_FilesGet_ReplyFinished();
    void on_Request_FilesGet_ReplyError(QNetworkReply::NetworkError);

private:
    /// save network reply json,usually file matadata
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILEGET_H
