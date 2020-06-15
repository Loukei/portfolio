#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = nullptr);
    ~UpdateDialog() override;

    /* Required Parameter */
    QString getFileID() const;
    void setFileID(const QString& fileID = QString());

    QString getFilePath() const;
    void setFilePath(const QString& filePath  = QStringLiteral("C:/"));

    int getUploadType() const;
    QString getUploadTypeStr() const;

    /* Optional Parameter Getter & Setter */
    QString getAddParents() const;
    bool getEnforceSingleParent() const;
    bool getKeepRevisionForever() const;
    QString getOcrLanguage() const;
    QString getRemoveParents() const;
    bool getSupportsAllDrives() const;
    bool getUseContentAsIndexableText() const;

private slots:
    void on_pushButton_FilePath_clicked();

private:
    Ui::UpdateDialog *ui;
};

#endif // UPDATEDIALOG_H
