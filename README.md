# espWebServerOTAMqtt
An easy-to-use arduino library for creating an HTML webserver on your esp8266 that accepts OTA (Over the Air) code uploads and also provides a framework for publishing sensor data to Home Assistant over MQTT. Wifi and MQTT credentials are stored in SPIFFs, and when first run a Wifi Access Point (AP) is created so you can easily connect to the device and do the initial configuration without having to hard-code credentials anywhere. 


## How to Use
Clone this repo into your **arduino/libraries** folder. 

To find the basepath of arduino on your PC, open the Arduino IDE and click File -> Preferences:

![image](https://user-images.githubusercontent.com/31904545/126867515-dc61a468-0e8d-4fa3-9aa3-18384a1f8cce.png)

Then start a new sketch in your Arduino IDE.  Using the library is as simple as:
```
#include <espWebServerOTAMqtt.h>

MyCommon  client("espConservatory1");

void setup() {
  client.setup();
}

void loop() {
  client.loop();
}
```

Uploading this simple sketch to a esp8266 device will put it into Wifi AP mode and broadcast its own SSID, which will be in the format ```mortylabs_<devicemacaddr>```:

![image](https://user-images.githubusercontent.com/31904545/128343205-7b3e722e-ad16-476e-8971-19f513c11999.png)
  
Connect to the AP and navigate to **192.168.4.1** where you can enter WiFi and MQTT credentials on the **setup tab** 

![image](https://user-images.githubusercontent.com/31904545/128088647-1e573e9c-77d1-4a0b-9fd5-87567a74625b.png)

To add your own html body to the main page (i.e. maybe you want to display some live sensor data), simply call the function ``` client.set_main_page_html_body (string_goes_here); ```

## Examples

For a fully working exammple of a BMP280 baromoter and temperature sensor and capacitive soil moisture sensor publishing data to MQTT for Home Assistant auto-discovery, and displaying the sensor data in a html table on the device itself, see [https://github.com/mortylabs/IoT-ESP-Devices/tree/master/indoors_soil_baro_temperature](url)

## TO DO

- [ ] add method to allow a custom bitmap logo to displayed at the top of each html page
