#ifndef GDRIVEFILESEARCH_H
#define GDRIVEFILESEARCH_H
#include "gdrivefiletask.h"
#include "gdrivefilelist.h"
#include <QNetworkReply>

class QOAuth2AuthorizationCodeFlow;
namespace GDrive {
/*!
 * \class GDriveFileSearch
 * \brief The GDriveFileSearch class
 *
 * ## Reference
 * [Search for files and folders]:https://developers.google.com/drive/api/v3/search-files
 * [Files: list]:https://developers.google.com/drive/api/v3/reference/files/list
 */
class GDriveFileSearch : public GDriveFileTask
{
    Q_OBJECT
public:
    /// constructor with 4 parameter, deprecated.
    explicit GDriveFileSearch(QOAuth2AuthorizationCodeFlow *parent,
                              const QString &q,
                              const QString &spaces,
                              const QString &fields,
                              const QString &pageToken);
    /// constructor with 2 parameter
    explicit GDriveFileSearch(QOAuth2AuthorizationCodeFlow *parent,
                              const QString &q,
                              const QString &pageToken);
    /// destructor
    ~GDriveFileSearch() override;
    /// get search result to GDriveFileList data format
    GDriveFileList getFileList() const;
    /// get search result to JSON string data format
    QByteArray getReplyString() const;

private:
    void request_Search(const QString &q,
                        const QString &spaces,
                        const QString &fields,
                        const QString &pageToken);
private slots:
    void on_Search_ReplyFinished();
    void on_Search_ReplyError(QNetworkReply::NetworkError);
private:
    QByteArray m_replyData;
};
}

#endif // GDRIVEFILESEARCH_H
