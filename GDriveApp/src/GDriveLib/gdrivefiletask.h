#ifndef GDRIVEFILETASK_H
#define GDRIVEFILETASK_H

#include <QObject>

class QOAuth2AuthorizationCodeFlow;
namespace GDrive {
/*!
 * \class GDriveFileTask
 * \brief A base class to handle all the Google Drive Api `File` method
 *
 * - 處理`File`類型的基底類別
 * - 使用`GDriveFileTask::finished`信號表示操作完成或失敗
 * - `errorString()`回傳錯誤的字串
 * - `isComplete()`表示任務完成與否，初始值為false
 * - `isFailed()`表示任務是否失敗，預設為false
 * - `getExpBackoffSleepTime()`用於計算指數退避所需時間
 *
 * ## Reference 參考資料
 * [Implementing exponential backoff]:https://cloud.google.com/iot/docs/how-tos/exponential-backoff?hl=zh-TW
 */
class GDriveFileTask : public QObject
{
    Q_OBJECT
public:
    /// constructor
    explicit GDriveFileTask(QOAuth2AuthorizationCodeFlow *parent);
    /// destructor
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
