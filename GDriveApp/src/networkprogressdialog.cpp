#include "networkprogressdialog.h"

NetworkProgressDialog::NetworkProgressDialog(const QString &labelText, QWidget *parent)
    : QProgressDialog(labelText,tr("cancled"),0,100,parent)
{

}

void NetworkProgressDialog::networkProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    setMaximum(bytesTotal);
    setValue(bytesReceived);
}
