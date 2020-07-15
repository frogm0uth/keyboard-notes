# OS Shortcuts - QMK notes

OS Shortcuts is a utility to ease implementing platform-dependent shortcuts in a keymap. It provides a set of shortcuts specific to each supported platform (currently, macOS and Windows).
<!--ts-->

   * [OS Shortcuts - QMK notes](#os-shortcuts---qmk-notes)
      * [Rationale](#rationale)
      * [LIMITATIONS](#limitations)
   * [Usage](#usage)
      * [How to add OS Shortcuts to your keymap](#how-to-add-os-shortcuts-to-your-keymap)
         * [For static shortcuts](#for-static-shortcuts)
         * [For dynamic/runtime shortcuts](#for-dynamicruntime-shortcuts)
      * [How to add shortcuts to custom code](#how-to-add-shortcuts-to-custom-code)
      * [How to change the OS selection](#how-to-change-the-os-selection)
      * [How to make OS selection persistent](#how-to-make-os-selection-persistent)
      * [OLED Support](#oled-support)
   * [Firmware size](#firmware-size)

<!--te-->
## Rationale

While I mostly type on a Mac, I also need to use Windows on a regular basis. Unfortunately, many shortcuts are different between the two platforms, leading to a lot of fumbles when I switch.

Therefore, the OS Shortcuts code here provides a useful set of shortcuts to make it easier to switch between platforms. Most of them are related to navigation (modifier+arrow) and window navigation. They can be added to the keymap as-is or used in more complex custom keycodes/macros.

There are two ways of using OS Shortcuts: static, where the actual shortcut codes are compiled in, or dynamic, where you can select the platform you want with a key on the keyboard.

## LIMITATIONS

1. Windows functionality largely untested.
2. No codes for Linux. This is to be added later.

# Usage

## How to add OS Shortcuts to your keymap

Note: the following assumes that you have a keymap.h file which is included by keymap.c and contains the definition of the `custom_keycodes` enum. The code can be obtained from my [Kyria keymap](../../../../keyboard-firmware/tree/master/kyria-rsthd-prime).

To add OS Shortcuts to your keymap:

1. Drop the files os_shortcuts.c, os_shortcuts.h and os_shortcuts_static.h into your keymap folder.

2. Include os_shortcuts.h in your keymap.h:

   ```c
   #include "os_shortcuts.h"
   ```

3. Add the following to your rules.mk:

   ```cmake
   OS_SHORTCUTS = yes	# Enable OS shortcut mapping
   ...
   ifeq ($(strip $(OS_SHORTCUTS)), yes)
   	SRC += os_shortcuts.c
   	OPT_DEFS += -DOS_SHORTCUTS
   endif
   ```
   
### For static shortcuts

If you want to have the shortcuts statically compiled for one platform:

4. Add this line to your config.h:

   ```c
   #define OS_SHORTCUTS_STATIC
   ```
   
5. Add one of the following to config.h:

   ```
   #define OS_MACOS
   ```

   or:

   ```
   #define OS_WINDOWS
   ```

### For dynamic/runtime shortcuts

If you want to be able to change the OS selection dynamically i.e. at runtime:

6. Add the following to your `custom_keycode` enum:

   ```c
   #if defined(OS_SHORTCUTS) && !defined(OS_SHORTCUTS_STATIC)
     OS_SELECT_KEYCODES,
     OS_SHORTCUT_KEYCODES,
   #endif
   ```

5. Optionally, create short names for the shortcut codes that you want to put in your keymap. For example:

   ```c
   #define SC_ALL SC_SELECT_ALL
   ```

6. Add the shortcut codes, or your #defined equivalents, to the keymap.

8. Add the following **after** the main switch in process_record_user():

   ```c
       // Default processing for OS shortcuts
   #if defined(OS_SHORTCUTS) && !defined(OS_SHORTCUTS_STATIC)
     process_record_shortcut(keycode, record);
   #endif
   ```

   Now each shortcut key in your keymap will automatically emit the correct shortcut for the currently selected platform.

9. For anything that requires a sequence of shortcuts, define more custom keycodes in `custom_keycode` and handle them in process_record_user().

## How to add shortcuts to custom code

Use the macro SC() to access shortcuts. The argument to SC must be listed in the OS_SHORTCUT_KEYCODES macro in shortcuts.h. For example, the following work for both static and dynamic shortcuts:

```c
  tap_code16(SC(SC_START_OF_DOC)); // Move to start of document
```

and:

```c
  register_code16(SC(SC_DEL_WORD_LEFT));   // Delete word left, auto-repeat while held
  ...
  unregister_code16(SC(SC_DEL_WORD_LEFT)); // Stop auto-repeat on word delete
```

Note that the argument to SC **must** be a constant defined in shortcuts.h. You can not use a variable as its argument. So this (for example) doesn't work:

```c
switch (keycode) {
  case SC_START_OF_DOC:
    tap_code16(SC(keycode);
    break;
```

## A note on static shortcuts

If you followed the instructions above for static shortcuts, the keycodes `SC_...` will not be in your keymap. This is because there is no way with static shortcuts to automatically handle those keycodes. I'm assuming in that case you will define the  keycodes manually, as you have to manually write the case for each of them in process_record_user() anyway.

If you do want all the keycodes defined, add this to  your `custom_keycode` enum:

```c
   #if defined(OS_SHORTCUTS)
     OS_SHORTCUT_KEYCODES,
   #endif
```

Then add the keycodes to your keymap and add the cases to process_record_user().

## How to change the OS selection

You can change the OS selection if you are using dynamic shortcuts:

1. Add OS_SELECT_KEYCODES to your `custom_keycode` enum as described earlier.

2. Add the keycodes CU_SELECT_MACOS and CU_SELECT_WINDOWS to your keymap. You may want to #define shorter versions:

   ```c
   #define CU_MAC    CU_SELECT_MACOS
   #define CU_WIN    CU_SELECT_WINDOWS
   ```

3. Add code to handle them in process_record_user():

   ```
   #if defined(OS_SHORTCUTS) && !defined(OS_SHORTCUTS_STATIC)
     case CU_SELECT_MACOS:
     case CU_SELECT_WINDOWS:
       if (record->event.pressed) {
         os_set_from_keycode(keycode);
       }
       break;
   #endif
   ```

## How to make OS selection persistent

To make the OS selection persistent, you will need to have a 32-bit structure and variable, as described in the [QMK docs](https://docs.qmk.fm/#/custom_quantum_functions?id=persistent-configuration-eeprom). If you already have this, add a 2-bit field to it for the OS selection as shown below. If you don't, add the following to your keymap.c:

```c
typedef union {
  uint32_t raw;
  struct {
    uint8_t     os_selection :2;
  };
} user_config_t;

user_config_t user_config;
```

Also in keymap.c, add the initialization function that reads the EEPROM and sets the OS:

```c
void keyboard_post_init_user(void) {
  // Read the user config from EEPROM
  user_config.raw = eeconfig_read_user();

  // Set OS
#if defined OS_SHORTCUTS && !defined(OS_SHORTCUTS_STATIC)
  os_set_raw(user_config.os_selection);
#endif
}
```

(If you already have this function, add the last line shown above.)

Change the code in process_record_user() to update the EEPROM when the OS selection is changed:

```c
#if defined(OS_SHORTCUTS) && !defined(OS_SHORTCUTS_STATIC)
  case CU_SELECT_MACOS:
  case CU_SELECT_WINDOWS:
    if (record->event.pressed) {
      os_set_from_keycode(keycode);
      user_config.os_selection = os_get_raw();
      eeconfig_update_user(user_config.raw);
    }
    break;
#endif
```

## OLED Support

If you have an OLED, the OS selection can be displayed by adding something like this to your `oled_task_user` function: 

```c
...
#ifdef OS_SHORTCUTS
    os_shortcuts_status();
#endif
...
}
oled_write_P(PSTR("\n"), false);
```

# Firmware size

In static mode, OS Shortcuts will add very little size. With dynamic shortcuts enabled, OS Shortcuts adds about 800 bytes to the size of my base Kyria firmware. Using shortcuts does add some additional size compared to using hardwired codes, so the actual size difference will vary. For example, with extensive use of shortcuts throughout my keymap, the difference is about 1 kbytes.

