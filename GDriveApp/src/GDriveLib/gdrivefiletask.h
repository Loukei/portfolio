#ifndef GDRIVEFILETASK_H
#define GDRIVEFILETASK_H

#include <QObject>

class QOAuth2AuthorizationCodeFlow;
namespace GDrive {
class GDriveFileTask : public QObject
{
    Q_OBJECT
public:
    explicit GDriveFileTask(QOAuth2AuthorizationCodeFlow *parent);
    virtual ~GDriveFileTask();

    /// return human readable string after receive(false) emit
    QString errorString() const;
    /// return true if task complete
    bool isComplete() const;
    /// return true if error occured
    bool isFailed() const;
signals:
    /// emit when error occured or task complete
    void finished();
protected:
    /// An instance to manage Oauth request, and Token resource, no need to delete
    QOAuth2AuthorizationCodeFlow * const mp_google;
    /// Human readable error string
    QString m_errStr;
    /// Does task complete and not failed?
    bool m_isFailed = false;
    /// Does task complete
    bool m_isComplete = false;
protected:
    /// calculate exponential backoff sleep time,return -1 if collisions > maxretry
    int getExpBackoffSleepTime(int collisions,int slottime,int maxretry) const;
};
}
#endif // GDRIVEFILETASK_H
