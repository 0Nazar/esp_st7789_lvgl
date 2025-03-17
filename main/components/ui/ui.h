// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.1
// LVGL version: 8.3.11
// Project name: SquareLine_Project

#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"


// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t * ui_Screen1;
void ui_event_NEXT(lv_event_t * e);
extern lv_obj_t * ui_NEXT;
void ui_event_Button3(lv_event_t * e);
extern lv_obj_t * ui_Button3;
void ui_event_Button5(lv_event_t * e);
extern lv_obj_t * ui_Button5;
extern lv_obj_t * ui_Label2;
extern lv_obj_t * ui_Label4;
extern lv_obj_t * ui_Label1;
// CUSTOM VARIABLES
extern lv_obj_t * uic_Screen1;
extern lv_obj_t * uic_NEXT;

// SCREEN: ui_Screen2
void ui_Screen2_screen_init(void);
extern lv_obj_t * ui_Screen2;
void ui_event_BACK(lv_event_t * e);
extern lv_obj_t * ui_BACK;
void ui_event_Button4(lv_event_t * e);
extern lv_obj_t * ui_Button4;
void ui_event_Button6(lv_event_t * e);
extern lv_obj_t * ui_Button6;
extern lv_obj_t * ui_Label5;
extern lv_obj_t * ui_LABEL;
extern lv_obj_t * ui_Label3;
// CUSTOM VARIABLES
extern lv_obj_t * uic_Screen2;
extern lv_obj_t * uic_BACK;
extern lv_obj_t * uic_LABEL;

// EVENTS

extern lv_obj_t * ui____initial_actions0;

// UI INIT
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
