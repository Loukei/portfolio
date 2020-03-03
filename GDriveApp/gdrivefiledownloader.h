#ifndef GDRIVEFILEDOWNLOADER_H
#define GDRIVEFILEDOWNLOADER_H
#include <QObject>
#include "gdrivefiletask.h"
#include <QNetworkReply>

class QFile;
namespace GDrive {
class GDriveFileDownloader : public GDriveFileTask
{
    Q_OBJECT
public:
    /// Download file form fileID, write file when finished().
    /// GDriveFileDownloader doesnt take ownership of file
    explicit GDriveFileDownloader(QOAuth2AuthorizationCodeFlow *parent,
                                  const QString &fileId,
                                  const QString &fields,
                                  QSharedPointer<QFile> file);
    ~GDriveFileDownloader() override;
private:
    /// pointer to Shared QFile ready to write,DELETE by owner
    QSharedPointer<QFile> mp_file;
private:
    /// send download request
    void request_Download(const QString &fileId,const QString &fields);
    /// Write data to mp_file;
    bool writeFile(QNetworkReply *reply);
    /// Parse Error message form reply
    QString getErrorMessage(QNetworkReply *reply);
private slots:
    void on_Download_ReplyFinished();
    void on_Download_ReplyError(QNetworkReply::NetworkError);
};
}
/*! \example
 *  QSharedPointer<QFile> writeTo =
 * QSharedPointer<QFile>(new QFile(path,this),&QFile::deleteLater);
 *  GDriveFileDownloader(m_google,"[fileID]","[fields]",QSharedPointer<QFile>(&writeTo))
  */
#endif // GDRIVEFILEDOWNLOADER_H
