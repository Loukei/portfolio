# Develop Journal

## Note

本想做一個類似Google網頁中登入帳號後會顯示名稱與簡易選項的UI，考慮之後決定使用簡易的QMenu來做。
在QMenu中可以插入QWidget來顯示，需要透過QWidgetAction來完成。
``` c++
CustomWidget *cwidget = new CustomWidget(menu);
QWidgetAction *action = new QWidgetAction(menu);
action->setDefaultWidget(cwidget);
```
需要注意的是，選擇插入的widget發生大小變化時，所在的QMenu本身不會跟著適應大小變化。
也就是說，假如cwidget內部元件hide時，本身在menu裡大小不會改變

針對這個問題，我個人嘗試過`QWidget::adjustSize()` `QWidget::updateGeometry()`等，均沒有用，後來發現需要個別加入widget,即**每一個label，都需要加入一個widgetaction，如此一來每個action單獨hide時，對應的label就會被正確的隱藏並更新Menu的大小**
``` c++
label_UserIcon = new QLabel(parrent);
label_UserName = new QLabel(parrent);
label_UserEmail = new QLabel(parrent);

action_UserIcon = new QWidgetAction(parrent);
action_UserName = new QWidgetAction(parrent);
action_UserEmail = new QWidgetAction(parrent);

action_UserIcon->setDefaultWidget(label_UserIcon);
action_UserName->setDefaultWidget(label_UserName);
action_UserEmail->setDefaultWidget(label_UserEmail);

action_UserName->setVisible(false);
action_UserEmail->setVisible(false);
```

## Todo

* 上傳/下載等動作要提供進度支援
* 對任務新增abort方法
* 在GDriveServiece開一個口，方便設定OAuth變數
* 考慮修改xxxArg的輸入方式，使用QUrlQuery或QVariantMap會更好
* 把Dialog裡的treeview結合到MainWindow裡
* Login的按鈕在等待認證時應該暫時鎖住，直到認證通過/不通過才回復，觀察一下是否有對應的signal

## Reference