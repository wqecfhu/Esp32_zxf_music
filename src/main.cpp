//  念张师音乐播放—水墨屏
//  2026-8-14
//  使用Esp32多核配置，实现无阻塞播放音乐，同步同步刷新
#include <Arduino.h>
#include <SD.h>
#include <GxEPD2_BW.h>
#include "image.h"
#include "melody.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #define BUZZER_PIN 21
#define HSPI_SCK    14
#define HSPI_MOSI   13

#define BUZZER_PIN  22
#define VSPI_SCK    18
#define VSPI_MOSI   23
#define VSPI_MISO   19
#define SD_CS       5

#define EPD_CS   25
#define EPD_DC   26
#define EPD_RST  27
#define EPD_BUSY 33



#define BEEP_CH 0
#define NOTE_PAUSE 0
SPIClass hspi(HSPI);

#define COUNT(arr)   sizeof(arr) / sizeof(arr[0])

//配置多核心同步
void core0_tone(void* ps);    //核心0 音乐
void core1_void(void* ps);    //核心1 刷新


GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT> display(
    GxEPD2_426_GDEQ0426T82(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));


void setup() {
    Serial.begin(115200);


    //配置HSPI到水墨屏
    //利用ESP32GPIO交换矩阵配置HSPI
    hspi.begin(HSPI_SCK, 99, HSPI_MOSI);
    hspi.setFrequency(4000000UL);
    SPISettings epdSPISettings(4000000UL, MSBFIRST, SPI_MODE0);

    //初始化水墨屏到HSPI
    display.epd2.selectSPI(hspi, epdSPISettings);
    display.init(0, 10, 1, 1);
    display.setRotation(2);
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    // 3. 绘制最简图形（黑块）
    display.display();
    Serial.println("刷新");
    //display.drawBitmap(0,0, IMAGE1, 800, 480, GxEPD_BLACK);
    //display.drawBitmap(0,128, Bitmap_dfsn, 296, 128, GxEPD_BLACK);
    delay(500);
    // pinMode(BUZZER_PIN, OUTPUT);

    // playMidi(BUZZER_PIN, midi1, ARRAY_LEN(midi1));
    ledcSetup(0, 1000, 8);
    ledcAttachPin(BUZZER_PIN, 0);
    ledcWriteTone(0,0); 

    //playMelody(midi2, COUNT(midi2));


    xTaskCreatePinnedToCore(core0_tone, NULL, 4096, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(core1_void, NULL, 4096, NULL, 10, NULL, 1);
}

void loop() {
    delay(1000);
}

//多核同步
void core0_tone(void* ps) {
    Serial.println("开始演奏");
    while (true)
    {
        for (size_t i = 0; i < NOTE_COUNT; i++)
        {
            ledcWriteTone(BEEP_CH, (note_t)melody[i].freq);
            delay(melody[i].dur_ms);
            ledcWriteTone(BEEP_CH, 0);
            //if (melody[i + 1].start_ms != 0) {
            //delay((melody[i].start_ms + melody[i].dur_ms) - melody[i + 1].start_ms);
            //}
        }
        vTaskDelay(5000);
        Serial.println("播放完毕,删除核心0任务");
        vTaskDelete(NULL);
    }
}

void core1_void(void* ps) {
    while (true)
    {
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0,0, IMAGE1, 713, 480, GxEPD_BLACK);
        display.display(1);
        delay(8000);
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0,0, IMAGE2, 800, 480, GxEPD_BLACK);
        display.display(1);
        delay(8000);
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0,0, IMAGE3, 800, 480, GxEPD_BLACK);
        display.display(1);
        delay(8000);
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0,0, IMAGE4, 800, 480, GxEPD_BLACK);
        display.display(1);
        delay(8000);
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0,0, IMAGE5, 800, 480, GxEPD_BLACK);
        display.display(1);
        delay(8000);
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0,0, IMAGE6, 800, 480, GxEPD_BLACK);
        display.display(1);
        delay(8000);
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0,0, IMAGE7, 800, 480, GxEPD_BLACK);
        display.display();
        delay(6000);
        vTaskDelay(2000);
    }
    
}
