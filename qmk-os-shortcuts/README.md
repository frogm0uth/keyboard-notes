# OS Shortcuts - QMK notes

OS Shortcuts is a utility to ease implementing platform-dependent shortcuts in a keymap. It provides a set of shortcuts specific to each supported platform (currently, macOS and Windows).

**Rationale**

While I mostly type on a Mac, I also need to use Windows on a regular basis. Unfortunately, many shortcuts are different between the two platforms, leading to a lot of fumbles when I switch.

Therefore, the OS Shortcuts code here provides all the shortcuts I can find that have a reasonable equivalence between the platforms. They can be added to the keymap as- is or used in more complex custom keycodes/macros.

**LIMITATIONS**

1. Windows functionality largely untested.
2. No codes for Linux. This is to be added later.

**How to use add OS Shortcuts to your keymap**

Note: the following assumes that you have a keymap.h file which is included by keymap.c and contains the definitions of the `custom_keycodes` enum.

To add OS Shortcuts to your keymap:

1. Drop the files os_shortcuts.c and os_shortcuts.h into your keymap folder.

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
   
5. **If** you want to define a custom keycode for every single shortcut, add the following to your `custom_keycode` enum. Because this defines a lot of custom keycodes which you may not use, it's probably best put at the end of the enum:

   ```c
   #ifdef OS_SHORTCUTS
     OS_SHORTCUT_KEYCODES,
   #endif
   ```
   
   Otherwise, you can just add individual keycodes to `custom_keycode`.
   
5. **If** you created all shortcut keycodes as above, add them to the keymap. The predefined names all start with "CU_" and you can see the full list in shortcodes.h. Because the names are fairly long, you might want to #define shorter versions in keymap.h e.g.:

   ```c
   #define CU_ALL CU_SELECT_ALL
   ```

6. **If** you created all shortcut keycodes above, you can have them all send the shortcut in response to each custom keycode. To do this, add the following code to the END of the main switch in process_record_user():

   ```c
       // Default processing for OS shortcuts
       process_record_shortcut_risky(keycode, record);
   ```

   The name of this function includes "risky" because it's very easy to get the wrong keycodes if changes are made in shortcuts.h. **Use at your own risk.**

   If you are not using many shortcuts directly from the keymap, you may prefer to manually define the handling of each of them in process_record_user().

   You may be tempted to "override" a shortcut keycode. For example, suppose you wanted to have CU_SELECT_ALL move the cursor before selecting all, so added a case for CU_SELECT_ALL to process_record_user(). If you do that, you have to return false immediately, otherwise the keycode will be processed again in process_record_shortcut_risky(). It would be better to define a different keycode e.g. CU_MY_SEL. **Either**:

   1. Do not call process_record_shortcut_risky(), and define handling for each shortcut keycode manually, **or**
   2. Call  process_record_shortcut_risky() and use different keycodes for anything you need handled differently than the default. 

**How to use OS Shortcuts**

Once you have added OS Shortcuts to your keymap, use the macro SC() to access them. The argument to SC must be an element of shortcuts_index, defined in shortcuts.h. For example:

```c
  tap_code16(SC(sc_start_of_doc)); // Move to start of document
```

and:

```c
  register_code16(SC(sc_del_word_left));   // Delete word left, auto-repeat while held
  ...
  unregister_code16(SC(sc_del_word_left)); // Stop auto-repeat on word delete
```

To switch OS, use the custom keycodes CU_SELECT_MACOS and CU_SELECT_WINDOWS. See below for more details. 

**How to change the OS selection**

To create keycodes to switch the OS selection.

1. Add this to your `custom_keycode` enum:

   ```c
   #ifdef OS_SHORTCUTS
     OS_SELECT_KEYCODES,
   #endif
   ```

2. Add the keycodes CU_SELECT_MACOS and CU_SELECT_WINDOWS to your keymap. You may want to #define shorter versions:

   ```c
   #define CU_MAC    CU_SELECT_MACOS
   #define CU_WIN    CU_SELECT_WINDOWS
   ```

3. Add to process_record_user():

   ```
     case CU_SELECT_MACOS:
     case CU_SELECT_WINDOWS:
       if (record->event.pressed) {
         os_set_from_keycode(keycode);
       }
       break;
   ```

**How to make OS selection persistent**

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
  os_set_raw(user_config.os_selection);
}
```

(If you already have this function, add the last line shown above.)

Also change the code in process_record_user() to update the EEPROM on a keypress:

```c
  case CU_SELECT_MACOS:
  case CU_SELECT_WINDOWS:
    if (record->event.pressed) {
      os_set_from_keycode(keycode);
      user_config.os_selection = os_get_raw();
      eeconfig_update_user(user_config.raw);
    }
    break;
```

**OLED Support**

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

**Firmware size**

In my Kyria keymap, OS Shortcuts adds about 650 bytes to the size. Using the shortcuts does add some additional size compared to using hardwired codes, so the actual size difference will vary. For example, with extensive use of shortcuts through my keymap, the difference is about 1k.

