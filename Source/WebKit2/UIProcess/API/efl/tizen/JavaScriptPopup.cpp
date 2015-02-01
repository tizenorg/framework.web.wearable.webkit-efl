/*
   Copyright (C) 2012 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "JavaScriptPopup.h"

#include "ewk_view.h"
#include "ewk_view_private.h"
#include <Elementary.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
#include <efl_assist.h>
#endif

namespace WebKit {

JavaScriptPopup::JavaScriptPopup(Evas_Object* ewkView)
    : m_popup(0)
    , m_entry(0)
    , m_ewkView(ewkView)
{
}

JavaScriptPopup::~JavaScriptPopup()
{
    close();
}

static void alertResponseCallback(void* data, Evas_Object*, void* /*eventInfo*/)
{
    JavaScriptPopup* popup = static_cast<JavaScriptPopup*>(data);
    if (!popup)
        return;
    popup->close();
    ewk_view_javascript_alert_reply(popup->ewkView());
}

bool JavaScriptPopup::alert(const char* message)
{
    if (m_popup)
        return false;

    Evas_Object* parent = getParentWindow();

    if (!parent)
        return false;

    m_popup = elm_popup_add(parent);
    evas_object_size_hint_weight_set(m_popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    if (!m_popup)
        return false;

    String popupMessage = String::fromUTF8(elm_entry_utf8_to_markup(message));
    popupMessage.replace("\n", "</br>");

    elm_object_text_set(m_popup, popupMessage.utf8().data());

    Evas_Object* okButton = elm_button_add(m_popup);
    // FIXME: Need to apply i18n for the button label
    elm_object_style_set(okButton, "popup");
    elm_object_text_set(okButton, "OK");
    elm_object_part_content_set(m_popup, "button1", okButton);
    evas_object_smart_callback_add(okButton, "clicked", alertResponseCallback, this);
#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
    ea_object_event_callback_add(m_popup, EA_CALLBACK_BACK, alertResponseCallback, this);
#endif

#if ENABLE(TIZEN_CHECK_MODAL_POPUP)
    ewk_view_modal_popup_open_set(m_ewkView, true);
#endif
    evas_object_show(m_popup);

    return true;
}

static void confirmOkCallback(void* data, Evas_Object*, void* /*eventInfo*/)
{
    JavaScriptPopup* popup = static_cast<JavaScriptPopup*>(data);
    if (!popup)
        return;
    popup->close();
    ewk_view_javascript_confirm_reply(popup->ewkView(), true);
}

static void confirmCancelCallback(void* data, Evas_Object*, void* /*eventInfo*/)
{
    JavaScriptPopup* popup = static_cast<JavaScriptPopup*>(data);
    if (!popup)
        return;
    popup->close();
    ewk_view_javascript_confirm_reply(popup->ewkView(), false);
}

bool JavaScriptPopup::confirm(const char* message)
{
    if (m_popup)
        return false;

    Evas_Object* parent = getParentWindow();

    if (!parent)
        return false;

    m_popup = elm_popup_add(parent);
    if (!m_popup)
        return false;

    String popupMessage = String::fromUTF8(elm_entry_utf8_to_markup(message));
    popupMessage.replace("\n", "</br>");

    elm_object_text_set(m_popup, popupMessage.utf8().data());

    Evas_Object* okButton = elm_button_add(m_popup);
    // FIXME: Need to apply i18n for the button label
    elm_object_style_set(okButton, "popup");
    elm_object_text_set(okButton, "OK");
    elm_object_part_content_set(m_popup, "button1", okButton);
    evas_object_smart_callback_add(okButton, "clicked", confirmOkCallback, this);

    Evas_Object* cancelButton = elm_button_add(m_popup);
    // FIXME: Need to apply i18n for the button label
    elm_object_style_set(cancelButton, "popup");
    elm_object_text_set(cancelButton, "Cancel");
    elm_object_part_content_set(m_popup, "button2", cancelButton);
    evas_object_smart_callback_add(cancelButton, "clicked", confirmCancelCallback, this);
#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
    ea_object_event_callback_add(m_popup, EA_CALLBACK_BACK, confirmCancelCallback, this);
#endif

#if ENABLE(TIZEN_CHECK_MODAL_POPUP)
    ewk_view_modal_popup_open_set(m_ewkView, true);
#endif
    evas_object_show(m_popup);

    return true;
}

static void promptEntryChanged(void* data, Ecore_IMF_Context*, int value)
{
    if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
        Evas_Object* entry = static_cast<Evas_Object*>(data);
        elm_object_focus_set(entry, false);
    }
}

