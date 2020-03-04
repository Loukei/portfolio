#ifndef UIFILEDOWNLOADDIALOG_H
#define UIFILEDOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class UIDownloadDialog;
}

class UIDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UIDownloadDialog(QWidget *parent = nullptr,
                                      const QString &downloadPath = "",
                                      const QString &fileID = "");
    ~UIDownloadDialog() override;
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
    Ui::UIDownloadDialog *ui;
};

#endif // GDRIVEFILEDOWNLOADDIALOG_H
