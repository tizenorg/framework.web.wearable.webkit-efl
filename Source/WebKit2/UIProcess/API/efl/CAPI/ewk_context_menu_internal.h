/*
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    ewk_context_menu_internal.h
 * @brief   Describes the Ewk Context Menu API.
 */

#ifndef ewk_context_menu_internal_h
#define ewk_context_menu_internal_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum    Ewk_Context_Menu_Item_Tag
 * @brief   Provides the tags of items for the context menu.
 */
typedef enum {
    EWK_CONTEXT_MENU_ITEM_TAG_NO_ACTION = 0,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_LINK_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_DOWNLOAD_LINK_TO_DISK,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_LINK_TO_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_IMAGE_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_DOWNLOAD_IMAGE_TO_DISK,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_IMAGE_TO_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_FRAME_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY,
    EWK_CONTEXT_MENU_ITEM_TAG_GO_BACK,
    EWK_CONTEXT_MENU_ITEM_TAG_GO_FORWARD,
    EWK_CONTEXT_MENU_ITEM_TAG_STOP,
    EWK_CONTEXT_MENU_ITEM_TAG_RELOAD,
    EWK_CONTEXT_MENU_ITEM_TAG_CUT,
    EWK_CONTEXT_MENU_ITEM_TAG_PASTE,
    EWK_CONTEXT_MENU_ITEM_TAG_SPELLING_GUESS,
    EWK_CONTEXT_MENU_ITEM_TAG_NO_GUESSES_FOUND,
    EWK_CONTEXT_MENU_ITEM_TAG_IGNORE_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_LEARN_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_OTHER,
    EWK_CONTEXT_MENU_ITEM_TAG_SEARCH_IN_SPOTLIGHT,
    EWK_CONTEXT_MENU_ITEM_TAG_SEARCH_WEB,
    EWK_CONTEXT_MENU_ITEM_TAG_LOOK_UP_IN_DICTIONARY,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_WITH_DEFAULT_APPLICATION,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_ACTUAL_SIZE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_ZOOM_IN,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_ZOOM_OUT,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_AUTO_SIZE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_SINGLE_PAGE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_FACTING_PAGES,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_CONTINUOUS,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_NEXT_PAGE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_PREVIOUS_PAGE,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_LINK,
    EWK_CONTEXT_MENU_ITEM_TAG_IGNORE_GRAMMAR,
    EWK_CONTEXT_MENU_ITEM_TAG_SPELLING_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_SPELLING_PANEL,
    EWK_CONTEXT_MENU_ITEM_TAG_CHECK_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_CHECK_SPELLING_WHILE_TYPING,
    EWK_CONTEXT_MENU_ITEM_TAG_CHECK_GRAMMAR_WITH_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_FONT_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_FONTS,
    EWK_CONTEXT_MENU_ITEM_TAG_BOLD,
    EWK_CONTEXT_MENU_ITEM_TAG_ITALIC,
    EWK_CONTEXT_MENU_ITEM_TAG_UNDERLINE,
    EWK_CONTEXT_MENU_ITEM_TAG_OUTLINE,
    EWK_CONTEXT_MENU_ITEM_TAG_STYLES,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_COLORS,
    EWK_CONTEXT_MENU_ITEM_TAG_SPEECH_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_START_SPEAKING,
    EWK_CONTEXT_MENU_ITEM_TAG_STOP_SPEAKING,
    EWK_CONTEXT_MENU_ITEM_TAG_WRITING_DIRECTION_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_DEFAULT_DIRECTION,
    EWK_CONTEXT_MENU_ITEM_TAG_LEFT_TO_RIGHT,
    EWK_CONTEXT_MENU_ITEM_TAG_RIGHT_TO_LEFT,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_SINGLE_PAGE_SCROLLING,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_FACING_PAGES_SCROLLING,
    EWK_CONTEXT_MENU_ITEM_TAG_INSPECT_ELEMENT,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_DEFAULT,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_LEFT_TO_RIGHT,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_RIGHT_TO_LEFT,
    EWK_CONTEXT_MENU_ITEM_TAG_CORRECT_SPELLING_AUTOMATICALLY,
    EWK_CONTEXT_MENU_ITEM_TAG_SUBSTITUTIONS_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_SUBSTITUTIONS,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_COPY_PASTE,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_QUOTES,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_DASHES,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_LINKS,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_REPLACEMENT,
    EWK_CONTEXT_MENU_ITEM_TAG_TRANSFORMATIONS_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_MAKE_UPPER_CASE,
    EWK_CONTEXT_MENU_ITEM_TAG_MAKE_LOWER_CASE,
    EWK_CONTEXT_MENU_ITEM_TAG_CAPITALIZE,
    EWK_CONTEXT_MENU_ITEM_TAG_CHANGE_BACK,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_MEDIA_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_MEDIA_LINK_TO_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_TOGGLE_MEDIA_CONTROLS,
    EWK_CONTEXT_MENU_ITEM_TAG_TOGGLE_MEDIA_LOOP,
    EWK_CONTEXT_MENU_ITEM_TAG_ENTER_VIDEO_FULLSCREEN,
    EWK_CONTEXT_MENU_ITEM_TAG_MEDIA_PLAY_PAUSE,
    EWK_CONTEXT_MENU_ITEM_TAG_MEDIA_MUTE,
    EWK_CONTEXT_MENU_ITEM_TAG_DICTATION_ALTERNATIVE,
    EWK_CONTEXT_MENU_ITEM_TAG_SELECT_ALL,
    EWK_CONTEXT_MENU_ITEM_TAG_SELECT_WORD,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_SELECTION_MODE,
    EWK_CONTEXT_MENU_ITEM_TAG_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_DRAG,
    EWK_CONTEXT_MENU_ITEM_TAG_TRANSLATE,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_LINK_DATA,

    EWK_CONTEXT_MENU_ITEM_TAG_COPY_IMAGE_URL_TO_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_PDFACTUAL_SIZE,
    EWK_CONTEXT_MENU_ITEM_PDFZOOM_IN,
    EWK_CONTEXT_MENU_ITEM_PDFZOOM_OUT,
    EWK_CONTEXT_MENU_ITEM_PDFAUTO_SIZE,
    EWK_CONTEXT_MENU_ITEM_PDFSINGLE_PAGE,
    EWK_CONTEXT_MENU_ITEM_PDFFACING_PAGES,
    EWK_CONTEXT_MENU_ITEM_PDFCONTINUOUS,
    EWK_CONTEXT_MENU_ITEM_PDFNEXT_PAGE,
    EWK_CONTEXT_MENU_ITEM_PDFPREVIOUS_PAGE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDFSINGLE_PAGE_SCROLLING,
    EWK_CONTEXT_MENU_ITEM_TAG_PDFFACING_PAGES_SCROLLING,
    EWK_CONTEXT_MENU_ITEM_TAG_DOWNLOAD_MEDIA_TO_DISK,
    EWK_CONTEXT_MENU_ITEM_OPEN_MEDIA_IN_NEW_WINDOW,

    EWK_CONTEXT_MENU_ITEM_BASE_APPLICATION_TAG = 10000

} Ewk_Context_Menu_Item_Action;

