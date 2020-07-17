#

```
1、有的apk反编译失败
apk并没有加固，可以尝试下载最新的apktool.jar，修改名称后替换掉 bin\apktool\apktool\ShakaApktool.jar

2、回编译的时候出现找不到“keyboardNavigationCluster”该属性
找到电脑中C盘下的1.apk的路径，例如：C:\Users\XX\AppData\Local\apktool\framework\1.apk，然后将其删掉，再重试就可以解决以上问题
好像每次回编都需要删除     


```