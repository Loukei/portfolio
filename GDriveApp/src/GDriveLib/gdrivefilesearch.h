#ifndef GDRIVEFILESEARCH_H
#define GDRIVEFILESEARCH_H
#include "gdrivefiletask.h"
#include "gdrivefilelist.h"
#include <QNetworkReply>

class QOAuth2AuthorizationCodeFlow;
namespace GDrive {
class GDriveFileSearch : public GDriveFileTask
{
    Q_OBJECT
public:
    explicit GDriveFileSearch(QOAuth2AuthorizationCodeFlow *parent,
                              const QString &q,
                              const QString &spaces,
                              const QString &fields,
                              const QString &pageToken);
    ~GDriveFileSearch() override;
    GDriveFileList getFileList() const;
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
    QByteArray m_replyData = QByteArray();
};
}

#endif // GDRIVEFILESEARCH_H
