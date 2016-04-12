/*
 * Copyright (C) 2014 Samsung Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "InputPicker.h"

#include "ewk_view_private.h"
#include <Ecore_X.h>
#include <Elementary.h>
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
#include <efl_extension.h>
#endif
#include <string.h>
#include <vconf/vconf.h>

#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
#include <efl_assist.h>
#endif

#if ENABLE(TIZEN_INPUT_DATE_TIME)
namespace WebKit {
static const unsigned maxDatetimeLength = 32;

struct dateTimeValue {
    char year[maxDatetimeLength];
    char mon[maxDatetimeLength];
    char day[maxDatetimeLength];
    char hour[maxDatetimeLength];
    char min[maxDatetimeLength];
    char sec[maxDatetimeLength];
};

static char* datetimeFormatGet()
{
    setenv("LANGUAGE", "en_US", 1);

    int timeValue = 0;
    int ret = vconf_get_int(VCONFKEY_REGIONFORMAT_TIME1224, &timeValue);
    int is24Hour = 0;
    if (ret < 0)
        is24Hour = 0;
    else if (timeValue == VCONFKEY_TIME_FORMAT_12 || timeValue == VCONFKEY_TIME_FORMAT_24)
        is24Hour = timeValue - 1;

    char buffer[256] = {0};
    char* regionFormat = vconf_get_str(VCONFKEY_REGIONFORMAT);
    if (is24Hour)
        snprintf(buffer, sizeof(buffer), "%s_DTFMT_24HR", regionFormat);
    else
        snprintf(buffer, sizeof(buffer), "%s_DTFMT_12HR", regionFormat);

    char* datetimeFormat = dgettext("dt_fmt", buffer);
    char* language = getenv("LANGUAGE");
    if (!language || !strcmp(language, ""))
        unsetenv("LANGUAGE");
    else
        setenv("LANGUAGE", language, 1);

    return strdup(datetimeFormat);
}

static void vconfDatetimeFormatChangedCallback(keynode_t* /* node */, void* data)
{
    Evas_Object* datetime = static_cast<Evas_Object*>(data);
    char* datetimeFormat = datetimeFormatGet();
    elm_datetime_format_set(datetime, datetimeFormat);
    free(datetimeFormat);
}

InputPicker::InputPicker(Evas_Object* ewkView)
    : m_ewkView(ewkView)
    , m_layout(0)
    , m_datetime(0)
{
}

InputPicker::~InputPicker()
{
    if (m_layout) {
        vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT, vconfDatetimeFormatChangedCallback);
        vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT_TIME1224, vconfDatetimeFormatChangedCallback);
    }
}

void InputPicker::canceledCallback(void* data, Evas_Object*, void*)
{
    InputPicker* self = static_cast<InputPicker*>(data);

    evas_object_del(self->m_layout);
    self->m_layout = 0;
    ewk_view_date_time_picker_set(self->m_ewkView,0);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
    eext_rotary_object_event_activated_set(self->m_ewkView, EINA_TRUE);
#endif
    vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT, vconfDatetimeFormatChangedCallback);
    vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT_TIME1224, vconfDatetimeFormatChangedCallback);
}

void InputPicker::clickedCallback(void* data, Evas_Object*, void*)
{
    struct tm currentTime;
    memset(&currentTime, 0, sizeof(struct tm));

    InputPicker* self = static_cast<InputPicker*>(data);

    elm_datetime_value_get(self->m_datetime, &currentTime);
    mktime(&currentTime);

    char dateString[20] = {0};
    strftime(dateString, 20, "%F" , &currentTime);

    if (self->m_isTimeType)
       strftime(dateString, 20, "%T", &currentTime);
    else
       strftime(dateString, 20, "%F", &currentTime);

    evas_object_del(self->m_layout);
    self->m_layout = 0;
    ewk_view_date_time_picker_set(self->m_ewkView, dateString);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
    eext_rotary_object_event_activated_set(self->m_ewkView, EINA_TRUE);
#endif
    vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT, vconfDatetimeFormatChangedCallback);
    vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT_TIME1224, vconfDatetimeFormatChangedCallback);
}

