# Google Drive Api 整理

## 參數整理

### About::get

- Gets information about the user, the user's Drive, and system capabilities.
- HTTP request : `GET https://www.googleapis.com/drive/v3/about`
- [Doc](https://developers.google.com/drive/api/v3/reference/about/get)

| name                          | type    | value                                    |
| ----------------------------- | ------- | ---------------------------------------- |
| **Required query parameters** |         |                                          |
| fields                        | string  |                                          |

``` http
GET https://www.googleapis.com/drive/v3/about?fields=user(displayName,photoLink,emailAddress) HTTP/1.1
Host: www.googleapis.com
User-Agent: QtOAuth/1.0 (+https://www.qt.io)
Authorization: Bearer ya29.a0AfH6SMBo2GcBf10IxUAAxz1Ud_5WHMh2VXm-Go0lDo6L3unQ2IFs2TUfl0-
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
```

### Files: create

- Creates a new file.
- `class GDriveFileSimpleCreate`
- `class GDriveFileMultipartCreate`
- `class GDriveFileResumableCreate`
- [Doc](https://developers.google.com/drive/api/v3/reference/files/create)

| name                          | type    | value                                    |
| ----------------------------- | ------- | ---------------------------------------- |
| **Required query parameters** |         |                                          |
| uploadType                    | string  | (value: 'media','multipart','resumable') |
| **OAuth query parameters**    |         |                                          |
| access_token                  | string  | (value: google->token() )                |
| **Optional query parameters** |         |                                          |
| enforceSingleParent           | boolean | (Default: false)                         |
| ignoreDefaultVisibility       | boolean | (Default: false)                         |
| keepRevisionForever           | boolean | (Default: false)                         |
| ocrLanguage                   | string  |                                          |
| supportsAllDrives             | boolean | (Default: false)                         |
| useContentAsIndexableText     | boolean | (Default: false)                         |

#### Simple upload request example

``` http
POST https://www.googleapis.com/upload/drive/v3/files?uploadType=media&access_token=ya29.a0AfH6SMBo2GcBf10IxUAAxz1Ud_5WHMh2VXm-Go0lDo6L3unQ2IFs2TUfl0-BDR9muvlAxuN9b4BRDErDs6_ZZg2EjsCc2E5TUEsHbtP7iO1aRMU2wmAfmt2o45mf1t-1r_qkCrEpcQPXllq0FEWpwLHDsiZCZy3J1uo HTTP/1.1
Host: www.googleapis.com
Content-Type: text/plain
Authorization: : Bearer ya29.a0AfH6SMBo2GcBf10IxUAAxz1Ud_5WHMh2VXm-Go0lDo6L3unQ2IFs2TUfl0-BDR9muvlAxuN9b4BRDErDs6_ZZg2EjsCc2E5TUEsHbtP7iO1aRMU2wmAfmt2o45mf1t-1r_qkCrEpcQPXllq0FEWpwLHDsiZCZy3J1uo
Content-Length: 1064
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0

"file body"
```

### Files: update

- Updates a file's metadata and/or content with patch semantics.
- `GDriveFileSimpleUpdate`
- `GDriveFileMultipartUpdate`
- `GDriveFileResumableUpdate`
- [Doc](https://developers.google.com/drive/api/v3/reference/files/update)

| name                          | type    | value                                    |
| ----------------------------- | ------- | ---------------------------------------- |
| **Path parameters**           |         |                                          |
| fileId                        | string  |                                          |
| **Required query parameters** |         |                                          |
| uploadType                    | string  | (value: 'media','multipart','resumable') |
| **OAuth query parameters**    |         |                                          |
| access_token                  | string  | (value: google->token() )                |
| **Optional query parameters** |         |                                          |
| addParents                    | string  |                                          |
| enforceSingleParent           | boolean | (Default: false)                         |
| keepRevisionForever           | boolean | (Default: false)                         |
| ocrLanguage                   | string  |                                          |
| removeParents                 | string  |                                          |
| supportsAllDrives             | boolean | (Default: false)                         |
| useContentAsIndexableText     | boolean | (Default: false)                         |

### Files: get

- 下載檔案/取得目標檔案的matadata
- `class GDriveFileGet`(取得matadata)
- `class GDriveFileDownloader`(下載檔案)
- Note: 只有下載檔案需要 acknowledgeAbuse 及 alt=media 參數
- [Doc](https://developers.google.com/drive/api/v3/reference/files/get)

| name                          | type    | value                                        |
| ----------------------------- | ------- | -------------------------------------------- |
| **Path parameters**           |         |                                              |
| fileId                        | string  |                                              |
| **OAuth query parameters**    |         |                                              |
| key                           | string  | (value: google->clientIdentifierSharedKey()) |
| **Required query parameters** |         |                                              |
| alt                           | string  | media (download only)                        |
| **Optional query parameters** |         |                                              |
| acknowledgeAbuse              | boolean | (download only) (Default: false)             |
| fields                        | string  |                                              |
| supportsAllDrives             | boolean | (Default: false)                             |

### Files: list

- Lists or searches files.
- `class GDriveFileSearch`
- string類的參數輸入值有範圍限制
- [Doc](https://developers.google.com/drive/api/v3/reference/files/list)

| name                          | type    | value                                        |
| ----------------------------- | ------- | -------------------------------------------- |
| **Optional query parameters** |         |                                              |
| corpora                       | string  | (value: 'user','drive','allDrives','domain') |
| driveId                       | string  |                                              |
| fields                        | string  |                                              |
| includeItemsFromAllDrives     | boolean | (Default: false)                             |
| orderBy                       | string  |                                              |
| pageSize                      | integer | (value: 1-1000) (Default: 100)               |
| pageToken                     | string  |                                              |
| q                             | string  |                                              |
| spaces                        | string  | (value: 'drive','appDataFolder','photos')    |
| supportsAllDrives             | boolean | (Default: false)                             |
