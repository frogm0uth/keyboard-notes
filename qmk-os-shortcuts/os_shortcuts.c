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

#include "keymap.h"
#include <print.h>

/**
 * Platform-dependent shortcuts.
 */
#ifdef WINDOWS
uint8_t os_index = platform_windows;
#else
uint8_t os_index = platform_macos;
#endif

void os_set_raw(uint8_t os) {
  os_index = os;
}
uint8_t os_get_raw() {
  return os_index;
}

// The array of shortcut codes. First column for Mac, second column
// for Windows
const uint16_t shortcut_codes[][2] = {

  // Navigation
  [sc_word_left]      = { A(KC_LEFT),  C(KC_LEFT) },
  [sc_word_right]     = { A(KC_RIGHT), C(KC_RIGHT) },

  [sc_start_of_line]  = { G(KC_LEFT),  KC_HOME },
  [sc_end_of_line]    = { G(KC_RIGHT), KC_END },

  [sc_start_of_para]  = { A(KC_UP),    C(KC_UP) },
  [sc_end_of_para]    = { A(KC_DOWN),  C(KC_DOWN) },

  [sc_start_of_doc]   = { G(KC_UP),    C(KC_HOME) },
  [sc_end_of_doc]     = { G(KC_DOWN),  C(KC_END) },

  [sc_del_word_left]  = { A(KC_BSPC),  C(KC_BSPC) },
  [sc_del_word_right] = { A(KC_DEL),   C(KC_DEL) },

  // Forward and backward
  [sc_next_search]    = { G(KC_G),     KC_F3 },
  [sc_prev_search]    = { S(G(KC_G)),  S(KC_F3) },
  [sc_browser_back]   = { G(KC_LBRC),  A(KC_LEFT) },
  [sc_browser_fwd]    = { G(KC_RBRC),  A(KC_RIGHT) },
  
  // Copy/paste/undo
  [sc_select_all]      = { G(KC_A),    C(KC_A)},
  [sc_cut_selection]   = { G(KC_X),    C(KC_X)},
  [sc_copy_selection]  = { G(KC_C),    C(KC_C)},
  [sc_paste_clipboard] = { G(KC_V),    C(KC_V)},
  [sc_undo_action]     = { G(KC_Z),    C(KC_Z)},
  [sc_redo_action]     = { S(G(KC_A)), C(KC_Y)},

  // App switcher
  [sc_appswitch_start] = { KC_LGUI,   KC_LALT },
  [sc_appswitch_right] = { KC_TAB,    KC_TAB },
  [sc_appswitch_left]  = { S(KC_TAB), S(KC_TAB) },

  // Window/tab navigation
  [sc_next_window]     = { G(KC_GRAVE),    A(KC_ESC) },
  [sc_prev_window]     = { S(G(KC_GRAVE)), S(A(KC_ESC)) },

  [sc_tab_right]       = { C(KC_TAB),      C(KC_TAB) },
  [sc_tab_left]        = { S(C(KC_TAB)),   S(C(KC_TAB)) },

  [sc_tab_right_alt]   = { A(G(KC_RIGHT)), C(KC_TAB) }, // Win: does the same as above
  [sc_tab_left_alt]    = { A(G(KC_LEFT)),  S(C(KC_TAB)) },

  // Zooming
  [sc_app_zoom_in]     = { G(KC_EQL),     C(KC_EQL) },
  [sc_app_zoom_out]    = { G(KC_MINS),    C(KC_MINS) },
  [sc_app_zoom_reset]  = { G(KC_0),       C(KC_0) },
  
  [sc_scr_zoom_in]     = { A(G(KC_EQL)),  G(KC_EQL) }, // MacOS: needs to be enabled in Accessibility
  [sc_scr_zoom_out]    = { A(G(KC_MINS)), G(KC_MINS) },
  [sc_scr_zoom_reset]  = { A(G(KC_8)),    G(KC_MINS) }, // Win: not useful
  
  // Screenshots - will probably need additional code to use
  [sc_shot_screen]     = { S(G(KC_3)),  G(KC_PSCR) },
  [sc_shot_region]     = { S(G(KC_4)),  G(S(KC_S)) },

  // Window manager
  [sc_expose_all]      = { C(KC_UP),    G(KC_TAB) },    
  [sc_next_screen]     = { C(KC_RIGHT), G(C(KC_RIGHT)) },
  [sc_prev_screen]     = { C(KC_LEFT),  G(C(KC_LEFT)) },
  [sc_fullscreen]      = { G(C(KC_F)),  KC_F11 },

  [sc_expose_windows]  = { C(KC_DOWN),  G(A(KC_TAB)) },
  [sc_next_app]        = { KC_TAB,      KC_NO }, // Win: does nothing FIXME
  [sc_prev_app]        = { S(KC_TAB),   KC_NO },

  [sc_reveal_desktop]  = { KC_F11,      G(KC_D) },

  // System control
  [sc_screen_lock]    = { C(G(KC_Q)),   A(G(KC_EJCT)) },
  [sc_system_sleep]   = { G(KC_L),      KC_SLEP },

};


// Implement default processing for custom keycodes for each shortcut. Call
// this at the end of process_record_user().
//
// The function name ends in "risky" because it's easy to introduce a
// bug here if changes are made in shortcut.h, in which case codes
// will be wrong or may even attempt to read from an invalid memory
// location.
//
void process_record_shortcut_risky(uint16_t keycode, keyrecord_t *record) {
  if (keycode >= CU_WORD_LEFT) {
    uint16_t offset = keycode - CU_WORD_LEFT;
    if (offset < num_shortcuts) {
      uint16_t shortcut = SC(offset); // a bit scary...
      if (record->event.pressed) {
	register_code16(shortcut);
      } else {
	unregister_code16(shortcut);
      }
    }
  }
}

// Set the OS from a keycode. Call this from process_record_user(). Not persistent.
//
void os_set_from_keycode(uint16_t keycode) {
  switch (keycode) {
  case CU_SELECT_MACOS:
    os_index = platform_macos;
    break;
      
  case CU_SELECT_WINDOWS:
    os_index = platform_windows;
    break;
  }
}

// Display OS on the OLED
//
#ifdef OLED_DRIVER_ENABLE
void os_shortcut_status () {
  switch (os_index) {
  case platform_macos:
    oled_write_P(PSTR("MACOS "), false);
    break;
  case platform_windows:
    oled_write_P(PSTR("WINDOWS "), false);
    break;
  default:
    oled_write_P(PSTR("?OS? "), false);
    break;
  }
}
#endif
