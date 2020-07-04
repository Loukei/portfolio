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

#### About::get request example

``` http
GET https://www.googleapis.com/drive/v3/about?fields=user(displayName,photoLink,emailAddress) HTTP/1.1
Host: www.googleapis.com
User-Agent: QtOAuth/1.0 (+https://www.qt.io)
Authorization: Bearer [Token]
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
POST https://www.googleapis.com/upload/drive/v3/files?uploadType=media&access_token=[Token] HTTP/1.1
Host: www.googleapis.com
Content-Type: text/plain
Authorization: : Bearer [Token]
Content-Length: 1064
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0

** YOUR FILE CONTENT **
```

#### Multipart upload request example

``` http
POST https://www.googleapis.com/upload/drive/v3/files?uploadType=multipart HTTP/1.1
Host: www.googleapis.com
Authorization: Bearer [Token]
Content-Type: multipart/related; boundary="foo_bar_baz"
MIME-Version: 1.0
Content-Length: 1217
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0

--foo_bar_baz
Content-Type: application/json; charset=UTF-8

{"name":"soviet example"}
--foo_bar_baz
Content-Type: text/plain

﻿
** YOUR FILE CONTENT **
```

#### Resumable upload request example

``` http
POST https://www.googleapis.com/upload/drive/v3/files?uploadType=resumable HTTP/1.1
Host: www.googleapis.com
Content-Type: application/json; charset=UTF-8
Authorization: Bearer [Token]
X-Upload-Content-Type: text/plain
X-Upload-Content-Length: 1064
Content-Length: 25
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0

{"name":"soviet example"}
```

``` http
PUT https://www.googleapis.com/upload/drive/v3/files?uploadType=resumable&upload_id=[Session URI] HTTP/1.1
Host: www.googleapis.com
Content-Type: text/plain
Content-Length: 1064
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0


** YOUR FILE CONTENT **
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

#### Files: update request example

``` http
PATCH https://www.googleapis.com/upload/drive/v3/files/[fileID]?uploadType=media&access_token=[Token] HTTP/1.1
Host: www.googleapis.com
Content-Type: text/plain
Authorization: : Bearer [Token]
Content-Length: 1064
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0


** YOUR FILE CONTENT **
```

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

#### Files: get (mata) request example

``` http
GET https://www.googleapis.com/drive/v3/files/[fileID]?key=[ClientSecert] HTTP/1.1
Host: www.googleapis.com
Authorization: Bearer [Token]
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0
```

#### Files: get (media) request example

``` http
GET https://www.googleapis.com/drive/v3/files/[fileID]]?alt=media&fields=id,name&key=[ClientSecert] HTTP/1.1
Host: www.googleapis.com
Authorization: Bearer [Token]
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0
```

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

#### Files: list request example

``` http
GET https://www.googleapis.com/drive/v3/files?pageSize=100&q=name%20%3D%20'Untitled' HTTP/1.1
Host: www.googleapis.com
Authorization: Bearer [Token]
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-TW,en,*
User-Agent: Mozilla/5.0
```
