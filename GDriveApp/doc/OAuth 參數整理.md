# OAuth 參數整理

## Files: create

- Creates a new file.
- class GDriveFileSimpleCreate
- class GDriveFileMultipartCreate
- class GDriveFileResumableCreate

**Required query parameters**
-------------------------------------------------
uploadType 					string	(value: 'media','multipart','resumable')
-------------------------------------------------
**OAuth query parameters**
-------------------------------------------------
access_token 				string 	(value: google->token() )
-------------------------------------------------
**Optional query parameters**
-------------------------------------------------
enforceSingleParent 		boolean	 (Default: false) 
ignoreDefaultVisibility 	boolean  (Default: false) 
keepRevisionForever 		boolean  (Default: false) 
ocrLanguage 				string
supportsAllDrives 			boolean  (Default: false) 
useContentAsIndexableText 	boolean  (Default: false) 

## Files: update

- Updates a file's metadata and/or content with patch semantics.
- GDriveFileSimpleUpdate
- GDriveFileMultipartUpdate
- GDriveFileResumableUpdate

**Path parameters**
------------------------------------------------
fileId 						string
------------------------------------------------
**Required query parameters**
------------------------------------------------
uploadType 					string	(value: 'media','multipart','resumable')
------------------------------------------------
**OAuth query parameters**
-------------------------------------------------
access_token 				string 	(value: google->token() )
-------------------------------------------------
**Optional query parameters**
------------------------------------------------
addParents 					string
enforceSingleParent 		boolean	 (Default: false) 
keepRevisionForever 		boolean	 (Default: false) 
ocrLanguage 				string  
removeParents 				string  
supportsAllDrives 			boolean  (Default: false) 
useContentAsIndexableText 	boolean  (Default: false) 

## Files: get

- 下載檔案/取得目標檔案的matadata
- class GDriveFileGet(取得mata)
- class GDriveFileDownloader(下載檔案)
- Note: 只有下載檔案需要 acknowledgeAbuse 及 alt=media 參數

**Path parameters**
------------------------------------------------
fileId 						string
------------------------------------------------
**OAuth query parameters**
-------------------------------------------------
key 						string 	(value: google->clientIdentifierSharedKey())
------------------------------------------------
**Required query parameters**
------------------------------------------------
alt=media 							(download only)
------------------------------------------------
**Optional query parameters**
------------------------------------------------
acknowledgeAbuse 			boolean (download only) (Default: false) 
fields 						string
supportsAllDrives 			boolean (Default: false)

## Files: list

- Lists or searches files.
- class GDriveFileSearch
- string類的參數輸入值有範圍限制

**Optional query parameters**
------------------------------------------------
corpora 					string  (value: 'user','drive','allDrives','domain')
driveId 					string 
fields 						string
includeItemsFromAllDrives 	boolean (Default: false) 
orderBy 					string
pageSize 					integer (value: 1-1000) (Default: 100) 
pageToken 					string
q 							string
spaces 						string  (value: 'drive','appDataFolder','photos')
supportsAllDrives 			boolean (Default: false) 
