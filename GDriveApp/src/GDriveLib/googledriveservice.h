#ifndef TESTOAUTH_H
#define TESTOAUTH_H

#include <QObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <QUrlQuery>
#include "gdriveaboutresource.h"
#include "gdrivefileresource.h"
#include "gdrivefileresourcelist.h"
#include "gdriveerrorformat.h"

#include "gdriveabout.h"
#include "gdrivefiledownloader.h"
#include "gdrivefilesimplecreate.h"
#include "gdrivefilemultipartcreate.h"
#include "gdrivefileresumablecreate.h"
#include "gdrivefilesimpleupdate.h"
#include "gdrivefilemultipartupdate.h"
#include "gdrivefileresumableupdate.h"
#include "gdrivefilesearch.h"
#include "gdrivefileget.h"

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

/*!
 * \namespace GDrive
 * \brief The GDrive namespace provide all the Google Drive Api library
 */
namespace GDrive {
/*!
 * \class GDriveService
 * \brief The main class to provide Google Drive serviece
 *
 * - 提供Google Drive Api的服務
 * - Google Drive Api採用資源導向操作，每個method都是對資源的操作
 * - 除了登入帳號外，其他method都使用factory pattern操作。
 * - 每執行一項操作都會回傳一個以`m_google`為parent的物件，我稱之為Task。
 * Task在建立時就會立刻執行，以建構子的參數處理對應的請求，並在完成時發出信號通知
 * - Google 的OAuth操作比起標準多了一些，想要修改需要使用`QAbstractOAuth::ModifyParametersFunction`這項設置，
 * Qt提供有限度的參數修正，例如請求Refersh Token、更新access token等
 * - QAbstractOAuth::Status 用來表達任證的狀態，但是Qt設計上並未提供登出的選擇，
 * 所以`QAbstractOAuth::Status::NotAuthenticated`的狀態只有在grant()之前或是refreshToken()發出請求取得新token之間的空窗期
 * - 想要正確處理logout狀態，使用tokenChanged()
 *
 * ## Note
 * QNetworkreply其中readyRead()信號繼承自QIODevice類，每當有新的數據可以讀取時，都會發射該信號；
 * 每當網絡請求的下載進度更新時都會發射downloadProgress()信號，它用來更新進度條；
 * 每當應答處理結束時，都會發射finished()信號，
 * 該信號與前面程序中QNetworkAccessManager類的finished()信號作用相同，只不過是發送者不同，參數也不同而已。
 *
 * ## Reference
 * - [Introduction to Google Drive API](https://developers.google.com/drive/api/v3/about-sdk)
 * - [QOAuth2AuthorizationCodeFlow class](https://code.woboq.org/qt5/qtnetworkauth/src/oauth/qoauth2authorizationcodeflow.cpp.html)
 * - [enum QAbstractOAuth::Stage](https://doc.qt.io/qt-5/qabstractoauth.html#Stage-enum)
 * - [Refreshing an access token (offline access)](https://developers.google.com/identity/protocols/oauth2/web-server#httprest_7)
 */
class GDriveService : public QObject
{
    Q_OBJECT
public:
    /// constructor of serviece
    explicit GDriveService(const QUrl &authorizationUrl,
                           const QUrl &accessTokenUrl,
                           const QString &clientIdentifier,
                           const QString &clientIdentifierSharedKey,
                           const QString &scope,
                           quint16 port,
                           QObject *parent = nullptr);
    /// destructor
    virtual ~GDriveService() final;
    /// return the expiration time of the current access token.
    QDateTime expirationAt() const;
    /// return m_google token
    QString token() const;
    /// Sets the token used to sign authenticated requests to token.
    void setToken(const QString &token);
    /// Gets the current refresh token.
    QString refreshToken() const;
    /// Sets the new refresh token refreshToken to be used.
    void setRefreshToken(const QString &refreshToken);
    /// Returns the current network access manager used to send the requests to the server during
    /// authentication flows or to make authentication calls.
    QNetworkAccessManager* networkAccessManager() const;

    /// send request to get About resource
    GDrive::GDriveAbout* about(const QString &fields);
    /// simple upload create file
    GDriveFileSimpleCreate* fileSimpleCreate(const QString &filepath);
    GDriveFileSimpleCreate* fileSimpleCreate(const QString &filepath,const QUrlQuery &args);
    /// multipart upload create file
    GDriveFileMultipartCreate* fileMultipartCreate(const QString &filepath);
    GDriveFileMultipartCreate* fileMultipartCreate(const QString &filepath,const QUrlQuery &args);
    /// resumable upload create file
    GDriveFileResumableCreate* fileResumableCreate(const QString &filepath);
    GDriveFileResumableCreate* fileResumableCreate(const QString &filepath,const QUrlQuery &args);
    /// simple upload update file
    GDriveFileSimpleUpdate* fileSimpleUpdate(const QString &filepath,const QString &fileID);
    GDriveFileSimpleUpdate* fileSimpleUpdate(const QString &filepath,const QString &fileID,const QUrlQuery &args);
    /// multipart upload update file
    GDriveFileMultipartUpdate* fileMultipartUpdate(const QString &filepath,const QString &fileID);
    GDriveFileMultipartUpdate* fileMultipartUpdate(const QString &filepath,const QString &fileID,const QUrlQuery &args);
    /// resumable upload update file
    GDriveFileResumableUpdate* fileResumableUpdate(const QString &filepath,const QString& fileID);
    GDriveFileResumableUpdate* fileResumableUpdate(const QString &filepath,const QString& fileID,const QUrlQuery &args);
    /// Gets a file's metadata by ID.
    GDriveFileGet* fileGet(const QString &fileId,const QUrlQuery &args);
    GDriveFileGet* fileGet(const QString &fileId,const QString &fields);
    /// Lists or searches files
    GDriveFileSearch* fileList(const QUrlQuery &args);
    /// Get File download by fileID
    GDriveFileDownloader* fileDownload(const QString &fileId,const QString &fields,const QString &filepath);
    GDriveFileDownloader* fileDownload(const QString &fileId,const QUrlQuery &args,const QString &filepath);

public slots:
    /// Starts the authentication flow as described in The OAuth 2.0 Authorization Framework
    void grant();
    /// Call this function to refresh the token.
    /// Access tokens are not permanent.
    /// After a time specified along with the access token when it was obtained, the access token will become invalid.
    void refreshAccessToken();

signals:
    /// This signal is emitted when the authorization flow finishes successfully.
    void granted();
    /// emit when expiration time changed
    void expirationAtChanged(const QDateTime &expiration);
    /// Signal emitted when the server responds to the request with an error:
    /// error is the name of the error;
    /// errorDescription describes the error and uri is an optional URI containing more information about the error.
    void error(const QString &error, const QString &errorDescription, const QUrl &uri);
    /// emit when refreshToken changed
    void refreshTokenChanged(const QString &refreshToken);
    /// emit when access token changed
    void tokenChanged(const QString &token);

protected:
    /// This function is used to customize the parameters sent to the server during a specified authorization stage.
    /// The number of calls to this function depends on the flow used during the authentication.
    /// see `QAbstractOAuth::ModifyParametersFunction
    QAbstractOAuth::ModifyParametersFunction buildModifyParametersFunction();

private:
    /// Qt Oauth2 Authorization
    QOAuth2AuthorizationCodeFlow *m_google = nullptr;
    /// networkmanager
    QNetworkAccessManager *m_manager = nullptr;
};
}

#endif // TESTOAUTH_H
