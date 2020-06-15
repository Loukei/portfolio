#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class SearchDialog;
}

/*!
 * \class SearchDialog
 * \brief The SearchDialog class is use to search drive file and show search result on QTreeView
 *
 * - 用來提供搜尋的介面
 * - 按下search按鈕後會發送`SearchDialog::query()`給mainwindow
 * - mainwindow接收後會處理搜尋請求，產生`GDriveFileSearch`物件
 * - `GDriveFileSearch`物件收到搜尋完成的內容後可以透過connect
 * `SearchDialog::onFileSearch_finished()`信號來接收搜尋完成的內容
 * - 回傳的json格式字串透過QJsonModel處理，用QTreeView來顯示
 *
 * ## Credit
 * - [QJsonModel](https://github.com/dridk/QJsonModel)
 */
class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr);
    ~SearchDialog();

    QString getCorpora() const;
    QString getDriveId() const;
    QString getFields() const;
    bool getIncludeItemsFromAllDrives() const;
    QString getOrderBy() const;
    int getPageSize() const;
    QString getPageToken() const;
    QString getQ() const;
    QString getSpaces() const;
    bool getSupportsAllDrives() const;

signals:
    /// send query parameter to mainwindow
    void query(const QString &corpora,
               const QString &driveId,
               const QString &fields,
               const QString &orderBy,
               int pageSize,
               const QString &pageToken,
               const QString &q,
               const QString &spaces);
private:
    Ui::SearchDialog *ui;
};
#endif // SEARCHDIALOG_H
