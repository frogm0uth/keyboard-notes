/* Copyright 2020 @frogm0uth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include QMK_KEYBOARD_H

/**
 * Platform-dependent shortcuts.
 */
#ifndef WINDOWS
#define MACOS // this makes macOS the default if nothing is defined
#endif


// Define the platform/OS
enum os_shortcut_platform {
  platform_macos = 0, // Don't change the order
  platform_windows
};

// Macro to access shortcuts
#define SC(sc_index) (shortcut_codes[sc_index][os_index])

// Variables must be extern for the SC() macro to work
extern const uint16_t shortcut_codes[][2];
extern uint8_t os_index;


// All shortcuts. If you add to or change this, make SURE that you
// update OS_SHORTCUT_KEYCODES below and the shortcut_codes array in
// os_shortcuts.c
//
enum shortcut_index {
  // Navigation
  sc_word_left = 0, // Must be left as the first one
  sc_word_right,

  sc_start_of_line,
  sc_end_of_line,

  sc_start_of_para,
  sc_end_of_para,

  sc_start_of_doc,
  sc_end_of_doc,

  sc_del_word_left,
  sc_del_word_right,

  // Forward and backward
  sc_next_search,
  sc_prev_search,
  sc_browser_back,
  sc_browser_fwd,

  // Copy/paste/undo
  sc_select_all,
  sc_cut_selection,
  sc_copy_selection,
  sc_paste_clipboard,
  sc_undo_action,
  sc_redo_action,
  
  // App switcher
  sc_appswitch_start,
  sc_appswitch_right,
  sc_appswitch_left,

  // Window/tab navigation
  sc_next_window,     // next/prev window of current app
  sc_prev_window,

  sc_tab_right,       // move current tab to the right/left
  sc_tab_left,

  sc_tab_right_alt,   // Alternate version for some apps (MacOS only)
  sc_tab_left_alt,

  // Zooming
  sc_app_zoom_in,
  sc_app_zoom_out,
  sc_app_zoom_reset,
  
  sc_scr_zoom_in, // MacOS: needs to be enabled in Accessibility
  sc_scr_zoom_out,
  sc_scr_zoom_reset,

  // Screenshots - will probably need additional code to use
  sc_shot_screen, // Take whole screen and save to file.
  sc_shot_region, // MacOS: starts drag-select. Win: Opens Snip & Sketch

  // Window manager
  sc_expose_all,      // Expose all windows/desktops
  sc_next_screen,     // Prev/next desktop/screen
  sc_prev_screen,
  sc_fullscreen,      // Toggle window to/from fullscreen
  
  sc_expose_windows,  // Expose windows of current app
  sc_next_app,        // Expose windows of next/prev app (macOS only)
  sc_prev_app,

  sc_reveal_desktop,  // Reveal desktop
  
  // System control
  sc_screen_lock,
  sc_system_sleep,
  
  num_shortcuts // total count
};


// Implement default processing for custom keycodes for each shortcut. Call
// this at the end of process_record_user().
//
// The function name ends in "risky" because it's easy to introduce a
// bug here if changes are made in shortcut.h, in which codes will be
// wrong or even attempt to read from invalid memory location.
//
void process_record_shortcut_risky(uint16_t keycode, keyrecord_t *record);

// Change OS selection, not written to EEPROM
void os_set_from_keycode(uint16_t keycode);

// Set/get the OS, no EEPROM involved
void os_set_raw(uint8_t os);
uint8_t os_get_raw(void);
		     
// Display OS on the OLED
void os_shortcut_status(void);


// Custom keycodes for OS selection
#define OS_SELECT_KEYCODES    \
  CU_SELECT_MACOS,	      \
  CU_SELECT_WINDOWS

// Macro to define custom keycodes for all shortcuts, include in custom_keycodes enum
// ***WARNING*** Items MUST be the exact same order as shortcut_index above ***WARNING***
//
#define OS_SHORTCUT_KEYCODES 			\
  CU_WORD_LEFT,					\
    CU_WORD_RIGHT,				\
						\
    CU_START_OF_LINE,				\
    CU_END_OF_LINE,				\
						\
    CU_START_OF_PARA,				\
    CU_END_OF_PARA,				\
						\
    CU_START_OF_DOC,				\
    CU_END_OF_DOC,				\
						\
    CU_DEL_WORD_LEFT,				\
    CU_DEL_WORD_RIGHT,				\
						\
    CU_NEXT_SEARCH,				\
    CU_PREV_SEARCH,				\
    CU_BROWSER_BACK,				\
    CU_BROWSER_FWD,				\
						\
    CU_SELECT_ALL,				\
    CU_CUT_SELECTION,				\
    CU_COPY_SELECTION,				\
    CU_PASTE_CLIPBOARD,				\
    CU_UNDO_ACTION,				\
    CU_REDO_ACTION,				\
						\
    CU_APPSWITCH_START,				\
    CU_APPSWITCH_RIGHT,				\
    CU_APPSWITCH_LEFT,				\
						\
    CU_NEXT_WINDOW,				\
    CU_PREV_WINDOW,				\
    						\
    CU_TAB_RIGHT,				\
    CU_TAB_LEFT,				\
    						\
    CU_TAB_RIGHT_ALT,				\
    CU_TAB_LEFT_ALT,				\
						\
    CU_APP_ZOOM_IN,				\
    CU_APP_ZOOM_OUT,				\
    CU_APP_ZOOM_RESET,				\
						\
    CU_SCR_ZOOM_IN,				\
    CU_SCR_ZOOM_OUT,				\
    CU_SCR_ZOOM_RESET,				\
						\
    CU_SHOT_SCREEN,				\
    CU_SHOT_REGION,				\
						\
    CU_EXPOSE_ALL,				\
    CU_NEXT_SCREEN,				\
    CU_PREV_SCREEN,				\
    CU_FULLSCREEN,				\
						\
    CU_EXPOSE_WINDOWS,				\
    CU_NEXT_APP,				\
    CU_PREV_APP,				\
						\
    CU_REVEAL_DESKTOP,				\
						\
    CU_SCREEN_LOCK,				\
    CU_SYSTEM_SLEEP
