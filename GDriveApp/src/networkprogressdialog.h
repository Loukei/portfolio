#ifndef NETWORKPROGRESSDIALOG_H
#define NETWORKPROGRESSDIALOG_H

#include <QProgressDialog>

class NetworkProgressDialog : public QProgressDialog
{
    Q_OBJECT
public:
    NetworkProgressDialog(const QString &labelText, QWidget *parent = nullptr);
public slots:
    void networkProgress(qint64 bytesReceived, qint64 bytesTotal);
};

#endif // NETWORKPROGRESSDIALOG_H
