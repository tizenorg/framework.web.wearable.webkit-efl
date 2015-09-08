/*
   Copyright (C) 2014 Samsung Electronics

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
#include "PermissionPopupManager.h"

#include "ewk_view.h"
#include "LocalizedStrings.h"
#include "PermissionPopup.h"
#include <Elementary.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WebCore/EflScreenUtilities.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>

#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
#include <dlfcn.h>
#include <efl_assist.h>
#endif


namespace WebKit {

PermissionPopupManager::PermissionPopupManager(Evas_Object* ewkView)
    : m_isDecided(false)
    , m_permissionPopups(0)
    , m_ewkView(ewkView)
    , m_popup(0)
    , m_widgetWin(0)
    , m_topWidget(0)
{
}

PermissionPopupManager::~PermissionPopupManager()
{
    hidePopup();
    deleteAllPermissionRequest();
}

void PermissionPopupManager::addPermissionRequest(PermissionPopup* popup)
{
    m_permissionPopups = eina_list_append(m_permissionPopups, popup);
    showPermissionPopup(static_cast<PermissionPopup*>(eina_list_data_get(m_permissionPopups)));
}

void PermissionPopupManager::deletePermissionRequest(PermissionPopup* popup)
{
    m_permissionPopups = eina_list_remove(m_permissionPopups, popup);
    delete popup;

    if (eina_list_count(m_permissionPopups) == 0)
        return;

    showPermissionPopup(static_cast<PermissionPopup*>(eina_list_data_get(m_permissionPopups)));
}

void PermissionPopupManager::deleteAllPermissionRequest()
{
    void* data;
    EINA_LIST_FREE(m_permissionPopups, data)
        delete static_cast<PermissionPopup*>(data);
}

Evas_Object* PermissionPopupManager::createPopup(String message)
{
    m_topWidget = elm_object_top_widget_get(elm_object_parent_widget_get(m_ewkView));
    if (!m_topWidget)
        return 0;

    m_popup = elm_popup_add(m_topWidget);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_PERMISSION_POPUP)
    elm_object_style_set(m_popup, "circle");
#endif
    elm_popup_align_set(m_popup, ELM_NOTIFY_ALIGN_FILL, ELM_NOTIFY_ALIGN_FILL);
    if (!m_popup)
        return false;

    message.replace("\n", "</br>");
#if ENABLE(TIZEN_CIRCLE_DISPLAY_PERMISSION_POPUP)
    Evas_Object* layout = elm_layout_add(m_popup);
    elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons2");

    elm_object_part_text_set(layout, "elm.text", message.utf8().data());

    elm_object_content_set(m_popup, layout);
#else
    elm_object_text_set(m_popup, message.utf8().data());
#endif
}


static void permissionPopupFocusInCallback(void* data, Evas_Object* obj, void* eventInfo)
{
    evas_object_smart_callback_del(obj, "focus,in", permissionPopupFocusInCallback);

    PermissionPopupManager* manager = static_cast<PermissionPopupManager*>(data);
    if (!manager)
        return;
    manager->closePopup();

    PermissionPopup* popup = manager->getPermissionPopup();
    if (!popup)
        return;

    popup->sendDecidedPermission(manager->ewkView(), manager->isDecided());
    manager->deletePermissionRequest(popup);
}

static void permissionOkCallback(void* data, Evas_Object* obj, void* eventInfo)
{
    PermissionPopupManager* manager = static_cast<PermissionPopupManager*>(data);
    if (!manager)
        return;

    manager->hidePopup();
    manager->setIsDecided(true);

    PermissionPopup* popup = manager->getPermissionPopup();
    popup->sendDecidedPermission(manager->ewkView(), manager->isDecided());
    manager->deletePermissionRequest(popup);
}

static void permissionCancelCallback(void* data, Evas_Object* obj, void* eventInfo)
{
    PermissionPopupManager* manager = static_cast<PermissionPopupManager*>(data);
    if (!manager)
        return;

    manager->hidePopup();
    manager->setIsDecided(false);

    PermissionPopup* popup = manager->getPermissionPopup();
    popup->sendDecidedPermission(manager->ewkView(), manager->isDecided());
    manager->deletePermissionRequest(popup);
}

#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
static void permissionHwBackKeyCallback(void* data, Evas_Object* obj, void* eventInfo)
{
    PermissionPopupManager* manager = static_cast<PermissionPopupManager*>(data);
    if (!manager)
        return;

    evas_object_smart_callback_add(manager->getTopWidget(), "focus,in", permissionPopupFocusInCallback, data);
    manager->hidePopup();
}
#endif

void PermissionPopupManager::showPermissionPopup(PermissionPopup* popup)
{
    if (m_popup)
        return;

    createPopup(popup->getMessage());

    if (!m_popup)
        return;

     Evas_Object* okButton = elm_button_add(m_popup);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_PERMISSION_POPUP)
    elm_object_style_set(okButton, "popup/circle/left");
    evas_object_size_hint_weight_set(okButton, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
#else
    elm_object_style_set(okButton, "popup");
    elm_object_text_set(okButton, "OK");
#endif
    elm_object_part_content_set(m_popup, "button1", okButton);
    evas_object_smart_callback_add(okButton, "clicked", permissionOkCallback, this);

    Evas_Object* cancelButton = elm_button_add(m_popup);
#if ENABLE(TIZEN_CIRCLE_DISPLAY_PERMISSION_POPUP)
    elm_object_style_set(cancelButton, "popup/circle/right");
    evas_object_size_hint_weight_set(cancelButton, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
#else
    elm_object_style_set(cancelButton, "popup");
    elm_object_text_set(cancelButton, "Cancel");
#endif
    elm_object_part_content_set(m_popup, "button2", cancelButton);
    evas_object_smart_callback_add(cancelButton, "clicked", permissionCancelCallback, this);
#if ENABLE(TIZEN_HW_MORE_BACK_KEY)
    ea_object_event_callback_add(m_popup, EA_CALLBACK_BACK, permissionCancelCallback, this);
#endif

    evas_object_show(m_popup);

#if ENABLE(TIZEN_CIRCLE_DISPLAY_PERMISSION_POPUP)
    Evas_Object* icon = elm_image_add(okButton);
    elm_image_file_set(icon, JS_POPUP_IMAGE_PATH"/popup_btn_check.png", NULL);
    evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_content_set(okButton, "elm.swallow.content", icon);
    evas_object_show(icon);

    icon = elm_image_add(cancelButton);
    elm_image_file_set(icon, JS_POPUP_IMAGE_PATH"/popup_btn_delete.png", NULL);
    evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_part_content_set(cancelButton, "elm.swallow.content", icon);
    evas_object_show(icon);
#endif
    evas_object_show(m_popup);
}

bool PermissionPopupManager::setLabelText(String message)
{
    if (!message)
        return false;

    message.replace("\n", "</br>");
    elm_object_text_set(m_popup, message.utf8().data());

    return true;
}

Evas_Object* PermissionPopupManager::ewkView()
{
    return m_ewkView;
}

void PermissionPopupManager::hidePopup(){
    if (m_popup)
        evas_object_del(m_popup);
    m_popup = 0;
}

PermissionPopup* PermissionPopupManager::getPermissionPopup()
{
    return static_cast<PermissionPopup*>(eina_list_data_get(m_permissionPopups));
}

} // namespace WebKit