static void promptCancelCallback(void* data, Evas_Object*, void* /*eventInfo*/)
{
    JavaScriptPopup* popup = static_cast<JavaScriptPopup*>(data);
    if (!popup)
        return;
    popup->close();
    ewk_view_javascript_prompt_reply(popup->ewkView(), 0);
}

static void promptOkCallback(void* data, Evas_Object*, void* /*eventInfo*/)
{
    JavaScriptPopup* popup = static_cast<JavaScriptPopup*>(data);
    if (!popup)
        return;
    char* result = elm_entry_markup_to_utf8(elm_entry_entry_get(popup->entry()));
    popup->close();
    ewk_view_javascript_prompt_reply(popup->ewkView(), result);
    if (result)
        free(result);
}

bool JavaScriptPopup::prompt(const char* message, const char* defaultValue)
{
    Evas_Object* parent = getParentWindow();

    if (m_popup)
        return false;

    if (!parent)
        return false;

    m_popup = elm_popup_add(parent);
    evas_object_size_hint_weight_set(m_popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    if (!m_popup)
        return false;

    if (message)
        elm_object_part_text_set(m_popup, "title,text", message);

    Evas_Object* layout = elm_layout_add(m_popup);
    elm_layout_file_set(layout, EDJE_DIR"/JavaScriptPopup.edj", "prompt");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_min_set(layout, 320, 320);

    m_entry = elm_entry_add(m_popup);
    Ecore_IMF_Context* imfContext = static_cast<Ecore_IMF_Context*>(elm_entry_imf_context_get(m_entry));
    ecore_imf_context_input_panel_event_callback_add(imfContext, ECORE_IMF_INPUT_PANEL_STATE_EVENT, promptEntryChanged, 0);
    elm_entry_scrollable_set(m_entry, EINA_TRUE);
    elm_object_text_set(m_entry, defaultValue);
    elm_entry_cursor_end_set(m_entry);
    elm_object_part_content_set(layout, "prompt_container", m_entry);
    elm_object_content_set(m_popup, layout);
    evas_object_show(m_entry);

    Evas_Object* okButton = elm_button_add(m_popup);
    // FIXME: Need to apply i18n for the button label
    elm_object_style_set(okButton, "popup");
    evas_object_size_hint_weight_set(okButton, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_text_set(okButton, "OK");
    elm_object_part_content_set(m_popup, "button1", okButton);
    evas_object_smart_callback_add(okButton, "clicked", promptOkCallback, this);

    Evas_Object* cancelButton = elm_button_add(m_popup);
    // FIXME: Need to apply i18n for the button label
    elm_object_style_set(cancelButton, "popup");
    evas_object_size_hint_weight_set(cancelButton, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_text_set(cancelButton, "Cancel");
    elm_object_part_content_set(m_popup, "button2", cancelButton);
    evas_object_smart_callback_add(cancelButton, "clicked", promptCancelCallback, this);
#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
    ea_object_event_callback_add(m_popup, EA_CALLBACK_BACK, promptCancelCallback, this);
#endif

#if ENABLE(TIZEN_CHECK_MODAL_POPUP)
    ewk_view_modal_popup_open_set(m_ewkView, true);
#endif
    evas_object_show(m_popup);
    return true;
}

Evas_Object* JavaScriptPopup::ewkView()
{
    return m_ewkView;
}

Evas_Object* JavaScriptPopup::entry()
{
    return m_entry;
}

Evas_Object* JavaScriptPopup::getParentWindow()
{
    Evas_Object* parent = elm_object_parent_widget_get(m_ewkView);

    if(!parent)
       return 0;

    while (parent) {
        const char* type = elm_object_widget_type_get(parent);
        if (type && !strncmp(type, "elm_conformant", strlen("elm_conformant")))
                return(elm_object_content_get(parent));
        parent = elm_object_parent_widget_get(parent);
    }

    return 0;
}

void JavaScriptPopup::close()
{
    if (m_popup) {
#if ENABLE(TIZEN_CHECK_MODAL_POPUP)
        ewk_view_modal_popup_open_set(m_ewkView, false);
#endif
        evas_object_del(m_popup);
        m_popup = 0;
    }
    m_entry = 0;
}

} // namespace WebKit
