#include "gdrivefiletask.h"
#include <QOAuth2AuthorizationCodeFlow>
#include <QRandomGenerator>

GDrive::GDriveFileTask::GDriveFileTask(QOAuth2AuthorizationCodeFlow *parent)
    :QObject (parent),mp_google(parent),m_currentReply(nullptr),m_errStr(""),m_isFailed(false),m_isComplete(false)
{

}

GDrive::GDriveFileTask::~GDriveFileTask()
{
    if(m_currentReply){
        m_currentReply->deleteLater();
    }
}

QString GDrive::GDriveFileTask::errorString() const
{
    return m_errStr;
}

bool GDrive::GDriveFileTask::isComplete() const
{
    return m_isComplete;
}

bool GDrive::GDriveFileTask::isFailed() const
{
    return m_isFailed;
}

int GDrive::GDriveFileTask::getExpBackoffSleepTime(int collisions, int slottime, int maxretry) const
{
    if(collisions > maxretry){ //! if collisions more than maxRetry, stop
        return -1;
    }
    //! sleep time = SLOT_TIME * Random{0...(2^i-1)}
    int p = int(powl(2,collisions)) -1;
    int r = QRandomGenerator::global()->bounded(p);
    return int(slottime * r);
}

void GDrive::GDriveFileTask::taskSucceeded()
{
    m_isFailed = false;
    m_isComplete = true;
    emit finished();
}

void GDrive::GDriveFileTask::taskFailed()
{
    m_isFailed = true;
    m_isComplete = true;
    emit finished();
}

void GDrive::GDriveFileTask::taskFinish(bool success)
{
    m_isFailed = !success;
    m_isComplete = true;
    emit finished();
}
