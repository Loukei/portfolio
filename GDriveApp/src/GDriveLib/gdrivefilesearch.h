#ifndef GDRIVEFILESEARCH_H
#define GDRIVEFILESEARCH_H
#include "gdrivefiletask.h"
#include "gdrivefileresourcelist.h"
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
class QOAuth2AuthorizationCodeFlow;
QT_END_NAMESPACE

namespace GDrive {
/*!
 * \class GDriveFileSearch
 * \brief GDriveFileSearch provide lists or searches files.
 *
 * - `GDriveFileSearch`實作google drive api的`Files: list`方法
 * - 負責搜尋與列出Drive裡的檔案資訊
 *
 * ## Reference
 * - [Search for files and folders](https://developers.google.com/drive/api/v3/search-files)
 * - [Files: list](https://developers.google.com/drive/api/v3/reference/files/list)
 */
class GDriveFileSearch : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor with 2 parameter
    explicit GDriveFileSearch(const QString &q,
                              const QString &pageToken,
                              QOAuth2AuthorizationCodeFlow *parent);
    explicit GDriveFileSearch(const QUrlQuery &args,
                              QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
    ~GDriveFileSearch() override;
    /// get search result to GDriveFileList data format
    GDriveFileResourceList getFileList() const;
    /// get search result to JSON string data format
    QByteArray getReplyString() const;

    static QUrlQuery buildUrlArgs(const QString &corpora = QString(),
                                  const QString &driveId = QString(),
                                  const QString &fields = QString(),
                                  const bool includeItemsFromAllDrives = false,
                                  const QString &orderBy = QString(),
                                  const int pageSize = 100,
                                  const QString &pageToken = QString(),
                                  const QString &q = QString(),
                                  const QString &spaces = QString(),
                                  const bool supportsAllDrives = false);

private:
    /// send network request to search file
    void request_Search(const QUrl &url);
    /// setup request url
    QUrl buildUrl() const;
    QUrl buildUrl(QUrlQuery args) const;

private slots:
    void onSearchReplyFinished();
    void onSearchReplyError(QNetworkReply::NetworkError);

private:
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILESEARCH_H
