# Compose key - QMK notes

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

The QMK Leader key requires that you type quickly after pressing it, otherwise it times out and you have to start again. Also, each leader entry takes quite a lot of space (about 70 bytes), which can be an issue if you have a long list of items and are short on flash space. Finally, Leader must be its own key, you can't combine it with e.g. Mod-Tap.

I wanted to see if I could address these. I also wanted to be able to display information on the current sequence on the OLED.

***This is a very preliminary implementation.***

## Differences to Leader

1. There is no timeout. You can cancel the sequence by pressing the Compose key again.
2. The output code is emitted as soon as the sequence matches. Therefore, you can't have both sequences EG and EGG input sequences (for example).
3. You need to define the sequence as a tree. See compose_tree.c for an example. 

## LIMITATIONS/TODO

1. The output codes used in from the default codes are Mac (only). Needs to use [OS Shortcuts](../../../../keyboard-notes/tree/master/qmk-os-shortcuts).
2. Add an example for generating diacritics.
3. Want to generate a "mod-tap" key that works with Compose.
4. Have the OLED display candidate characters at each step in the sequence.
5. (Maybe) Write a script to convert a human-readable set of strings into a compose tree.

# Usage

## How to add Compose Key to your keymap

Note: the following assumes that you have a keymap.h file which is included by keymap.c and contains the definition of the `custom_keycodes` enum. The code can be obtained from my [Kyria keymap](../../../../keyboard-firmware/tree/master/kyria-rsthd-prime).

To add OS Shortcuts to your keymap:

1. Drop the files compose_key.c, compose_key.h and compose_tree.c into your keymap folder.

2. Include compose_key.h in your keymap.h:

   ```c
   #include "compose_key.h"
   ```

3. Add the following to your rules.mk:

   ```cmake
   COMPOSE_KEY = yes	# Enable the custom compose key feature
   ...
   ifeq ($(strip $(COMPOSE_KEY)), yes)
   	SRC += compose_key.c compose_tree.c
   	OPT_DEFS += -DCOMPOSE_KEY
   endif
   ```
   
6. Add the compose key to your `custom_keycode` enum:

   ```c
   #ifdef COMPOSE_KEY
     CU_COMPOSE,
   #endif
   ```
   
5. Add the above key to your keymap.

6. Add the shortcut codes, or your #defined equivalents, to the keymap.

9. Add the following **before** the main switch in process_record_user():

   ```c
     // First check to see if we intercept for a compose sequence
   #ifdef COMPOSE_KEY
     if (compose_key_intercept(keycode, record)) {
       return false;
     }
   #endif
   ```

   This intercepts the normal processing sequence to capture keys in the compose sequence.

10. Add the following inside the main switch in process_record_user():

   ```c
       // Toggle compose mode on or off
   #ifdef COMPOSE_KEY
     case CU_COMPOSE:
       process_record_compose(keycode, record);
       break;
   #endif
   ```

   This turns the compose capture on and off.

11. Edit compose_tree.c to suit yourself. My version should demonstrate how to use the macros to write the tree. For output of than a single keycode, use the callback. **Make sure** that each node of the tree ends in COMPOSE_END.

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

## OLED Support

If you have an OLED, the Compose key info can be displayed by adding something like this to your `oled_task_user` function: 

```c
...
#ifdef COMPOSE_STATUS_ENABLE
    compose_key_status();
#endif
...
```

In addition, add this to config.h:

```c
#define COMPOSE_STATUS_ENABLE
```

# Firmware size

With about 20 sequences in a tree of 5 nodes, Compose Key uses 370 bytes without OLED support. With OLED enabled, it uses an additional 300 bytes. If you use SEND_STRING() in your compose callbacks, that will add at least 500 bytes. In comparison, Leader with 12 sequences (and no SEND_STRING) uses 1080 bytes. With more sequences, the difference will be higher.

