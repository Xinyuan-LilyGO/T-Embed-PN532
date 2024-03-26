
#pragma once
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl.h"
#include "scr_mrg.h"
#include "assets/assets.h"

// The default is landscape screen, HEIGHT and WIDTH swap
#define DISPALY_WIDTH  TFT_HEIGHT
#define DISPALY_HEIGHT TFT_WIDTH

#define COLOR_BG       0x161823   // 漆黑色
#define COLOR_FOCUS_ON 0x91B821   // 绿茶色
#define COLOR_TETX     0xE9D4DF   // 丁香淡紫
#define COLOR_4  //
#define COLOR_5  //
#define COLOR_6  //


enum{
    SCREEN0_ID = 0,
    SCREEN1_ID,
    SCREEN2_ID,
    SCREEN3_ID,
    SCREEN4_ID,
    SCREEN5_ID,
    SCREEN6_ID,
    SCREEN7_ID,
    SCREEN_ID_MAX,
};

void ui_entry(void);