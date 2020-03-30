#ifndef GDRIVEFILES_H
#define GDRIVEFILES_H

#include <QObject>
#include "gdrivefiletask.h" // upload new file

namespace GDrive {
/*!
 * \class GDriveFiles
 * \brief The GDriveFiles class is design for handle all file resource management(Unused)
 */
class GDriveFiles : public QObject
{
    Q_OBJECT
public:
    explicit GDriveFiles(QOAuth2AuthorizationCodeFlow *parent);
    virtual ~GDriveFiles();

    /// upload new file
    void create();
    /// upload to update file
    void update();
    /// delete file
    void remove();
private:
    QOAuth2AuthorizationCodeFlow *m_google;
};
}
#endif // GDRIVEFILES_H

/// class