/**
 * @brief Creates a type name for Ewk_Context_Menu_Item_Tag
 */
typedef uint32_t Ewk_Context_Menu_Item_Tag;

/**
 * @brief Creates a type name for _Ewk_Context_Menu
 */
typedef struct _Ewk_Context_Menu Ewk_Context_Menu;

/**
 * @brief Creates a type name for _Ewk_Context_Menu_Item
 */
typedef struct _Ewk_Context_Menu_Item Ewk_Context_Menu_Item;

/**
 * Creates a new Ewk_Context_Menu to be used as a submenu of an existing
 * Ewk_Context_Menu. The context menu is created by the ewk_view and
 * passed as an argument of ewk_view smart callback.
 *
 * @return the pointer to the new context menu
 *
 * @see ewk_context_menu_new_with_items
 */
EAPI Ewk_Context_Menu *ewk_context_menu_new(void);

/**
 * Creates a new Ewk_Context_Menu to be used as a submenu of an existing
 * Ewk_Context_Menu with the given initial items. The context menu is
 * created by the ewk_view and passed as an argument of ewk_view smart callback.
 *
 * @param items the list of initial items
 * @return the pointer to the new context menu
 *
 * @see ewk_context_menu_new
 */
EAPI Ewk_Context_Menu *ewk_context_menu_new_with_items(Eina_List *items);

