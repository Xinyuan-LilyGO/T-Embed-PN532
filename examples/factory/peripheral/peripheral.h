#pragma once
#include "../utilities.h"

/**----------------------------- WS2812 ----------------------------------**/
#include <FastLED.h>
#define WS2812_DEFAULT_LIGHT 0
extern TaskHandle_t ws2812_handle;

void ws2812_init(void);
void ws2812_set_color(CRGB c);
void ws2812_set_light(uint8_t light);
void ws2812_set_mode(int m);
int ws2812_get_mode(void);
void ws2812_task(void *param);

/**------------------------------- NFC -----------------------------------**/
// PN532
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
extern TaskHandle_t nfc_handle;
void nfc_init(void);
uint32_t nfc_get_ver_data(void);
void nfc_task(void *param);

/**------------------------------ LORA -----------------------------------**/
// CC1101
#include <RadioLib.h>
#define LORA_MODE_SEND 1
#define LORA_MODE_RECV 2

extern TaskHandle_t lora_handle;
extern SemaphoreHandle_t radioLock;

void lora_init(void);
void lora_mode_sw(int m);
int lora_get_mode(void);
void lora_send(const char *str);
void lora_task(void *param);

/**---------------------------- BATTERY ----------------------------------**/
#include "BQ25896.h"

extern TaskHandle_t battery_handle;
extern BQ25896 battery_charging;
void battery_task(void *param);
