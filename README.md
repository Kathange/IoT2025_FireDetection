# IoT2025_FireDetection

# 火災偵測與聯防系統

使用工具與元件：\
硬體\
麵包版、esp32開發版(nodemcu32s)、MQ-2煙霧感測器、KY-026火焰感測器、DS18B20溫度感測器、Buzzer\
軟體\
ArduinoIDE、Firebase、Blynk、Flask

(之後放.ino檔、flask專案、blynk設定教學、ngrok設定教學、接線的圖、email轉寄設定、firebase設定教學)\
(寫完之後要放圖)

## Flask專案設定
在 fire_analysis 資料夾底下，要執行 app.py，在執行之前，要先把 Firebase 的許可證拿下來。\
進入 firebase，點擊你的專案 (我的叫 IoT2025Fire)，左側的專案導覽有一個齒輪，點擊之後點專案設定，選擇服務帳戶後會看到 Firebase Admin SDK，在最下面有一個"產生新的私密金鑰"，點擊就會生成一個 .json，接著把那個放到跟 app.py 同個目錄底下就可以了。

## ngrok設定教學
進入 [ngrok](https://ngrok.com/) 官網，登入 (看要用什麼都可以)。\
下載執行檔 (我是windows 64-bit)，下載結果是一個 .zip，解壓縮之後只有一個 ngrok.exe，你可以把這個執行檔放在任何地方 (我是放在 C 槽底下)，然後在 ngrok.exe 在的地方開啟 cmd，並回網頁上複製這行到 cmd 上，
```
ngrok config add-authtoken YOUR_AUTHTOKEN
```
YOUR_AUTHTOKEN 記得要換成自己的 token，它像一個亂數。

接著輸入這行就可以把 Flask
```
ngrok http http://localhost:5000
```

