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
    explicit DownloadDialog(QWidget *parent = nullptr);
    ~DownloadDialog() override;

    QString getDownloadFilePath() const;
    void setDownloadFilePath(const QString &downloadPath = QStringLiteral("C:/"));

    QString getFileId() const;
    void setFileId(const QString &fileID = QString());

    bool getAcknowledgeAbuse() const;
    bool getSupportsAllDrives() const;

// TODO: check input
//public slots:
//    virtual void accept() override;

private slots:
    /// open file dialog to choose download path
    void on_pushButton_DownloadPath_clicked();

private:
    Ui::DownloadDialog *ui;
    static const QString instructions_String;
};

#endif // GDRIVEFILEDOWNLOADDIALOG_H