/**
 * Hides the context menu.
 *
 * @param menu the context menu to hide
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_hide(Ewk_Context_Menu *menu);

/**
 * Gets the list of items.
 *
 * @param o the context menu to get list of the items
 * @return the list of the items on success or @c NULL on failure
 */
EAPI const Eina_List *ewk_context_menu_items_get(const Ewk_Context_Menu *o);

/**
 * Selects the item from the context menu.
 *
 * @param menu the context menu
 * @param item the item is selected
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_item_select(Ewk_Context_Menu *menu, Ewk_Context_Menu_Item *item);

/**
 * @brief Counts the number of the context menu item.
 *
 * @param[in] menu the context menu object
 *
 * @return number of current context menu item
 */
EAPI unsigned ewk_context_menu_item_count(Ewk_Context_Menu* menu);

/**
 * @brief Returns the nth item in a context menu.
 *
 * @param[in] menu The context menu object
 * @param[in] n The number of the item
 *
 * @return The nth item of context menu
 */
EAPI Ewk_Context_Menu_Item* ewk_context_menu_nth_item_get(Ewk_Context_Menu* menu, unsigned int n);

/**
 * @brief Removes the context menu item from the context menu object.
 *
 * @param[in] menu the context menu object
 * @param[in] item the context menu item to remove
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_item_remove(Ewk_Context_Menu* menu, Ewk_Context_Menu_Item* item);

/**
 * @brief Adds the context menu item to the context menu object.
 *
 * @param[in] menu the context menu object
 * @param[in] tag the tag of context menu item
 * @param[in] title the title of context menu item
 * @param[in] enabled the enabled of context menu item
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_item_append_as_action(Ewk_Context_Menu* menu, Ewk_Context_Menu_Item_Tag tag, const char* title, Eina_Bool enabled);

/**
 * @brief Adds the context menu item to the context menu object.
 *
 * @param[in] menu the context menu object
 * @param[in] tag the tag of context menu item
 * @param[in] title the title of context menu item
 * @param[in] icon_file the path of icon to be set on context menu item
 * @param[in] enabled the enabled of context menu item
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_item_append(Ewk_Context_Menu* menu, Ewk_Context_Menu_Item_Tag tag, const char* title, const char* icon_file, Eina_Bool enabled);

/**
 * @brief Returns the tag of context menu item.
 *
 * @param[in] item The context menu item object
 *
 * @return The tag of context menu item
 */
EAPI Ewk_Context_Menu_Item_Tag ewk_context_menu_item_tag_get(Ewk_Context_Menu_Item* item);

/**
 * @brief Returns the link url string of context menu item.
 *
 * @param[in] item the context menu item object
 *
 * @return current link url string on success or @c 0 on failure
 */
EAPI const char* ewk_context_menu_item_link_url_get(Ewk_Context_Menu_Item* item);

/**
 * @brief Returns the image url string of context menu item.
 *
 * @param[in] item the context menu item object
 *
 * @return current image url string on success or @c 0 on failure
 */
EAPI const char* ewk_context_menu_item_image_url_get(Ewk_Context_Menu_Item* item);

#ifdef __cplusplus
}
#endif

#endif /* ewk_context_menu_internal_h */
