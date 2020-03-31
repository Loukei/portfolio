#ifndef GDRIVEFILERESOURCELIST_H
#define GDRIVEFILERESOURCELIST_H
#include <QJsonArray>
#include <QList>
#include "GDriveLib/gdrivefileresource.h"

namespace GDrive{
/*!
 * \class GDriveFileResourceList
 * \brief A data class which store the result form `Files: list`.
 *
 * - 一個儲存`Files: list`方法回傳資料的型別
 * - 這個類別用來儲存"搜尋檔案"的命令回傳的結果，原始的內容以json顯示。
 * - 但此處會將其轉換成get函數，省去操作者手動轉換json的操作。
 * - File list 沒有{部份回傳}的需要，因此可以儲存全部的value，在建構子就可以解析資料
 *
 * ## Reference 參考資料
 * - [Search for files and folders](https://developers.google.com/drive/api/v3/search-files)
 * - [Files: list](https://developers.google.com/drive/api/v3/reference/files/list)
 */
class GDriveFileResourceList
{
public:
    /// constructor by QJsonDocument
    explicit GDriveFileResourceList(const QJsonDocument &doc);
    /// constructor empty
    explicit GDriveFileResourceList();
    /// destructor
    ~GDriveFileResourceList();
    /// return m_kind
    QString kind() const;
    /// return m_nextPageToken
    QString nextPageToken() const;
    /// return m_incompleteSearch
    bool incompleteSearch() const;
    /// return m_filelist
    QJsonArray files() const;

private:
    /// parse QJsonDocument form constructor,save to data member
    void parseDocument(const QJsonDocument &doc);

private:
    /// Identifies what kind of resource this is. Value: the fixed string "drive#fileList".
    QString m_kind = QString();
    /// The page token for the next page of files.
    /// This will be absent if the end of the files list has been reached.
    /// If the token is rejected for any reason, it should be discarded, and pagination should be restarted from the first page of results.
    QString m_nextPageToken = QString();
    /// Whether the search process was incomplete.
    /// If true, then some search results may be missing, since all documents were not searched.
    /// This may occur when searching multiple drives with the "allDrives" corpora, but all corpora could not be searched.
    /// When this happens, it is suggested that clients narrow their query by choosing a different corpus such as "user" or "drive".
    bool m_incompleteSearch = false;
    /// The list of files.
    /// If nextPageToken is populated, then this list may be incomplete and an additional page of results should be fetched.
    QJsonArray m_filelist = QJsonArray();
};
}

#endif // GDRIVEFILERESOURCELIST_H
