#ifndef GDRIVEFILEDOWNLOADDIALOG_H
#define GDRIVEFILEDOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class GDriveFileDownloadDialog;
}

class GDriveFileDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GDriveFileDownloadDialog(QWidget *parent = nullptr,
                                      const QString &downloadPath = "",
                                      const QString &fileID = "");
    ~GDriveFileDownloadDialog() override;
    /// return Download path form lineEdit_DownloadPath
    QString getDownloadFilePath();
    /// return fileID form lineEdit_FileID
    QString getFileId();
signals:
    /// return download path and fileId to mainWindow
    void downloadParam(QString downloadFilePath,QString fileId);
public slots:
    /// override accept(), emit downloadParam(...)
    virtual void accept() override;
private slots:
    /// open file dialog to choose download path
    void on_pushButton_DownloadPath_clicked();

private:
    Ui::GDriveFileDownloadDialog *ui;
};

#endif // GDRIVEFILEDOWNLOADDIALOG_H
