# IoT2025_FireDetection

# 火災偵測與聯防系統



## 硬體接線
需要元件：

* 麵包板
* esp32開發版(nodemcu32s)
* MQ-2煙霧感測器
* KY-026火焰感測器
* DS18B20溫度感測器
* Buzzer
* 一定數量的杜邦線(至少要20條，公公、公母、母母都可以，看你怎麼接)
* 4.7k 歐姆電阻 3 個

<img src="/img/components_esp32.jpg" />\
<img src="/img/components_breadboard.jpg" />\
<img src="/img/components_mq2.jpg" />\
<img src="/img/components_ky026.jpg" />\
<img src="/img/components_ds18b20.jpg" />\
<img src="/img/components_buzzer.jpg" />\
<img src="/img/components_dupontline.jpg" />\
<img src="/img/components_47k.jpg" />

esp32 的規格如下

<img src="/img/esp32_pinout.jpg" />

接線圖如下

<img src="/img/Wiring Diagram.png" />



## Arduino IDE 設定
需要下載的套件：

Sketch -> Incluing Library -> Manage Libraries
```
1. Arduino IDE, version 0.2.0, by Arduino
2. Blynk, version1.3.2, by Volodymyr Shymanskyy
3. BlynkNcpDriver, version0.6.3, by Volodymyr Shymanskyy
4. DallasTemperature, version 4.0.3, by Miles Burton
5. Firebase, version 1.0.1, by Rupak Poddar
6. Firebase ESP32 Client, version 4.4.17, by Mobizt
7. OneWire, version 2.3.8, by Jim Studt, Tom Pollard, Robin James, ...
```

程式設定：
在 fire_detection.ino" 中，需要更改一些程式碼：

* 1~3 行的 blynk 設定如 [Blynk設定](#blynk-專案設定)
* 21~22 行的 Wi-Fi 請設定成自己的名稱和密碼
* 24~25 行的 firebase 設定如 [Firebase設定](#firebase-專案與資料庫設定)



## Firebase 專案與資料庫設定
進入 firebase 控制台 (不是 firebase 首頁，是 firebase console 喔！)。點擊 "建立 firebase 專案"，輸入專案名稱 (我的叫 IoT2025Fire)，接著就是一直下一步就完成了。

<img src="/img/firebase_1.png" />

接著點進 realtime database 裡面，在規則這裡，要改成 true，程式碼才能使用。

<img src="/img/firebase_2.png" />

左側的專案導覽有一個齒輪，點擊之後點專案設定

<img src="/img/flask_1.png" />

在一般設定底下 "新增應用程式"，然後選擇 web (</> 這個標籤)，取應用程式的名稱 (我的叫 esp32)，該輸入的輸入、該打勾的打勾

<img src="/img/firebase_3.png" />

24 行的 FIREBASE_HOST，就是 databaseURL ；25 行的 FIREBASE_AUTH，就是 apiKey

<img src="/img/firebase_4.png" />



## Blynk 專案設定


此外，你也可以參考[這個影片](https://youtu.be/aPIM9in89pU?si=B9uPW6IaaR9JXPTj)，我就是看這個設定出來的



## Email 轉寄設定
進入 email 網頁中，在右邊有一個設定的齒輪，點擊 "查看所有設定"

<img src="/img/email_1.png" />

進入 "轉寄和 POP/IMAP"，點擊 "新增轉寄地址"，在跳出的視窗中填入你要轉寄的地址後，會進行手機驗證，驗證過了之後，就成功這個地址加入轉寄列表中了。

<img src="/img/email_2.png" />

接著看你是否要設定篩選器，我有，有的話大概會長這樣

<img src="/img/email_3.png" />

此外，你也可以參考[這個影片](https://youtu.be/ELt1DCB_DD0?si=wdDlpsCYugm9VbIa)，我就是看這個設定出來的



## 執行專案
可以測試(要放測試的.ino檔上來)



## Flask 專案設定
在 fire_analysis 資料夾底下，要執行 app.py，tree 如下。不過在執行之前，要先把 Firebase 的許可證拿下來。
```
fire_analysis
│   app.py
│   iot2025fire-firebase-adminsdk-fbsvc-7534bd2067.json
|   db.json
|
└───static
│   
└───templates
    │   index.html
```
進入 firebase，點擊你的專案 (我的叫 IoT2025Fire)，左側的專案導覽有一個齒輪，點擊之後點專案設定

<img src="/img/flask_1.png" />

選擇服務帳戶後會看到 Firebase Admin SDK，在最下面有一個"產生新的私密金鑰"，點擊就會生成一個 .json，接著把那個放到跟 app.py 同個目錄底下就可以了。

<img src="/img/flask_2.png" />



## ngrok 設定教學
進入 [ngrok](https://ngrok.com/) 官網，登入。

下載執行檔 (我是windows 64-bit)，下載結果是一個 .zip，解壓縮之後只有一個 ngrok.exe，你可以把這個執行檔放在任何地方 (我放在 C 槽底下)，然後在 ngrok.exe 在的地方開啟 cmd，並回網頁上複製這行到 cmd 上，
```
ngrok config add-authtoken YOUR_AUTHTOKEN
```
YOUR_AUTHTOKEN 記得要換成自己的 token，它像一個亂數。

接著輸入這行就可以把 Flask 的網頁變成公開網址
```
ngrok http http://localhost:5000
```
