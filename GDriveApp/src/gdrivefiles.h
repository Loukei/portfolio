#ifndef GDRIVEFILES_H
#define GDRIVEFILES_H

#include <QObject>
#include "gdriveuploader.h" // upload new file

namespace GDrive {
class GDriveFiles : public QObject
{
    Q_OBJECT
public:
    explicit GDriveFiles(QOAuth2AuthorizationCodeFlow *parent);
    virtual ~GDriveFiles();

    /// upload new file
    GDriveUploader* create();
    /// upload to update file
    void update();
    /// delete file
    void remove();

signals:
private:
    QOAuth2AuthorizationCodeFlow *m_google;
};
}
#endif // GDRIVEFILES_H

/// class
