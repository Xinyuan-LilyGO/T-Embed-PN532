
#include "ui.h"

#define UI_THEME_DARK  0
#define UI_THEME_LIGHT 1

uint8_t display_rotation  = 3;
uint8_t setting_theme     = UI_THEME_DARK;
uint32_t COLOR_BG         = 0x161823;  // UI 的背景色
uint32_t COLOR_FOCUS_ON   = 0x91B821;  // 组件选中时的颜色
uint32_t COLOR_TEXT       = 0xffffff;  // 文本的颜色
uint32_t COLOR_BORDER     = 0xBBBBBB;  // 一些组件边框的颜色
uint32_t COLOR_PROMPT_BG  = 0xfffee6;  // 提示弹窗的背景色
uint32_t COLOR_PROMPT_TXT = 0x1e1e00;  // 提示弹窗的文本色

//************************************[ Other fun ]******************************************
#if 1
bool prompt_is_busy = false;

void prompt_label_timer(lv_timer_t *t)
{
    lv_obj_del((lv_obj_t *)t->user_data);
    lv_timer_del(t);
    prompt_is_busy = false;
}

void prompt_info(const char *str, uint16_t time)
{
    if(prompt_is_busy == false){
        lv_obj_t *label = lv_label_create(lv_layer_top());
        lv_obj_set_width(label, DISPALY_WIDTH * 0.8);
        lv_label_set_text(label, str);
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
        lv_obj_set_style_radius(label, 5, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(label, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_pad_hor(label, 3, LV_PART_MAIN);
        lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
        lv_obj_set_style_bg_color(label, lv_color_hex(COLOR_PROMPT_BG), LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_hex(COLOR_PROMPT_TXT), LV_PART_MAIN);
        lv_obj_center(label);
        lv_timer_create(prompt_label_timer, time, label);
        prompt_is_busy = true;
    }
}

void scr_back_btn_create(lv_obj_t *parent, lv_event_cb_t cb)
{
    lv_obj_t * btn = lv_btn_create(parent);
    // lv_group_add_obj(lv_group_get_default(), btn);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_height(btn, 20);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 8, 8);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(btn, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_remove_style(btn, NULL, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_pad(btn, 0, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(btn, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label2 = lv_label_create(btn);
    lv_obj_align(label2, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_text_color(label2, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_label_set_text(label2, " " LV_SYMBOL_LEFT " ");
}

void ui_theme_setting(int ui_theme)
{
    extern uint32_t default_bg_color;
    if(ui_theme == UI_THEME_DARK) {
        COLOR_BG         = 0x161823;
        COLOR_FOCUS_ON   = 0x91B821;
        COLOR_TEXT       = 0xffffff;
        COLOR_BORDER     = 0xBBBBBB;
        COLOR_PROMPT_BG  = 0xfffee6;
        COLOR_PROMPT_TXT = 0x1e1e00; 
    } else if(ui_theme == UI_THEME_LIGHT) {
        COLOR_BG         = 0xffffff;
        COLOR_FOCUS_ON   = 0x91B821;
        COLOR_TEXT       = 0x161823;
        COLOR_BORDER     = 0xBBBBBB;
        COLOR_PROMPT_BG  = 0x1e1e00;
        COLOR_PROMPT_TXT = 0xfffee6;
    }
    default_bg_color = COLOR_BG;
}
#endif
//************************************[ screen 0 ]****************************************** menu
#if 1
// 
#define MENU_PROPORTION     (0.55)
#define MENU_LAB_PROPORTION (1 - MENU_PROPORTION)

lv_obj_t *item_cont;
lv_obj_t *menu_cont;
lv_obj_t *menu_icon;
lv_obj_t *menu_icon_lab;
lv_obj_t *menu_time_lab;

int fouce_item = 0;
int hour = 0;
int minute = 0;
int second = 0;
const char *name_buf[] = { 
    "<- Lora"   ,
    "<- WS2812" , 
    "<- NFC"    , 
    "<- Battery", 
    "<- Wifi"   ,
    "<- Other"  ,
    "<- Setting"
};

void entry0_anim(void);
void switch_scr0_anim(int user_data);

// event
static void clock_upd_event(lv_event_t *e)
{
    lv_obj_t * label = lv_event_get_target(e);
    lv_msg_t * m = lv_event_get_msg(e);
    int data = (int)lv_msg_get_user_data(m);
    const int32_t *v = (int32_t *)lv_msg_get_payload(m);

    switch (data) {
        case MSG_CLOCK_HOUR:   hour = *v;    break;
        case MSG_CLOCK_MINUTE: minute = *v;  break;
        case MSG_CLOCK_SECOND: second = *v;  break;
        default: break;
    }
    lv_label_set_text_fmt(menu_time_lab, "#EE781F %02d# #C0C0C0 :# #D8E699 %02d#", hour, minute);
}

static void scr0_btn_event_cb(lv_event_t * e)
{   
    int data = (int)e->user_data;

    if(e->code == LV_EVENT_CLICKED){
        fouce_item = data;
        switch (fouce_item)
        {
            case 0: switch_scr0_anim(SCREEN2_ID); break; // lora
            case 1: switch_scr0_anim(SCREEN1_ID); break; // ws2812
            case 2: switch_scr0_anim(SCREEN3_ID); break; // nfc
            case 3: switch_scr0_anim(SCREEN5_ID); break; // battery
            case 4: switch_scr0_anim(SCREEN6_ID); break; // wifi
            case 5: switch_scr0_anim(SCREEN7_ID); break; // wifi
            case 6: switch_scr0_anim(SCREEN4_ID); break; // setting
            default:
                break;
        }
    }

    if(e->code == LV_EVENT_FOCUSED){
        switch (data)
        { 
            case 0: 
                lv_img_set_src(menu_icon, &img_lora_32); 
                lv_obj_set_style_img_recolor(menu_icon, lv_palette_main(LV_PALETTE_CYAN), LV_PART_MAIN);
                lv_obj_set_style_img_recolor_opa(menu_icon, LV_OPA_100, LV_PART_MAIN);
                break; 
            case 1: 
                lv_img_set_src(menu_icon, &img_light_32); 
                lv_obj_set_style_img_recolor_opa(menu_icon, LV_OPA_0, LV_PART_MAIN);
                break;    
            case 2: 
                lv_img_set_src(menu_icon, &img_nfc_32);
                lv_obj_set_style_img_recolor(menu_icon, lv_palette_main(LV_PALETTE_CYAN), LV_PART_MAIN);
                lv_obj_set_style_img_recolor_opa(menu_icon, LV_OPA_100, LV_PART_MAIN);
                break;      
            case 3: 
                lv_img_set_src(menu_icon, &img_battery_32); 
                lv_obj_set_style_img_recolor_opa(menu_icon, LV_OPA_0, LV_PART_MAIN);
                break;  
            case 4: 
                lv_img_set_src(menu_icon, &img_wifi_32);
                lv_obj_set_style_img_recolor_opa(menu_icon, LV_OPA_0, LV_PART_MAIN);
                break;     
            case 5: 
                lv_img_set_src(menu_icon, &img_setting_32); 
                lv_obj_set_style_img_recolor_opa(menu_icon, LV_OPA_0, LV_PART_MAIN);
                break;  
            case 6: 
                lv_img_set_src(menu_icon, &img_setting_32); 
                lv_obj_set_style_img_recolor_opa(menu_icon, LV_OPA_0, LV_PART_MAIN);
                break;  
            default:
                break;
        }
        lv_label_set_text(menu_icon_lab, ((char*)name_buf[data] + 3));
    }
}

void entry0_anim(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, item_cont);
    lv_anim_set_values(&a, DISPALY_WIDTH * MENU_PROPORTION, 0);
    lv_anim_set_time(&a, 400);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&a, [](void * var, int32_t v){
        lv_obj_set_x((lv_obj_t *)var, v);
        lv_port_indev_enabled(false);
    });
    lv_anim_set_ready_cb(&a, [](struct _lv_anim_t *a){
        lv_port_indev_enabled(true);
    });
    lv_anim_start(&a);

    lv_anim_set_time(&a, 400);
    lv_anim_set_var(&a, menu_cont);
    lv_anim_set_values(&a, -DISPALY_WIDTH * MENU_LAB_PROPORTION, 0);
    lv_anim_set_exec_cb(&a, [](void * var, int32_t v){
        lv_obj_set_x((lv_obj_t *)var, v);
        lv_port_indev_enabled(false);
    });
    lv_anim_start(&a);
}

void switch_scr0_anim(int user_data)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, item_cont);
    lv_anim_set_values(&a, 0, DISPALY_WIDTH * MENU_PROPORTION);
    lv_anim_set_time(&a, 400);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&a, [](void * var, int32_t v){
        lv_obj_set_x((lv_obj_t *)var, v);
        lv_port_indev_enabled(false);
    });
    lv_anim_set_ready_cb(&a, [](struct _lv_anim_t *a){
        scr_mgr_switch((int)a->user_data, false);
        lv_port_indev_enabled(true);
    });
    lv_anim_set_user_data(&a, (void *)user_data);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 400);
    lv_anim_set_var(&a, menu_cont);
    lv_anim_set_values(&a, 0, -DISPALY_WIDTH * MENU_LAB_PROPORTION);
    lv_anim_set_exec_cb(&a, [](void * var, int32_t v){
        lv_obj_set_x((lv_obj_t *)var, v);
        lv_port_indev_enabled(false);
    });
    lv_anim_start(&a);
}

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        }
        else {
            x =  (diff_y * 0.866);
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

void create0(lv_obj_t *parent)
{
    menu_cont = lv_obj_create(parent);
    lv_obj_set_size(menu_cont, DISPALY_WIDTH * MENU_LAB_PROPORTION, DISPALY_HEIGHT);
    lv_obj_set_scrollbar_mode(menu_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_align(menu_cont, LV_ALIGN_LEFT_MID);
    lv_obj_set_style_bg_color(menu_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_pad_all(menu_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(menu_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(menu_cont, 0, LV_PART_MAIN);

    menu_icon = lv_img_create(menu_cont);
    // lv_img_set_src(menu_icon, &Battery_Charging_48);
    lv_obj_align(menu_icon, LV_ALIGN_CENTER, 5, 0);
    
    menu_icon_lab = lv_label_create(menu_cont);
    lv_obj_set_width(menu_icon_lab, DISPALY_WIDTH * MENU_LAB_PROPORTION);
    lv_obj_set_style_text_align(menu_icon_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(menu_icon_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(menu_icon_lab, &Font_Mono_Bold_18, LV_PART_MAIN);
    // lv_label_set_text(menu_icon_lab, "battery");
    lv_obj_align_to(menu_icon_lab, menu_cont, LV_ALIGN_BOTTOM_MID, 5, -25);
    
    menu_time_lab = lv_label_create(menu_cont);
    lv_obj_set_width(menu_time_lab, DISPALY_WIDTH * MENU_LAB_PROPORTION);
    lv_obj_align(menu_time_lab, LV_ALIGN_TOP_MID, 5, 25);
    lv_obj_set_style_text_align(menu_time_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(menu_time_lab, &Font_Mono_Bold_18, LV_PART_MAIN);
    // lv_obj_set_style_text_font(menu_time_lab, &lv_font_montserrat_26, LV_PART_MAIN);
    lv_label_set_recolor(menu_time_lab, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text_fmt(menu_time_lab, "#EE781F %02d# #C0C0C0 :# #D8E699 %02d#", hour, minute);

    ///////////////////////////////////////////////////////
    item_cont = lv_obj_create(parent);
    lv_obj_set_size(item_cont, DISPALY_WIDTH * MENU_PROPORTION, DISPALY_HEIGHT);
    lv_obj_set_align(item_cont, LV_ALIGN_RIGHT_MID);
    lv_obj_set_flex_flow(item_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_color(item_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_border_width(item_cont, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(item_cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(item_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(item_cont, true, 0);
    lv_obj_set_scroll_dir(item_cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(item_cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(item_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_row(item_cont, 10, LV_PART_MAIN);

    int buf_len = sizeof(name_buf)/sizeof(name_buf[0]);

    uint32_t i;
    for(i = 0; i < buf_len; i++) {
        lv_obj_t * btn = lv_btn_create(item_cont);
        lv_obj_set_width(btn, lv_pct(100));
        lv_obj_set_style_radius(btn, 10, LV_PART_MAIN);
        lv_obj_set_style_bg_color(btn, lv_color_hex(COLOR_BG), LV_PART_MAIN);
        lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);
        lv_obj_remove_style(btn, NULL, LV_STATE_FOCUS_KEY);
        lv_obj_set_style_outline_width(btn, 2, LV_STATE_FOCUS_KEY);
        lv_obj_set_style_outline_color(btn, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
        lv_obj_set_style_bg_img_opa(btn, LV_OPA_100, LV_PART_MAIN);
        lv_obj_add_event_cb(btn, scr0_btn_event_cb, LV_EVENT_CLICKED, (void *)i);
        lv_obj_add_event_cb(btn, scr0_btn_event_cb, LV_EVENT_FOCUSED, (void *)i);

        lv_obj_t * label = lv_label_create(btn);
        lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
        lv_obj_set_style_text_font(label, FONT_MENU_ITEM, LV_PART_MAIN);
        lv_label_set_text(label, name_buf[i]);
    }

    /*Update the buttons position manually for first*/
    lv_event_send(item_cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(item_cont, 0), LV_ANIM_OFF);

    lv_group_focus_obj(lv_obj_get_child(item_cont, fouce_item));

    lv_group_set_wrap(lv_group_get_default(), false);
}

void entry0(void) {   
    entry0_anim();
    lv_obj_add_event_cb(menu_time_lab, clock_upd_event, LV_EVENT_MSG_RECEIVED, NULL);
    lv_msg_subsribe_obj(MSG_CLOCK_HOUR, menu_time_lab, (void *)MSG_CLOCK_HOUR);
    lv_msg_subsribe_obj(MSG_CLOCK_MINUTE, menu_time_lab, (void *)MSG_CLOCK_MINUTE);
    lv_msg_subsribe_obj(MSG_CLOCK_SECOND, menu_time_lab, (void *)MSG_CLOCK_SECOND);
}
void exit0(void) {
    lv_obj_remove_event_cb(menu_time_lab, clock_upd_event);
    lv_msg_unsubscribe_obj(MSG_CLOCK_HOUR, menu_time_lab);
    lv_msg_unsubscribe_obj(MSG_CLOCK_MINUTE, menu_time_lab);
    lv_msg_unsubscribe_obj(MSG_CLOCK_SECOND, menu_time_lab);
}
void destroy0(void) {}

scr_lifecycle_t screen0 = {
    .create = create0,
    .entry = entry0,
    .exit  = exit0,
    .destroy = destroy0,
};
#endif
//************************************[ screen 1 ]****************************************** ws2812
#if 1
lv_obj_t *scr1_cont;
lv_obj_t *light_acr;
int ws2812_light = WS2812_DEFAULT_LIGHT;
lv_color_t ws2812_color ={.full = 0xF800};
static int ui_light_mode = 0;

void entry1_anim(lv_obj_t *obj);
void exit1_anim(int user_data, lv_obj_t *obj);

void colorwheel_focus_event(lv_event_t *e)
{
    lv_obj_t *tgt = lv_event_get_target(e);
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    if(e->code == LV_EVENT_CLICKED){
        if (!lv_obj_has_flag(tgt, LV_OBJ_FLAG_CHECKABLE)) {
            lv_obj_clear_flag(tgt, LV_OBJ_FLAG_CHECKABLE);
            lv_group_set_editing(lv_group_get_default(), false);
        } else {
            lv_obj_add_flag(tgt, LV_OBJ_FLAG_CHECKABLE);
            lv_group_set_editing(lv_group_get_default(), true);
        }
    }
    else if(e->code == LV_EVENT_VALUE_CHANGED){
        ws2812_color = lv_colorwheel_get_rgb(tgt);
        CRGB c;
        lv_color32_t c32;
        c32.full = lv_color_to32(ws2812_color);
        c.red = c32.ch.red;
        c.green = c32.ch.green;
        c.blue = c32.ch.blue;
        if(ws2812_get_mode() == 0){
            ws2812_set_color(c);
        }
        lv_label_set_text_fmt(label, "0x%02X%02X%02X", c.red, c.green, c.blue);
        lv_obj_set_style_bg_color(light_acr, ws2812_color, LV_PART_KNOB);
        lv_obj_set_style_arc_color(light_acr, ws2812_color, LV_PART_INDICATOR);
        // lv_msg_send(MSG_WS2812_COLOR, &colorwheel_buf);
        // printf("full=0x%x, r=0x%x, g=0x%x, b=0x%x\n",c.full, c.ch.red, c.ch.green, c.ch.blue);
    }
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target(e);
    lv_obj_t * label = (lv_obj_t *)lv_event_get_user_data(e);

    ws2812_light = lv_arc_get_value(arc);
    lv_label_set_text_fmt(label, "%d", ws2812_light);
    ws2812_set_light(ws2812_light);
    
    /*Rotate the label to the current position of the arc*/
    // lv_arc_rotate_obj_to_angle(arc, label, 25);
}

static void scr1_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        // scr_mgr_set_anim(LV_SCR_LOAD_ANIM_FADE_OUT, -1, -1);
        // scr_mgr_switch(SCREEN0_ID, false);
        exit1_anim(SCREEN0_ID, scr1_cont);
    }
}

static void ws2812_mode_event_cb(lv_event_t * e)
{
    lv_obj_t *tgt = (lv_obj_t *)e->target;
    lv_obj_t *lab = (lv_obj_t *)e->user_data;
    if(e->code == LV_EVENT_CLICKED){
        ui_light_mode++;
        ui_light_mode &= 0x3;
        switch (ui_light_mode)
        {
            case 0: lv_label_set_text(lab, " OFF ");    break;
            case 1: lv_label_set_text(lab, " demo1 "); break;
            case 2: lv_label_set_text(lab, " demo2 "); break;
            case 3: lv_label_set_text(lab, " demo3 "); break;
            default:
                break;
        }
        ws2812_set_mode(ui_light_mode);
        if(ui_light_mode == 0){
            vTaskSuspend(ws2812_handle);
        } else {
            vTaskResume(ws2812_handle);
        }
    }
}

void entry1_anim(lv_obj_t *obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_0, LV_OPA_100);
    lv_anim_set_time(&a, 200);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_exec_cb(&a, [](void * var, int32_t v){ 
        lv_port_indev_enabled(false);
        lv_obj_set_style_opa((lv_obj_t *)var, v, LV_PART_MAIN);
    });
    lv_anim_set_ready_cb(&a, [](_lv_anim_t *a){ 
        lv_port_indev_enabled(true);
    });
    lv_anim_start(&a);
}

void exit1_anim(int user_data, lv_obj_t *obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_100, LV_OPA_0);
    lv_anim_set_time(&a, 200);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_exec_cb(&a, [](void * var, int32_t v){ 
        lv_port_indev_enabled(false);
        lv_obj_set_style_opa((lv_obj_t *)var, v, LV_PART_MAIN);
    });
    lv_anim_set_ready_cb(&a, [](_lv_anim_t *a){
        scr_mgr_switch((int)a->user_data, false);
        lv_port_indev_enabled(true);
    });
    lv_anim_set_user_data(&a, (void *)user_data);
    lv_anim_start(&a);
}

void create1(lv_obj_t *parent)
{   
    scr1_cont = lv_obj_create(parent);
    lv_obj_set_size(scr1_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr1_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr1_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr1_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr1_cont, 0, LV_PART_MAIN);

    lv_obj_t *colorwheel = lv_colorwheel_create(scr1_cont, true);
    lv_obj_set_size(colorwheel, 100, 100);
    lv_obj_set_style_arc_width(colorwheel, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_width(colorwheel, 6, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(colorwheel, 6, LV_PART_KNOB);
    lv_obj_align(colorwheel, LV_ALIGN_RIGHT_MID, -35, 0);
    lv_colorwheel_set_rgb(colorwheel, ws2812_color);
    lv_obj_set_style_outline_pad(colorwheel, 4, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_radius(colorwheel, LV_RADIUS_CIRCLE, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(colorwheel, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(colorwheel, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_group_add_obj(lv_group_get_default(), colorwheel);
    lv_obj_t * label = lv_label_create(colorwheel);
    lv_obj_center(label);
    lv_label_set_text(label, "color");
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_add_event_cb(colorwheel, colorwheel_focus_event, LV_EVENT_CLICKED, label);
    lv_obj_add_event_cb(colorwheel, colorwheel_focus_event, LV_EVENT_VALUE_CHANGED, label);
    // lv_event_send(colorwheel, LV_EVENT_VALUE_CHANGED, NULL);

    light_acr = lv_arc_create(scr1_cont);
    lv_obj_set_size(light_acr, 100, 100);
    lv_obj_align(light_acr, LV_ALIGN_LEFT_MID, 35, 0);
    lv_obj_set_style_bg_color(light_acr, ws2812_color, LV_PART_KNOB);
    lv_obj_set_style_arc_color(light_acr, ws2812_color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(light_acr, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_width(light_acr, 10, LV_PART_INDICATOR);
    lv_arc_set_rotation(light_acr, 90);
    lv_arc_set_bg_angles(light_acr, 0, 360);
    lv_arc_set_range(light_acr, 0, 255);
    lv_arc_set_value(light_acr, ws2812_light);
    lv_obj_set_style_outline_pad(light_acr, 4, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_radius(light_acr, LV_RADIUS_CIRCLE, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(light_acr, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(light_acr, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_group_add_obj(lv_group_get_default(), light_acr);
    lv_obj_t * label1 = lv_label_create(light_acr);
    lv_obj_center(label1);
    lv_obj_set_style_text_color(label1, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_add_event_cb(light_acr, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label1);

    /*Manually update the label for the first time*/
    lv_event_send(light_acr, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * mode_btn = lv_btn_create(scr1_cont);
    // lv_group_add_obj(lv_group_get_default(), mode_btn);
    lv_obj_set_style_pad_all(mode_btn, 0, 0);
    lv_obj_set_height(mode_btn, 30);
    lv_obj_align(mode_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_border_color(mode_btn, lv_color_hex(COLOR_BORDER), LV_PART_MAIN);
    lv_obj_set_style_border_width(mode_btn, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(mode_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(mode_btn, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_remove_style(mode_btn, NULL, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_pad(mode_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(mode_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(mode_btn, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_obj_t * mode_lab = lv_label_create(mode_btn);
    lv_obj_set_style_text_color(mode_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(mode_lab, LV_ALIGN_LEFT_MID, 0, 0);
    if(ui_light_mode == 0) {
        lv_label_set_text(mode_lab, " OFF ");
    } else {
        lv_label_set_text_fmt(mode_lab, " demo%d ", ui_light_mode);
    }
    lv_obj_add_event_cb(mode_btn, ws2812_mode_event_cb, LV_EVENT_CLICKED, mode_lab);
    lv_event_send(mode_btn, LV_EVENT_VALUE_CHANGED, mode_lab);

    // back btn
    scr_back_btn_create(scr1_cont, scr1_btn_event_cb);
}
void entry1(void) {
    entry1_anim(scr1_cont);
    lv_group_set_wrap(lv_group_get_default(), true);
}
void exit1(void) {
    lv_group_set_wrap(lv_group_get_default(), false);
}
void destroy1(void) {}

scr_lifecycle_t screen1 = {
    .create = create1,
    .entry = entry1,
    .exit = exit1,
    .destroy = destroy1,
};
#endif
//************************************[ screen 2 ]****************************************** lora
#if 1
lv_obj_t *scr2_cont;
lv_obj_t *lora_kb;
lv_obj_t *lora_recv_ta;
lv_obj_t *lora_recv_lab;
lv_obj_t *lora_send_ta;
lv_obj_t *lora_send_lab;
lv_obj_t *lora_mode_btn;
lv_timer_t *lora_recv_timer;
int lora_recv_cnt = 0;
int lora_send_cnt = 0;

extern int lora_recv_success;
extern String lora_recv_str;

void entry2_anim(lv_obj_t *obj);
void exit2_anim(int user_data, lv_obj_t *obj);

void lora_recv_event(lv_timer_t *t)
{
    if(lora_recv_success) {
        lora_recv_success = 0;
        lora_recv_cnt++;
        lv_textarea_set_text(lora_recv_ta, " ");
        lv_label_set_text_fmt(lora_recv_lab, "R: %d", lora_recv_cnt);
        lv_textarea_add_text(lora_recv_ta, lora_recv_str.c_str());
    }
}

void mode_sw_event(lv_event_t *e)
{
    lv_obj_t *tgt =  (lv_obj_t *)e->target;
    lv_obj_t *data = (lv_obj_t *)e->user_data;
    
    if(e->code == LV_EVENT_CLICKED) {
        switch (lora_get_mode())
        {
            case LORA_MODE_SEND: lora_mode_sw(LORA_MODE_RECV); lv_label_set_text(data, "recv"); break;
            case LORA_MODE_RECV: lora_mode_sw(LORA_MODE_SEND); lv_label_set_text(data, "send"); break;
            default: break;
        }
    }
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
        /*Focus on the clicked text area*/
        if(lora_kb != NULL) lv_keyboard_set_textarea(lora_kb, ta);
    }

    else if(code == LV_EVENT_READY) {
        // LV_LOG_USER("Ready, current text: %s", lv_textarea_get_text(ta));
        uint32_t max_length = lv_textarea_get_cursor_pos(ta);
        if(max_length != 0){
            if(lora_get_mode() == LORA_MODE_SEND){
                lora_send(lv_textarea_get_text(ta));
            } else {
                Serial.println("Current Lora mode is RECV!");
            }
            // lv_textarea_set_text(ta, " "); // clear textarea
            Serial.printf("mode=%d, %s\n", lora_get_mode(), lv_textarea_get_text(ta));
            lora_send_cnt++;
            lv_label_set_text_fmt(lora_send_lab, "S: %d", lora_send_cnt);
        }
    }
}

static void scr2_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        exit2_anim(SCREEN0_ID, scr2_cont);
    }
}

void entry2_anim(lv_obj_t *obj)
{
    entry1_anim(obj);
}

void exit2_anim(int user_data, lv_obj_t *obj)
{
    exit1_anim(user_data, obj);
}

void create2(lv_obj_t *parent){
    scr2_cont = lv_obj_create(parent);
    lv_obj_set_size(scr2_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr2_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr2_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr2_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr2_cont, 0, LV_PART_MAIN);

    lora_recv_ta = lv_textarea_create(scr2_cont);
    lv_obj_set_size(lora_recv_ta, 150, 60);
    lv_obj_set_style_border_width(lora_recv_ta, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(lora_recv_ta, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(lora_recv_ta, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_text_color(lora_recv_ta, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_border_color(lora_recv_ta, lv_color_hex(COLOR_TEXT), LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_textarea_set_text(lora_recv_ta, "");
    lv_obj_align(lora_recv_ta, LV_ALIGN_TOP_LEFT, 6, 30);
    lv_textarea_set_placeholder_text(lora_recv_ta, "Hello");
    // lv_obj_add_event_cb(lora_recv_ta, ta_event_cb, LV_EVENT_ALL, NULL);
    lv_group_remove_obj(lora_recv_ta);

    /*Create a label and position it above the text box*/
    lora_recv_lab = lv_label_create(scr2_cont);
    lv_obj_set_style_text_color(lora_recv_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(lora_recv_lab, "R: %d", lora_recv_cnt);
    lv_obj_align_to(lora_recv_lab, lora_recv_ta, LV_ALIGN_OUT_TOP_MID, 0, -5);

    lora_mode_btn = lv_btn_create(scr2_cont);
    lv_obj_set_height(lora_mode_btn, 20);
    lv_obj_set_style_shadow_width(lora_mode_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(lora_mode_btn, 0, LV_PART_MAIN);
    lv_obj_remove_style(lora_mode_btn, NULL, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_pad(lora_mode_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(lora_mode_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(lora_mode_btn, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_obj_align(lora_mode_btn, LV_ALIGN_TOP_MID, 0, 6);
    lv_obj_t *mode_lab = lv_label_create(lora_mode_btn);
    lv_obj_center(mode_lab);
    switch (lora_get_mode())
    {
        case LORA_MODE_SEND: lv_label_set_text(mode_lab, "send"); break;
        case LORA_MODE_RECV: lv_label_set_text(mode_lab, "recv"); break;
        default: break;
    }
    lv_obj_add_event_cb(lora_mode_btn, mode_sw_event, LV_EVENT_CLICKED, mode_lab);


    /*Create the one-line mode text area*/
    lora_send_ta = lv_textarea_create(scr2_cont);
    lv_obj_set_size(lora_send_ta, 150, 60);
    lv_obj_set_style_border_width(lora_send_ta, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(lora_send_ta, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(lora_send_ta, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_text_color(lora_send_ta, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_border_color(lora_send_ta, lv_color_hex(COLOR_TEXT), LV_PART_CURSOR | LV_STATE_FOCUSED);
    lv_textarea_set_one_line(lora_send_ta, false);
    lv_textarea_set_password_mode(lora_send_ta, false);
    lv_obj_add_event_cb(lora_send_ta, ta_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(lora_send_ta, LV_ALIGN_TOP_RIGHT, -6, 30);
    lv_group_remove_obj(lora_send_ta);

    /*Create a label and position it above the text box*/
    lora_send_lab = lv_label_create(scr2_cont);
    lv_obj_set_style_text_color(lora_send_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_label_set_text_fmt(lora_send_lab, "S: %d", lora_send_cnt);
    lv_obj_align_to(lora_send_lab, lora_send_ta, LV_ALIGN_OUT_TOP_MID, 0, -5);

    /*Create a keyboard*/
    lora_kb = lv_keyboard_create(scr2_cont);
    lv_obj_set_style_border_width(lora_kb, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(lora_kb, lv_color_hex(COLOR_BORDER), LV_PART_MAIN);
    lv_obj_remove_style(lora_kb, NULL, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(lora_kb, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(lora_kb, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(lora_kb, lv_color_hex(COLOR_FOCUS_ON), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(lora_kb, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_bg_color(lora_kb, lv_color_hex(COLOR_BG), LV_PART_ITEMS);
    lv_obj_set_style_bg_color(lora_kb, lv_color_hex(COLOR_BG), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(lora_kb, lv_color_hex(COLOR_TEXT), LV_PART_ITEMS);
    lv_obj_set_style_text_color(lora_kb, lv_color_hex(COLOR_TEXT), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_size(lora_kb,  LV_HOR_RES, LV_VER_RES / 2 - 10);
    lv_keyboard_set_textarea(lora_kb, lora_send_ta); /*Focus it on one of the text areas to start*/

    lv_event_send(lora_send_ta, LV_EVENT_FOCUSED, NULL);

    // back btn
    scr_back_btn_create(scr2_cont, scr2_btn_event_cb);
}
void entry2(void) {
    entry2_anim(scr2_cont);
    lv_group_set_wrap(lv_group_get_default(), true);
    lora_recv_timer = lv_timer_create(lora_recv_event, 50, NULL);
    vTaskResume(lora_handle);
}
void exit2(void) {
    lv_timer_del(lora_recv_timer);
    lora_recv_timer = NULL;
    lv_group_set_wrap(lv_group_get_default(), false);
    vTaskSuspend(lora_handle);
}
void destroy2(void) {
}
scr_lifecycle_t screen2 = {
    .create = create2,
    .entry = entry2,
    .exit = exit2,
    .destroy = destroy2,
};
#endif
//************************************[ screen 3 ]****************************************** nfc
#if 1
extern bool nfc_upd_falg;
extern uint32_t cardid;
extern uint8_t uid[];
lv_obj_t *scr3_cont;
lv_obj_t *nfc_list;
lv_obj_t *nfc_led;
lv_obj_t *nfc_ledlab;
lv_timer_t *nfc_chk_timer = NULL;
uint32_t nfc_id[10] = {0};
uint32_t nfc_recode_cnt = 0;

void list_item_event(lv_event_t *e)
{

}

void nfc_chk_timer_event(lv_timer_t *t)
{
    int is_exist = -1;
    static int led_flag = 0;
    if(nfc_upd_falg) {
        nfc_upd_falg = false;

        for(int i = 0; i < sizeof(nfc_id) / sizeof(nfc_id[0]); i++) {
            if(nfc_id[i] == cardid) {
                is_exist = i;
                break;
            }
        }

        if(is_exist != -1) {
            lv_label_set_text_fmt(nfc_ledlab, "%02d -", is_exist+1);
            if(led_flag){
                lv_led_on(nfc_led);
            }else{
                lv_led_off(nfc_led);
            }
            led_flag = !led_flag;
        } else {
            char buf[33];
            nfc_id[nfc_recode_cnt] = cardid;
            nfc_recode_cnt++;
            
            lv_snprintf(buf, 33, "%02d-UID-[%02X:%02X:%02X:%02X]-#%d", 
                nfc_recode_cnt, uid[0], uid[1], uid[2], uid[3], cardid);
            lv_obj_t *item = lv_list_add_btn(nfc_list, NULL, buf);
            lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
            lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_BG), LV_PART_MAIN);
            lv_obj_set_style_text_color(item, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
            lv_obj_set_style_text_font(item, FONT_MONO_BOLD, LV_PART_MAIN);
        }
    } 
    else {
        lv_led_off(nfc_led);
        lv_label_set_text(nfc_ledlab, "xx -");
    }
}

void entry3_anim(lv_obj_t *obj)
{
    entry1_anim(obj);
}

void exit3_anim(int user_data, lv_obj_t *obj)
{
    exit1_anim(user_data, obj);
}

static void scr3_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        // scr_mgr_set_anim(LV_SCR_LOAD_ANIM_FADE_OUT, -1, -1);
        // scr_mgr_switch(SCREEN0_ID, false);
        exit3_anim(SCREEN0_ID, scr3_cont);
    }
}

void create3(lv_obj_t *parent){
    scr3_cont = lv_obj_create(parent);
    lv_obj_set_size(scr3_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr3_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr3_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr3_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr3_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr3_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    uint32_t verisondata = nfc_get_ver_data();
    uint32_t chip = (verisondata >> 24) & 0xFF;
    uint32_t verH = (verisondata >> 16) & 0xFF;
    uint32_t verL = (verisondata >>  8) & 0xFF;
    lv_label_set_text_fmt(label, "PN5%x  ver:%d.%d", chip, verH, verL);

    nfc_led  = lv_led_create(scr3_cont);
    lv_obj_set_size(nfc_led, 16, 16);
    lv_obj_align(nfc_led, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_led_off(nfc_led);

    nfc_ledlab = lv_label_create(scr3_cont);
    lv_obj_set_style_text_color(nfc_ledlab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(nfc_ledlab, FONT_MONO_LIGHT, LV_PART_MAIN);
    lv_obj_align_to(nfc_ledlab, nfc_led, LV_ALIGN_OUT_LEFT_MID, -6, 0);
    lv_label_set_text(nfc_ledlab, "xx -");

    nfc_list = lv_list_create(scr3_cont);
    lv_obj_set_size(nfc_list, LV_HOR_RES, 135);
    lv_obj_align(nfc_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(nfc_list, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_pad_row(nfc_list, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(nfc_list, 0, LV_PART_MAIN);
    // lv_obj_set_style_outline_pad(nfc_list, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(nfc_list, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(nfc_list, 0, LV_PART_MAIN);

    for(int i = 0; i < nfc_recode_cnt; i++) {
        char buf[33];
        lv_snprintf(buf, 33, "%02d-UID-[%02X:%02X:%02X:%02X]-#%d", 
                i + 1, 
                (nfc_id[i] >> 24) & 0xFF, 
                (nfc_id[i] >> 16) & 0xFF, 
                (nfc_id[i] >> 8)  & 0xFF, 
                nfc_id[i] & 0xFF, 
                nfc_id[i]);
        lv_obj_t *item = lv_list_add_btn(nfc_list, NULL, buf);
        lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
        lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_BG), LV_PART_MAIN);
        lv_obj_set_style_text_color(item, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
        lv_obj_set_style_text_font(item, FONT_MONO_BOLD, LV_PART_MAIN);
        // lv_obj_add_event_cb(item, list_item_event, LV_EVENT_CLICKED, (void *)i);
    }

    scr_back_btn_create(scr3_cont, scr3_btn_event_cb);
}
void entry3(void) {
    entry3_anim(scr3_cont);
    nfc_chk_timer = lv_timer_create(nfc_chk_timer_event, 100, NULL);
    vTaskResume(nfc_handle);
}
void exit3(void) {
    lv_timer_del(nfc_chk_timer);
    nfc_chk_timer = NULL;
    vTaskSuspend(nfc_handle);
}
void destroy3(void) {
}
scr_lifecycle_t screen3 = {
    .create = create3,
    .entry = entry3,
    .exit = exit3,
    .destroy = destroy3,
};
#endif
//************************************[ screen 4 ]****************************************** setting
#if 1
lv_obj_t *scr4_cont;
lv_obj_t *setting_list;
lv_obj_t *theme_label;

void setting_scr_event(lv_event_t *e)
{
    lv_obj_t *tgt = (lv_obj_t *)e->target;
    int data = (int)e->user_data;

    if(e->code == LV_EVENT_CLICKED) {
        switch (data)
        {
        case 0: // "Rotatoion"
            if(display_rotation == 3){
                // tft.fillScreen(TFT_BLACK);
                tft.setRotation(1);
                display_rotation = 1;
            } else if(display_rotation == 1){
                // tft.fillScreen(TFT_BLACK);
                tft.setRotation(3);
                display_rotation = 3;
            }
            eeprom_wr(UI_ROTATION_EEPROM_ADDR, display_rotation);
            break;
        case 1: // "Deep Sleep"
            ws2812_set_light(0);
            esp_sleep_enable_ext0_wakeup((gpio_num_t)ENCODER_KEY, 0);
            esp_deep_sleep_start();
            break;
        case 2: // "UI Theme"
            if(setting_theme == UI_THEME_DARK) {
                setting_theme = UI_THEME_LIGHT;
                lv_label_set_text(theme_label, "LIGHT");
            } else if(setting_theme == UI_THEME_LIGHT) {
                setting_theme = UI_THEME_DARK;
                lv_label_set_text(theme_label, "DARK");
            }
            ui_theme_setting(setting_theme);
            eeprom_wr(UI_THEME_EEPROM_ADDR, setting_theme);
            break;
        default:
            break;
        }
    }
}

void entry4_anim(lv_obj_t *obj)
{
    entry1_anim(obj);
}

void exit4_anim(int user_data, lv_obj_t *obj)
{
    exit1_anim(user_data, obj);
}

static void scr4_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        // scr_mgr_set_anim(LV_SCR_LOAD_ANIM_FADE_OUT, -1, -1);
        // scr_mgr_switch(SCREEN0_ID, false);
        exit4_anim(SCREEN0_ID, scr4_cont);
    }
}

void create4(lv_obj_t *parent){
    scr4_cont = lv_obj_create(parent);
    lv_obj_set_size(scr4_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr4_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr4_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr4_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr4_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr4_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(label, "Setting");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    setting_list = lv_list_create(scr4_cont);
    lv_obj_set_size(setting_list, LV_HOR_RES, 135);
    lv_obj_align(setting_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(setting_list, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_pad_row(setting_list, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(setting_list, 0, LV_PART_MAIN);
    // lv_obj_set_style_outline_pad(setting_list, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(setting_list, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(setting_list, 0, LV_PART_MAIN);

    lv_obj_t *setting1 = lv_list_add_btn(setting_list, LV_SYMBOL_SETTINGS, "Rotatoion");
    lv_obj_t *setting2 = lv_list_add_btn(setting_list, LV_SYMBOL_SETTINGS, "Deep Sleep");
    lv_obj_t *setting3 = lv_list_add_btn(setting_list, LV_SYMBOL_SETTINGS, "UI Theme");

    for(int i = 0; i < lv_obj_get_child_cnt(setting_list); i++) {
        lv_obj_t *item = lv_obj_get_child(setting_list, i);
        // lv_obj_set_style_text_font(item, &Font_Mono_Bold_16, LV_PART_MAIN);
        lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
        lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_BG), LV_PART_MAIN);
        lv_obj_set_style_text_color(item, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
        lv_obj_add_event_cb(item, setting_scr_event, LV_EVENT_CLICKED, (void *)i);
    }

    theme_label = lv_label_create(setting3);
    lv_obj_align(theme_label, LV_ALIGN_RIGHT_MID, 0, 0);
    switch (setting_theme)
    {
        case UI_THEME_DARK:  lv_label_set_text(theme_label, "DARK");  break;
        case UI_THEME_LIGHT: lv_label_set_text(theme_label, "LIGHT"); break;
        default:
            break;
    }
    
    scr_back_btn_create(scr4_cont, scr4_btn_event_cb);
}
void entry4(void) {
    entry4_anim(scr4_cont);
}
void exit4(void) {
}
void destroy4(void) {
}
scr_lifecycle_t screen4 = {
    .create = create4,
    .entry = entry4,
    .exit = exit4,
    .destroy = destroy4,
};
#endif
//************************************[ screen 5 ]****************************************** battery
#if 1

#define BAT_INFO_LIEN_CHAR 30
#define BAT_INFO_LIEN_NUM  7

lv_obj_t *scr5_cont;
lv_obj_t *batt_cont;
lv_obj_t *batt_line[BAT_INFO_LIEN_NUM];
lv_timer_t *batt_timer;

void scr5_add_info_lab(const char *s);

void batt_timer_event(lv_timer_t *t)
{
    lv_label_set_text_fmt(batt_line[0], "VBUS --- %3.2f | VSYS --- %3.2f", battery_charging.getVBUS(), battery_charging.getVSYS());
    lv_label_set_text_fmt(batt_line[1], "VBAT --- %3.2f | ICHG --- %3.2f", battery_charging.getVBAT(), battery_charging.getICHG());
    lv_label_set_text_fmt(batt_line[2], "TSPCT - %4.2f | Tempe - %4.2f", battery_charging.getTSPCT(), battery_charging.getTemperature());
    lv_label_set_text_fmt(batt_line[3], "Charging Status ----------- %d", battery_charging.getCHG_STATUS());
    lv_label_set_text_fmt(batt_line[4], "VBUS Status --------------- %d", battery_charging.getVBUS_STATUS());
    lv_label_set_text_fmt(batt_line[5], "VSYS Status --------------- %d", battery_charging.getVSYS_STATUS());
    lv_label_set_text_fmt(batt_line[6], "Charger err --------------- %d", battery_charging.getCHG_Fault_STATUS());
}

void entry5_anim(lv_obj_t *obj)
{
    entry1_anim(obj);
}

void exit5_anim(int user_data, lv_obj_t *obj)
{
    exit1_anim(user_data, obj);
}

static void scr5_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        // scr_mgr_set_anim(LV_SCR_LOAD_ANIM_FADE_OUT, -1, -1);
        // scr_mgr_switch(SCREEN0_ID, false);
        exit5_anim(SCREEN0_ID, scr5_cont);
    }
}

lv_obj_t * scr5_add_info_lab(lv_obj_t *label, const char *s)
{
    label = lv_label_create(batt_cont);
    lv_obj_set_width(label, DISPALY_WIDTH);
    lv_obj_set_style_bg_color(label, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_bg_color(label, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(label, LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_text_font(label, &Font_Mono_Bold_16, LV_PART_MAIN);
    lv_label_set_text(label, s);
    // lv_group_add_obj(lv_group_get_default(), label);
    lv_obj_center(label);
    return label;
}

void create5(lv_obj_t *parent) 
{
    scr5_cont = lv_obj_create(parent);
    lv_obj_set_size(scr5_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr5_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr5_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr5_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr5_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr5_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(label, "Battery");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    batt_cont = lv_obj_create(scr5_cont);
    lv_obj_set_size(batt_cont, DISPALY_WIDTH, 140);
    lv_obj_align(batt_cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_radius(batt_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(batt_cont, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(batt_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(batt_cont, 1, LV_PART_MAIN);
    lv_obj_set_style_border_width(batt_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(batt_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(batt_cont, LV_SCROLLBAR_MODE_OFF);
    
    lv_obj_t *label1; // init label style
    for(int i = 0; i < BAT_INFO_LIEN_NUM; i++) {
        batt_line[i] = scr5_add_info_lab(label1, " ");
    }

    // back btn
    scr_back_btn_create(scr5_cont, scr5_btn_event_cb);
}
void entry5(void) {   
    entry5_anim(scr5_cont);
    batt_timer = lv_timer_create(batt_timer_event, 500, NULL);
    vTaskResume(battery_handle);
}
void exit5(void) {
    lv_timer_del(batt_timer);
    batt_timer = NULL;
    vTaskSuspend(battery_handle);
}
void destroy5(void) { 
}

scr_lifecycle_t screen5 = {
    .create = create5,
    .entry = entry5,
    .exit  = exit5,
    .destroy = destroy5,
};
#endif
//************************************[ screen 6 ]****************************************** wifi
#if 1
lv_obj_t *scr6_cont;
lv_obj_t *wifi_config_btn;
lv_obj_t *wifi_help_btn;
lv_obj_t *wifi_st_lab = NULL;
lv_obj_t *ip_lab;
lv_obj_t *ssid_lab;
lv_obj_t *pwd_lab;
lv_obj_t *config_state_lab;

static volatile bool smartConfigStart      = false;
static lv_timer_t   *wifi_timer            = NULL;
static uint32_t      wifi_timer_counter    = 0;
static uint32_t      wifi_connnect_timeout = 60;

void wifi_info_label_create(void);

void wifi_help_event(lv_event_t *e)
{
    if(e->code == LV_EVENT_CLICKED) {
        prompt_info(" You need to download 'EspTouch' to configure WiFi.", 3000);
    }
}

static void wifi_config_event_handler(lv_event_t *e)
{
    static int step = 0;
    lv_event_code_t code  = lv_event_get_code(e);

    if(code != LV_EVENT_CLICKED) {
        return;
    }

    if(wifi_is_connect){
        prompt_info("WiFi is connected do not need to configure WiFi.", 2000);
        return;
    }

    if (smartConfigStart) {
        lv_label_set_text(config_state_lab, "Config Stop");
        if (wifi_timer) {
            lv_timer_del(wifi_timer);
            wifi_timer = NULL;
        }
        WiFi.stopSmartConfig();
        Serial.println("return smart Config has Start;");
        smartConfigStart = false;
        return;
    }
    WiFi.disconnect();
    smartConfigStart = true;
    WiFi.beginSmartConfig();
    lv_label_set_text(config_state_lab, "Config Start  ");

    wifi_timer = lv_timer_create([](lv_timer_t *t) {
        bool      destory = false;
        wifi_timer_counter++;
        if (wifi_timer_counter > wifi_connnect_timeout && !WiFi.isConnected()) {
            Serial.println("Connect timeout!");
            destory = true;
            lv_label_set_text(config_state_lab, "Time Out");
        } else {
            switch (step)
            {
                case 0: lv_label_set_text(config_state_lab, "Config Start -"); break;
                case 1: lv_label_set_text(config_state_lab, "Config Start /"); break;
                case 2: lv_label_set_text(config_state_lab, "Config Start -"); break;
                case 3: lv_label_set_text(config_state_lab, "Config Start \\"); break;
                default:
                    break;
            }
            step++;
            step &= 0x3;
        }
        if (WiFi.isConnected()) {
            Serial.println("WiFi has connected!");
            Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
            Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());

            if(strcmp(wifi_ssid, WiFi.SSID().c_str()) == 0) {
                Serial.printf("SSID == CURR SSID\r\n");
            }
            if(strcmp(wifi_password, WiFi.psk().c_str()) == 0) {
                Serial.printf("PSW == CURR PSW\r\n");
            }
            
            String ssid = WiFi.SSID();
            String pwsd = WiFi.psk();
            if(strcmp(wifi_ssid, ssid.c_str()) != 0 ||
               strcmp(wifi_password, pwsd.c_str()) != 0) {
                memcpy(wifi_ssid, ssid.c_str(), WIFI_SSID_MAX_LEN);
                memcpy(wifi_password, pwsd.c_str(), WIFI_PSWD_MAX_LEN);
                eeprom_wr_wifi(ssid.c_str(), ssid.length(), pwsd.c_str(), pwsd.length());
            }

            destory   = true;
            String IP = WiFi.localIP().toString();
            wifi_is_connect = true;
            lv_label_set_text(config_state_lab, "WiFi has connected!");

            lv_label_set_text(wifi_st_lab, (wifi_is_connect == true ? "Wifi Connect" : "Wifi Disconnect"));
            wifi_info_label_create();
        }
        if (destory) {
            WiFi.stopSmartConfig();
            smartConfigStart = false;
            lv_timer_del(wifi_timer);
            wifi_timer         = NULL;
            wifi_timer_counter = 0;
        }
        // Every seconds check conected
    },
    1000, NULL);
}

void entry6_anim(lv_obj_t *obj)
{
    entry1_anim(obj);
}

void exit6_anim(int user_data, lv_obj_t *obj)
{
    exit1_anim(user_data, obj);
}

static void scr6_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        // scr_mgr_set_anim(LV_SCR_LOAD_ANIM_FADE_OUT, -1, -1);
        // scr_mgr_switch(SCREEN0_ID, false);
        exit6_anim(SCREEN0_ID, scr6_cont);
    }
}

void wifi_info_label_create(void)
{   
    ip_lab = lv_label_create(scr6_cont);
    lv_obj_set_style_text_color(ip_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(ip_lab, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text_fmt(ip_lab, "ip: %s", WiFi.localIP().toString());
    lv_obj_align_to(ip_lab, wifi_st_lab, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    ssid_lab = lv_label_create(scr6_cont);
    lv_obj_set_style_text_color(ssid_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(ssid_lab, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text_fmt(ssid_lab, "ssid: %s", wifi_ssid);
    lv_obj_align_to(ssid_lab, ip_lab, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    pwd_lab = lv_label_create(scr6_cont);
    lv_obj_set_style_text_color(pwd_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(pwd_lab, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text_fmt(pwd_lab, "pswd: %s", wifi_password);
    lv_obj_align_to(pwd_lab, ssid_lab, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}

void create6(lv_obj_t *parent) 
{
    scr6_cont = lv_obj_create(parent);
    lv_obj_set_size(scr6_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr6_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr6_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr6_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr6_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr6_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(label, "Wifi config");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    wifi_st_lab = lv_label_create(scr6_cont);
    lv_obj_set_style_text_color(wifi_st_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(wifi_st_lab, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(wifi_st_lab, (wifi_is_connect == true ? "Wifi Connect" : "Wifi Disconnect"));
    lv_obj_align(wifi_st_lab, LV_ALIGN_TOP_LEFT, 5, 40);

    if(wifi_is_connect) {
        wifi_info_label_create();
    }

    wifi_config_btn = lv_btn_create(scr6_cont);
    lv_obj_set_size(wifi_config_btn, 100, 40);
    lv_obj_set_style_border_width(wifi_config_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(wifi_config_btn, 0, LV_PART_MAIN);
    lv_obj_remove_style(wifi_config_btn, NULL, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_pad(wifi_config_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(wifi_config_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(wifi_config_btn, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_obj_align(wifi_config_btn, LV_ALIGN_TOP_RIGHT, -20 , 40);
    lv_obj_t *config_lab = lv_label_create(wifi_config_btn);
    lv_obj_center(config_lab);
    lv_obj_set_style_text_color(config_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(config_lab, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(config_lab, "config");
    lv_obj_add_event_cb(wifi_config_btn, wifi_config_event_handler, LV_EVENT_CLICKED, NULL);

    wifi_help_btn = lv_btn_create(scr6_cont);
    lv_obj_set_size(wifi_help_btn, 100, 40);
    lv_obj_set_style_border_width(wifi_help_btn, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(wifi_help_btn, 0, LV_PART_MAIN);
    lv_obj_remove_style(wifi_help_btn, NULL, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_pad(wifi_help_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(wifi_help_btn, 2, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_color(wifi_help_btn, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
    lv_obj_align(wifi_help_btn, LV_ALIGN_BOTTOM_RIGHT, -20 , -40);
    lv_obj_t *help_lab = lv_label_create(wifi_help_btn);
    lv_obj_center(help_lab);
    lv_obj_set_style_text_color(help_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(help_lab, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(help_lab, "help");
    lv_obj_add_event_cb(wifi_help_btn, wifi_help_event, LV_EVENT_CLICKED, NULL);

    config_state_lab = lv_label_create(scr6_cont);
    lv_obj_align(config_state_lab, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_text_color(config_state_lab, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(config_state_lab, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(config_state_lab, " ");

    // back btn
    scr_back_btn_create(scr6_cont, scr6_btn_event_cb);
}
void entry6(void) {
    entry6_anim(scr6_cont);
}
void exit6(void) {}
void destroy6(void) {}

scr_lifecycle_t screen6 = {
    .create = create6,
    .entry = entry6,
    .exit  = exit6,
    .destroy = destroy6,
};
#endif
//************************************[ screen 7 ]****************************************** other(IR、MIC、SD)
// --------------------- screen 7.1 --------------------- IR
#if 1
lv_obj_t *scr7_1_cont;

void entry7_1_anim(lv_obj_t *obj) { entry1_anim(obj); }
void exit7_1_anim(int user_data, lv_obj_t *obj) { exit1_anim(user_data, obj); }

static void scr7_1_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        exit7_1_anim(SCREEN7_ID, scr7_1_cont);
    }
}

void create7_1(lv_obj_t *parent) 
{   
    scr7_1_cont = lv_obj_create(parent);
    lv_obj_set_size(scr7_1_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr7_1_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr7_1_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr7_1_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr7_1_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr7_1_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(label, "Infrared");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    // back bottom
    scr_back_btn_create(scr7_1_cont, scr7_1_btn_event_cb);
}
void entry7_1(void) {
    entry7_1_anim(scr7_1_cont);
}
void exit7_1(void) {}
void destroy7_1(void) {}

scr_lifecycle_t screen7_1 = {
    .create = create7_1,
    .entry = entry7_1,
    .exit  = exit7_1,
    .destroy = destroy7_1,
};
#endif
// --------------------- screen 7.2 --------------------- MIC
#if 1
lv_obj_t *scr7_2_cont;
void entry7_2_anim(lv_obj_t *obj) { entry1_anim(obj); }
void exit7_2_anim(int user_data, lv_obj_t *obj) { exit1_anim(user_data, obj); }

static void scr7_2_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        exit7_2_anim(SCREEN7_ID, scr7_2_cont);
    }
}

void create7_2(lv_obj_t *parent) 
{   
    scr7_2_cont = lv_obj_create(parent);
    lv_obj_set_size(scr7_2_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr7_2_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr7_2_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr7_2_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr7_2_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr7_2_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(label, "Microphone");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    // back bottom
    scr_back_btn_create(scr7_2_cont, scr7_2_btn_event_cb);
}
void entry7_2(void) {
    entry7_2_anim(scr7_2_cont);
}
void exit7_2(void) {}
void destroy7_2(void) {}

scr_lifecycle_t screen7_2 = {
    .create = create7_2,
    .entry = entry7_2,
    .exit  = exit7_2,
    .destroy = destroy7_2,
};
#endif
// --------------------- screen 7.3 --------------------- SD
#if 1

lv_obj_t *scr7_3_cont;

void entry7_3_anim(lv_obj_t *obj) { entry1_anim(obj); }
void exit7_3_anim(int user_data, lv_obj_t *obj) { exit1_anim(user_data, obj); }

static void scr7_3_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        exit7_3_anim(SCREEN7_ID, scr7_3_cont);
    }
}

void create7_3(lv_obj_t *parent) 
{
    scr7_3_cont = lv_obj_create(parent);
    lv_obj_set_size(scr7_3_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr7_3_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr7_3_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr7_3_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr7_3_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr7_3_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(label, "TF Card");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    // back bottom
    scr_back_btn_create(scr7_3_cont, scr7_3_btn_event_cb);
}
void entry7_3(void) {
    entry7_3_anim(scr7_3_cont);
}
void exit7_3(void) {}
void destroy7_3(void) {}

scr_lifecycle_t screen7_3 = {
    .create = create7_3,
    .entry = entry7_3,
    .exit  = exit7_3,
    .destroy = destroy7_3,
};
#endif
// --------------------- screen 7 -----------------------
#if 1
lv_obj_t *scr7_cont;
lv_obj_t *other_list;

void entry7_anim(lv_obj_t *obj) { entry1_anim(obj); }
void exit7_anim(int user_data, lv_obj_t *obj) { exit1_anim(user_data, obj); }

void other_list_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    int user_data = (int)e->user_data;
    if(code == LV_EVENT_CLICKED) {
        switch (user_data)
        {
            case 0: // Infrared
                scr_mgr_switch(SCREEN7_1_ID, false);
                prompt_info("IR underdevelopment", 1000);
                break;
            case 1: // Microphone
                scr_mgr_switch(SCREEN7_2_ID, false);
                prompt_info("MIC underdevelopment", 1000);
                break;
            case 2: // TF Card
                scr_mgr_switch(SCREEN7_3_ID, false);
                prompt_info("SD underdevelopment", 1000);
                break;
            default:
                break;
        }
    }
}

static void scr7_btn_event_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED){
        // scr_mgr_set_anim(LV_SCR_LOAD_ANIM_FADE_OUT, -1, -1);
        // scr_mgr_switch(SCREEN0_ID, false);
        exit7_anim(SCREEN0_ID, scr7_cont);
    }
}

void create7(lv_obj_t *parent) 
{
    scr7_cont = lv_obj_create(parent);
    lv_obj_set_size(scr7_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(scr7_cont, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr7_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_border_width(scr7_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scr7_cont, 0, LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(scr7_cont);
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, FONT_MONO_BOLD, LV_PART_MAIN);
    lv_label_set_text(label, "Other device");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    other_list = lv_list_create(scr7_cont);
    lv_obj_set_size(other_list, LV_HOR_RES, 135);
    lv_obj_align(other_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(other_list, lv_color_hex(COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_pad_row(other_list, 2, LV_PART_MAIN);
    lv_obj_set_style_radius(other_list, 0, LV_PART_MAIN);
    // lv_obj_set_style_outline_pad(other_list, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(other_list, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(other_list, 0, LV_PART_MAIN);

    lv_obj_t *setting1 = lv_list_add_btn(other_list, NULL, " - Infrared");
    lv_obj_t *setting2 = lv_list_add_btn(other_list, NULL, " - Microphone");
    lv_obj_t *setting3 = lv_list_add_btn(other_list, NULL, " - TF Card");

    for(int i = 0; i < lv_obj_get_child_cnt(other_list); i++) {
        lv_obj_t *item = lv_obj_get_child(other_list, i);
        lv_obj_set_style_text_font(item, FONT_MONO_BOLD, LV_PART_MAIN);
        lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_FOCUS_ON), LV_STATE_FOCUS_KEY);
        lv_obj_set_style_bg_color(item, lv_color_hex(COLOR_BG), LV_PART_MAIN);
        lv_obj_set_style_text_color(item, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
        lv_obj_add_event_cb(item, other_list_event, LV_EVENT_CLICKED, (void *)i);
    }

    // back btn
    scr_back_btn_create(scr7_cont, scr7_btn_event_cb);
}
void entry7(void) { 
    entry7_anim(scr7_cont);
}
void exit7(void) {   }
void destroy7(void) { 
}

scr_lifecycle_t screen7 = {
    .create = create7,
    .entry = entry7,
    .exit  = exit7,
    .destroy = destroy7,
};
#endif
//************************************[ UI ENTRY ]******************************************
void ui_entry(void)
{
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    scr_mgr_init();
    // scr_mgr_set_bg_color(COLOR_BG);
    ui_theme_setting(setting_theme);
    scr_mgr_register(SCREEN0_ID, &screen0);     // menu
    scr_mgr_register(SCREEN1_ID, &screen1);     // ws2812
    scr_mgr_register(SCREEN2_ID, &screen2);     // lora
    scr_mgr_register(SCREEN3_ID, &screen3);     // nfc
    scr_mgr_register(SCREEN4_ID, &screen4);     // setting
    scr_mgr_register(SCREEN5_ID, &screen5);     // battery
    scr_mgr_register(SCREEN6_ID, &screen6);     // wifi
    scr_mgr_register(SCREEN7_ID, &screen7);     // other
    scr_mgr_register(SCREEN7_1_ID, &screen7_1); //   -IR
    scr_mgr_register(SCREEN7_2_ID, &screen7_2); //   -MIC
    scr_mgr_register(SCREEN7_3_ID, &screen7_3); //   -TF Card

    scr_mgr_switch(SCREEN0_ID, false); // main scr
}