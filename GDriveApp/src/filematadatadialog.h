#ifndef FILEMATADATADIALOG_H
#define FILEMATADATADIALOG_H

#include <QDialog>

class QJsonModel;
namespace Ui {
class FileMataDataDialog;
}
/*!
 * \class FileMataDataDialog
 * \brief The FileMataDataDialog class is UI to get file matadata
 */
class FileMataDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileMataDataDialog(QWidget *parent = nullptr);
    ~FileMataDataDialog();

    QString getFileID() const;
    void setFileID(const QString &fileID = QString());

    QString getFields() const;
    void setFields(const QString &fields = QString());

    bool getSupportsAllDrives() const;
    void setSupportsAllDrives(bool supportsAllDrives = false);

private:
    Ui::FileMataDataDialog *ui;
};

#endif // FILEMATADATADIALOG_H