void InputPicker::show(bool isTimeType, const char* value)
{
    time_t curTime;
    time(&curTime);
    struct tm* currentTime = localtime(&curTime);

    dateTimeValue dateString;
    memset(&dateString, 0, sizeof(dateTimeValue));

    if (isTimeType) {
       if (value && strlen(value)) {
           char tempValue[maxDatetimeLength] = {0};
           snprintf(tempValue, maxDatetimeLength, "%s", value);
           char* token = strtok(tempValue,":");
           if (token)
               strncpy(dateString.hour, token, maxDatetimeLength);
           token = strtok(0, ":");
           if (token)
               strncpy(dateString.min, token, maxDatetimeLength);
           if (dateString.hour)
               currentTime->tm_hour = atoi(dateString.hour);
           if (dateString.min)
               currentTime->tm_min = atoi(dateString.min);
       }
    } else {
       if (value && strlen(value)) {
           char tempValue[maxDatetimeLength] = {0};
           snprintf(tempValue, maxDatetimeLength, "%s", value);
           char* token = strtok(tempValue,"-");
           if (token)
               strncpy(dateString.year, token, maxDatetimeLength);
           token = strtok(0, "-");
           if (token)
               strncpy(dateString.mon, token, maxDatetimeLength);
           token = strtok(0, "-");
           if (token)
               strncpy(dateString.day, token, maxDatetimeLength);

           if (dateString.year)
               currentTime->tm_year = atoi(dateString.year);
           if (dateString.mon)
               currentTime->tm_mon = atoi(dateString.mon);
           if (dateString.day)
               currentTime->tm_mday = atoi(dateString.day);

           currentTime->tm_year = currentTime->tm_year - 1900;
           currentTime->tm_mon = currentTime->tm_mon - 1;
       }
    }

#if ENABLE(TIZEN_WEARABLE_PROFILE)
    ea_theme_changeable_ui_enabled_set(EINA_TRUE);
#endif

    Evas_Coord width, height;
#ifdef HAVE_ECORE_X
    ecore_x_screen_size_get(ecore_x_default_screen_get(), &width, &height);
#else
    width = 360;
    height = 480;
#endif

    m_layout = elm_layout_add(m_ewkView);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
    elm_layout_theme_set(m_layout, "layout", "circle", "datetime");
    if (isTimeType)
        elm_object_part_text_set(m_layout, "elm.text", "Set time");
    else
        elm_object_part_text_set(m_layout, "elm.text", "Set date");
#else
    elm_layout_file_set(m_layout, EDJE_DIR "/control.edj", "elm/datetime/default");
#endif
    evas_object_resize(m_layout, width, height);

    evas_object_move(m_layout, 0, 0);

    m_datetime = elm_datetime_add(m_layout);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
    Evas_Object *conform, *circle_genlist;
    Eext_Circle_Surface *circle_surface;
    conform = elm_conformant_add(m_ewkView);
    evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_resize(conform, width, height);
    circle_surface = eext_circle_surface_conformant_add(conform);
    elm_object_content_set(conform, m_layout);
    evas_object_show(conform);

    Evas_Object* circle_datetime;
    circle_datetime = eext_circle_object_datetime_add(m_datetime, circle_surface);
    eext_rotary_object_event_activated_set(m_datetime, EINA_TRUE);
#endif
    m_isTimeType = isTimeType;

    if (!m_isTimeType) {
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
        elm_object_style_set(m_datetime, "datepicker/circle");
#else
        elm_object_style_set(m_datetime, "datepicker_layout");
#endif
    }
    else {
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
        elm_object_style_set(m_datetime, "timepicker/circle");
#else
        elm_object_style_set(m_datetime, "timepicker_layout");
#endif
    }
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
    elm_object_part_content_set(m_layout, "elm.swallow.content", m_datetime);
#else
    elm_object_part_content_set(m_layout, "content", m_datetime);
#endif
    elm_datetime_value_set(m_datetime, currentTime);
    evas_object_show(m_datetime);

    char* datetimeFormat = datetimeFormatGet();
    elm_datetime_format_set(m_datetime, datetimeFormat);
    free(datetimeFormat);
    vconf_notify_key_changed(VCONFKEY_REGIONFORMAT, vconfDatetimeFormatChangedCallback, m_datetime);
    vconf_notify_key_changed(VCONFKEY_REGIONFORMAT_TIME1224, vconfDatetimeFormatChangedCallback, m_datetime);

    Evas_Object* btn;
#if !ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
    btn = elm_button_add(m_layout);
    elm_object_text_set(btn, "Cancel");
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_content_set(m_layout, "btn1", btn);
    evas_object_smart_callback_add(btn, "clicked", canceledCallback, this);

#endif
    btn = elm_button_add(m_layout);
    elm_object_text_set(btn, "Set");
    evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_INPUT)
    elm_object_style_set(btn, "bottom");
    elm_object_part_content_set(m_layout, "elm.swallow.btn", btn);
#else
    elm_object_part_content_set(m_layout, "btn2", btn);
#endif
    evas_object_smart_callback_add(btn, "clicked", clickedCallback, this);

#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
    ea_object_event_callback_add(m_layout, EA_CALLBACK_BACK, canceledCallback, this);
#endif
    evas_object_show(m_layout);

}

} // namespace WebKit
#endif
