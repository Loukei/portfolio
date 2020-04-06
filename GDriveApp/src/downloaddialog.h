#ifndef UIFILEDOWNLOADDIALOG_H
#define UIFILEDOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class DownloadDialog;
}
/*!
 * \class DownloadDialog
 * \brief The DownloadDialog class provide download parameter to mainwindow
 *
 * - 選擇下載路徑的對話框，並在按下確認鍵後提供mainwindow取得下載所需參數
 */
class DownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadDialog(QWidget *parent = nullptr,
                            const QString &caption = QString(),
                            const QString &downloadPath = QString(),
                            const QString &fileID = QString());
    ~DownloadDialog() override;
    /// return Download path form lineEdit_DownloadPath
    QString getDownloadFilePath() const;
    /// return fileID form lineEdit_FileID
    QString getFileId() const;

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
    Ui::DownloadDialog *ui;
};

#endif // GDRIVEFILEDOWNLOADDIALOG_H
