/* FLASH SETTINGS
Board: LOLIN D32
Flash Frequency: 80MHz
Partition Scheme: Minimal SPIFFS
https://www.online-utility.org/image/convert/to/XBM
*/

#define TOUCH_CS 21 // enable touch
#include <ESP32-Chimera-Core.h> // https://github.com/tobozo/ESP32-Chimera-Core or regular M5Stack Core
#define tft M5.Lcd


#include <WiFi.h>
#include <Wire.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <Arduino.h>


#include "Assets.h"
#include "Display.h"
#include "WiFiScan.h"
#include "MenuFunctions.h"
#include "SDInterface.h"
#include "Web.h"
#include "Buffer.h"
#include "BatteryInterface.h"
#include "TemperatureInterface.h"
#include "LedInterface.h"
//#include "icons.h"

/*
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
*/

Display display_obj;
WiFiScan wifi_scan_obj;
MenuFunctions menu_function_obj;
SDInterface sd_obj;
Web web_obj;
Buffer buffer_obj;
BatteryInterface battery_obj;
TemperatureInterface temp_obj;
LedInterface led_obj;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, PIN, NEO_GRB + NEO_KHZ800);

uint32_t currentTime  = 0;

void setup()
{

  M5.setTouchSpiShared( TOUCH_CS ); // call this before M5.begin() !!
  M5.begin(true, false); // don't start the SD
  M5.ts->setRotation(0);

  tft.drawRect(10,10,20,20,0xFFFF);
  tft.print("BLAH");
  delay(1000);

  //pinMode(FLASH_BUTTON, INPUT);
  //pinMode(TFT_BL, OUTPUT);
  //digitalWrite(TFT_BL, LOW);

#if BATTERY_ANALOG_ON == 1
  //pinMode(BATTERY_PIN, OUTPUT);
  //pinMode(CHARGING_PIN, INPUT);
#endif

  // Preset SPI CS pins to avoid bus conflicts
  //digitalWrite(TFT_CS, HIGH);
  //digitalWrite(SD_CS, HIGH);

  //Serial.begin(115200); // serial is already started by M5.begin()

  display_obj.RunSetup();
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  //digitalWrite(TFT_BL, HIGH);

  tft.println("Marauder " + display_obj.version_number + "\n");

  tft.println("Started Serial");

  Serial.println("\n\n--------------------------------\n");
  Serial.println("         ESP32 Marauder      \n");
  Serial.println("            " + display_obj.version_number + "\n");
  Serial.println("       By: justcallmekoko\n");
  Serial.println("--------------------------------\n\n");

  //Serial.println("Internal Temp: " + (String)((temprature_sens_read() - 32) / 1.8));

  Serial.println(wifi_scan_obj.freeRAM());

  tft.println("Checked RAM");

  // Do some SD stuff
  /*
  if(sd_obj.initSD()) {
    Serial.println("SD Card supported");
    tft.println("Initialized SD Card");
  } else {
    Serial.println("SD Card NOT Supported");
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.println("Failed to Initialize SD Card");
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
  }*/

  // Run display setup
  Serial.println(wifi_scan_obj.freeRAM());
  //display_obj.RunSetup();

  // Build menus
  Serial.println(wifi_scan_obj.freeRAM());
  //menu_function_obj.RunSetup();

  //tft.println("Created Menu Structure");

  // Battery stuff
  Serial.println(wifi_scan_obj.freeRAM());
  //battery_obj.RunSetup();

  tft.println("Checked battery configuration");

  // Temperature stuff
  Serial.println(wifi_scan_obj.freeRAM());
  temp_obj.RunSetup();

  tft.println("Initialized temperature interface");

  //battery_obj.battery_level = battery_obj.getBatteryLevel();

  if (battery_obj.i2c_supported) {
    Serial.println("IP5306 I2C Supported: true");
  }
  else
    Serial.println("IP5306 I2C Supported: false");

  Serial.println(wifi_scan_obj.freeRAM());

  // Do some LED stuff
  //led_obj.RunSetup();

  tft.println("Initialized LED Interface");

  tft.println("Starting...");

  delay(1000);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  //digitalWrite(TFT_BL, LOW);

  // Draw the title screen
  tft.drawJpgFile( SPIFFS, "/marauder3L.jpg", 0 , 0);     // 240 x 320 image

  //showCenterText(version_number, 250);
  tft.drawCentreString(display_obj.version_number, 120, 250, 2);

  //digitalWrite(TFT_BL, HIGH);

  delay(5000);

  menu_function_obj.RunSetup();
}


void loop()
{
  // get the current time
  //if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM))
  currentTime = millis();

  // Update all of our objects
  //if ((!display_obj.draw_tft) &&
  //    (wifi_scan_obj.currentScanMode != OTA_UPDATE))
  if (!display_obj.draw_tft)
  {
    display_obj.main();
    wifi_scan_obj.main(currentTime);
    sd_obj.main();
    battery_obj.main(currentTime);
    temp_obj.main(currentTime);
    //led_obj.main(currentTime);
    //if ((wifi_scan_obj.currentScanMode != WIFI_ATTACK_BEACON_SPAM))
    if ((wifi_scan_obj.currentScanMode != WIFI_PACKET_MONITOR) &&
        (wifi_scan_obj.currentScanMode != WIFI_SCAN_EAPOL))
      menu_function_obj.main(currentTime);
      if (wifi_scan_obj.currentScanMode == OTA_UPDATE)
        web_obj.main();
    delay(1);
  }
  else if ((display_obj.draw_tft) &&
           (wifi_scan_obj.currentScanMode != OTA_UPDATE))
  {
    display_obj.drawStylus();
  }
  //else
  //{
  //  web_obj.main();
  //}

  //Serial.println(wifi_scan_obj.currentScanMode);

  //Serial.print("Run Time: ");
  //Serial.print(millis() - currentTime);
  //Serial.println("ms");
}
