
# Homekit_ESP_AC_IRemote
[![Release](https://img.shields.io/github/v/tag/LouisLee985/Homekit-Air-Conditioner-Remote?color=red&label=release)](https://github.com/LouisLee985/Homekit-Air-Conditioner-Remote/releases/latest)

<p align="center">
<img src="/image/HomeApp_Icon_2.svg" width="20%"/>
</p>

<div align="center">
<img src="/image/process.svg"  width="90%"/>
</div>


### [WiFiManager](https://github.com/tzapu/WiFiManager?tab=readme-ov-file#how-it-works)



<div align="center">
<br><img src="/image/WiFi_Configure.jpg"  width="80%"/>
</div>

### Add accessories 添加 HomeKit 配件

* Homekit 配对码：985-21-985。
   
<div align="center">
<br><img src="/image/qrcode.jpg"  width="20%"/>
</div>

   　
* [向“家庭”App 添加配件](https://support.apple.com/en-us/104998) ３种途径：
<!-- 
https://support.apple.com/en-us/HT204893 
-->


 <br>
　　１．使用 iPhone 或 iPad 上的相机扫描上面的二维码。　＂家庭＂二维码　　在＂家庭＂中打开
<br>
　　２．或者　　打开“家庭”App，然后轻点“添加”；　　轻点“添加配件”。

<div align="center">
<br><img src="/image/IMG_3209.jpg"  width="80%"/>
</div>
<br>
 
　　３．也可以输入配对码。

<div align="center">
<br><img src="/image/Add_accessories3.jpg"  width="80%"/>
</div>

<!-- 
<div align="center">
<br><img src="/image/Add_accessories1.jpg"  width="90%"/>
<br><img src="/image/Add_accessories2.jpg"  width="80%"/>
</div>
-->
<br>

### 空调功能

1. Apple Homekit App 空调功能：开机，关机，制冷，制热，风速，当前环境温湿度显示。 
<br>基于 [HomeKit Accessory Protocol Specification(Noncommercial Version) Release R2](https://developers.apple.com/homekit/faq) ，有 *`此配件尚未经过认证可与Homekit配合使用， 因此部分功能可能不可用。`* 字样。
2. 如果需要出门在外时远程控制 HomeKit 配件，请将 HomePod、HomePod mini、Apple TV 或 iPad  [设置为家居中枢。 ](https://support.apple.com/en-us/102557)
3. 红外信号反馈：普通空调遥控器进行 开机，关机，制冷，制热，风速等 Apple Homekit 协议支持的动作时，Homekit App 会同步更新状态。


<div align="center">
<br><img src="/image/HAP-Specification-Non-Commercial-Version.jpg"  width="60%"/>
<br><img src="/image/Add_accessories3.PNG"  width="80%"/>
</div>

### Button IO0 按键定义

1. 长按 IO0 按键5-10S，LED 2/S 闪烁，松开按键, 空调遥控识别：15S 内普通空调遥控器对准此项目上的遥控接收头(IR_R)按一下开关机键，等待 LED 闪烁停止。IRremoteESP8266 库，支持市场上绝大多数[空调品牌](https://github.com/crankyoldgit/IRremoteESP8266/blob/master/SupportedProtocols.md#ir-protocols-supported-by-this-library)。
2. 长按 IO0 按键 10-15S，LED 2/S 闪烁，4/S 闪烁，松开按键，恢复设置：抹除 Homekit 配对信息，抹除 Wifi 连接信息，抹除空调遥控识别信息。
 



# Thanks
* [HomeKit Accessory Protocol Specification(Noncommercial Version) Release R2](https://developers.apple.com/homekit/faq)
* [arduino-sht](https://github.com/Sensirion/arduino-sht) 
* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266) 
* [Arduino-HomeKit-ESP8266](https://github.com/Mixiaoxiao/Arduino-HomeKit-ESP8266) 
* [arduino-button-long-press-short-press](https://arduinogetstarted.com/tutorials/arduino-button-long-press-short-press)
*  https://github.com/openscopeproject/InteractiveHtmlBom

<div align="center">  
<img src="https://myoctocat.com/assets/images/base-octocat.svg"  width="10%"/> 
</div>

