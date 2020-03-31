#ifndef GDRIVEERRORFORMAT_H
#define GDRIVEERRORFORMAT_H
#include <QJsonArray>

QT_BEGIN_NAMESPACE
class QJsonDocument;
QT_END_NAMESPACE

namespace GDrive {
/*!
 * \class GDriveErrorItem
 * \brief The GDriveErrorItem class is the single Object store in "errors" list
 *
 * - 解析error欄位的資料
 */
class GDriveErrorItem{
public:
    /// constructor by QJsonValue
    explicit GDriveErrorItem(const QJsonValue &value);
    /// destructor
    ~GDriveErrorItem();
    /// return m_domain
    QString domain() const;
    /// return m_reason
    QString reason() const;
    /// return m_message
    QString message() const;
    /// return m_locationType
    QString locationType() const;
    /// return m_location
    QString location() const;
    /// return for QJsonValue is empty or not
    bool isEmpty() const;

private:
    /// The scope of the error. Example values include: global, push, and usageLimits.
    QString m_domain = QString();
    /// The specific item within the locationType that caused the error.
    /// For example, if you specify an invalid value for a parameter,
    /// the location will be the name of the parameter.
    QString m_reason = QString();
    /// The location or part of the request that caused the error.
    /// Use with location to pinpoint the error. For example,
    /// if you specify an invalid value for a parameter,
    /// the locationType will be parameter and the location will be the name of the parameter.
    QString m_message = QString();
    /// Description of the error.
    QString m_locationType = QString();
    /// Example values include invalid, invalidParameter, and required.
    QString m_location = QString();
    /// return if constructor is empty
    bool m_isEmpty = true;
};

/*!
 * \class GDriveErrorFormat
 * \brief The GDriveErrorFormat class provide parse JSON error format
 *
 * - 負責解析google drive的json格式資料，單純資料型別，不涉及錯誤處理
 *
 * ## Reference
 * - [HTTP status and error codes for JSON](https://cloud.google.com/storage/docs/json_api/v1/status-codes)
 * - [Resolve errors](https://developers.google.com/drive/api/v3/handle-errors)
 */
class GDriveErrorFormat
{
public:
    /// counstructor by QJsonDocument
    explicit GDriveErrorFormat(const QJsonDocument &doc);
    /// counstructor empty object
    explicit GDriveErrorFormat();
    /// destructor
    ~GDriveErrorFormat();
    /// An HTTP status code value, without the textual description.
    int code() const;
    /// A container for the error information.
    QJsonObject error() const;
    /// A container for the error details.
    QJsonArray errors() const;
    /// return Error object form errors
    QJsonValue errorListAt(int index);
    /// Description of the error. Same as errors.message.
    QString message() const;
    /// return object is empty
    bool isEmpty();

private:
    /// return if constructor document is empty
    bool m_isEmpty = true;
    /// An HTTP status code value, without the textual description.
    int m_code = 404;
    /// Description of the error. Same as errors.message.
    QString m_message = QString();
    /// A container for the error details.
    QJsonArray m_errors = QJsonArray();
};
}

#endif // GDRIVEERRORFORMAT_H
