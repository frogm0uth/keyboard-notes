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

/**
 * Platform-dependent shortcuts.
 */
#ifndef WINDOWS
#define MACOS // this makes macOS the default if nothing is defined
#endif

// System control
#ifdef MACOS
#define SC_SLOCK  C(G(KC_Q))     // Lock screen
#define SC_SLEEP  A(G(KC_EJCT))  // System sleep
#else // WINDOWS
#define SC_SLOCK  G(KC_L)     // Lock screen
#define SC_SLEEP  KC_SLEP     // System sleep
#endif

// Window manager things
#ifdef MACOS
#define SC_WALL C(KC_UP)        // Expose all windows
#define SC_WAPP C(KC_DOWN)      // Expose app windows
#define SC_DTOP KC_F11          // Reveal desktop

#define SC_SCR_R C(KC_RIGHT)     // Move to next screen
#define SC_SCR_L C(KC_LEFT)      // Move to previous screen

#define SC_APP_0 KC_LGUI         // Start app switcher
#define SC_APP_R KC_TAB          // Next application
#define SC_APP_L S(KC_TAB)       // Previous application

#define SC_APP_N KC_TAB       // Next application while in Mission Control
#define SC_APP_P S(KC_TAB)    // Previous application

#define SC_WIN_R G(KC_GRAVE)     // Next application window
#define SC_WIN_L S(G(KC_GRAVE))  // Previous application window

#define SC_TAB_R C(KC_TAB)       // Move to next tab
#define SC_TAB_L S(C(KC_TAB))    // Move to previous tab

#define SC_TAB2_R A(G(KC_RIGHT)) // Move to next tab, alternate version
#define SC_TAB2_L A(G(KC_LEFT))  // Move to previous tab, alternate version

#define SC_FSCR G(C(KC_F))    // Toggle full-screen

#else // WINDOWS

#define SC_WALL G(KC_TAB)        // Expose all windows
#define SC_WAPP G(A(KC_TAB))     // Expose app windows (like alt-tab but persistent)
#define SC_DTOP G(KC_D)          // Reveal desktop

#define SC_SCR_R G(C(KC_RIGHT))     // Move to next screen
#define SC_SCR_L G(C(KC_LEFT))      // Move to previous screen

#define SC_APP_0 KC_LALT         // Start app switcher
#define SC_APP_R KC_TAB          // Next application
#define SC_APP_L S(KC_TAB)       // Previous application

#define SC_APP_N KC_NO       // Does nothing
#define SC_APP_P KC_NO

#define SC_WIN_R A(KC_ESC)     // Next application window
#define SC_WIN_L S(A(KC_ESC))  // Previous application window

#define SC_TAB_R C(KC_TAB)       // Move to next tab
#define SC_TAB_L S(C(KC_TAB))    // Move to previous tab

#define SC_TAB2_R SC_TAB_R
#define SC_TAB2_L SC_TAB_L

#define SC_FSCR KC_F11    // Toggle full-screen
#endif

// Copy/paste/undo
#ifdef MACOS
#define SC_ALL  G(KC_A)   // Select all
#define SC_CUT  G(KC_X)   // Cut/copy/paste
#define SC_COPY G(KC_C)
#define SC_PAST G(KC_V)
#define SC_UNDO G(KC_Z)    // Undo
#define SC_REDO S(G(KC_Z)) // Redo

#else // WINDOWS
#define SC_ALL  C(KC_A)   // Select all
#define SC_CUT  C(KC_X)   // Cut/copy/paste
#define SC_COPY C(KC_C)
#define SC_PAST C(KC_V)
#define SC_UNDO C(KC_Z)   // Undo
#define SC_REDO C(KC_Y)   // Redo
#endif

// Navigation
#ifdef MACOS
#define SC_WORD_L     A(KC_LEFT)    // Move word left
#define SC_WORD_R     A(KC_RIGHT)   // Move word right
#define SC_DEL_WORD_L A(KC_BSPC)    // Delete word left
#define SC_DEL_WORD_R A(KC_DEL)     // Delete word right

#define SC_SOL G(KC_LEFT)     // Move to start of line
#define SC_EOL G(KC_RIGHT)    // Move to end of line

#define SC_SOP A(KC_UP)       // Move to start of paragraph (or up one)
#define SC_EOP A(KC_DOWN)     // Move to end of paragraph (or down one)

#define SC_SRCH_R G(KC_G)        // Next search result
#define SC_SRCH_L S(G(KC_G))     // Previous search result
  

#else // WINDOWS

#define SC_WORD_L     C(KC_LEFT)    // Move word left
#define SC_WORD_R     C(KC_RIGHT)   // Move word right
#define SC_DEL_WORD_L C(KC_BSPC)    // Delete word left
#define SC_DEL_WORD_R C(KC_DEL)     // Delete word right

#define SC_SOL KC_HOME     // Move to start of line
#define SC_EOL KC_END      // Move to end of line

#define SC_SOP C(KC_UP)       // Move to start of paragraph (or up one)
#define SC_EOP C(KC_DOWN)     // Move to end of paragraph (or down one)

#define SC_SRCH_R KC_F3        // Next search result
#define SC_SRCH_L S(KC_F3)     // Previous search result

#endif


// Browser
#ifdef MACOS
#define SC_BACK   G(KC_LBRC)  // Go back one page
#define SC_FWD    G(KC_RBRC)  // Go forward one page
#define SC_RELOAD G(KC_R)     // Reload page
#else // WINDOWS
#define SC_BACK   A(KC_LEFT)  // Go back one page
#define SC_FWD    A(KC_RIGHT) // Go forward one page
#define SC_RELOAD C(KC_R)     // Reload page
#endif


// Zooming
#ifdef MACOS
#define SC_SCR_ZIN  A(G(KC_EQL))  // Screen zoom in (needs to be enabled in Accessibility)
#define SC_SCR_ZOUT A(G(KC_MINS)) // Screen zoom out screen
#define SC_SCR_ZD   A(G(KC_8))    // Screen zoom default
#define SC_APP_ZIN  G(KC_EQL)     // Application zoom in
#define SC_APP_ZOUT G(KC_MINS)    // Application zoom out
#define SC_APP_ZD   G(KC_0)       // Application zoom default
#else // WINDOWS
#define SC_SCR_ZIN  G(KC_EQL)     // Screen zoom in
#define SC_SCR_ZOUT G(KC_MINS)    // Screen zoom out
#define SC_SCR_ZD   G(KC_MINS)    // not useful, really want Screen zoom default
#define SC_APP_ZIN  C(KC_EQL)     // Application zoom in
#define SC_APP_ZOUT C(KC_MINS)    // Application zoom out
#define SC_APP_ZD   C(KC_0)       // Application zoom default
#endif

// Screenshots
#ifdef MACOS
#define SC_SSSCR S(G(KC_3))  // Take whole screen and save to file
#define SC_SSRGN S(G(KC_4))  // Then drag-select region
#else // WINDOWS
#define SC_SSSCR G(KC_PSCR)  // Take whole screen and save to file
#define SC_SSRGN G(S(KC_S))  // Start Snip & Sketch
#endif

