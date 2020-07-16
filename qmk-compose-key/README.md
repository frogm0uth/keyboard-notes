# Compose key - QMK notes

A [Compose key](https://en.wikipedia.org/wiki/Compose_key) prefixes a sequence of keys that produce something completely different. This is my implemention on QMK. It's a "dead" key like the QMK Leader, but different in ways listed below.
<!--ts-->

   * [Compose key - QMK notes](#compose-key---qmk-notes)
      * [Rationale](#rationale)
      * [Differences to Leader](#differences-to-leader)
      * [LIMITATIONS/TODO](#limitationstodo)
   * [Usage](#usage)
      * [How to add Compose Key to your keymap](#how-to-add-compose-key-to-your-keymap)
      * [How to add shortcuts to custom code](#how-to-add-shortcuts-to-custom-code)
      * [OLED Support](#oled-support)
   * [Firmware size](#firmware-size)

<!--te-->

## Rationale

The QMK Leader key requires that you type quickly after pressing it, otherwise it times out and you have to start again. Also, each leader entry takes quite a lot of space (about 70 bytes), which can be an issue if you have a long list of items and are short on flash space. Finally, Leader must be its own key, you can't combine it with e.g. Mod-Tap.

I wanted to see if I could address these. I also wanted to be able to display information on the current sequence on the OLED.

***This is a very preliminary implementation.***

## Differences to Leader

1. There is no timeout. You can cancel the sequence by pressing the Compose key again.
2. The output code is emitted as soon as the sequence matches. Therefore, you can't have both EG and EGG input sequences (for example).
3. You need to define the sequence as a tree. See compose_tree.c for an example. 

## LIMITATIONS/TODO

1. The output codes used in compose_tree.c are Mac (only). It needs to be updated to use [OS Shortcuts](../../../../keyboard-notes/tree/master/qmk-os-shortcuts).
2. In theory, it will work well for diacritics, but a working example should be added.
3. I am yet to generate a "mod-tap" key that works with Compose.
4. It would be helpful if the OLED displayed candidate characters at each step in the sequence.
5. (Maybe) Write a script to convert a human-readable set of strings into a compose tree.

# Usage

## How to add Compose Key to your keymap

Note: the following assumes that you have a keymap.h file which is included by keymap.c and contains the definition of the `custom_keycodes`  enum.

To add Compose Key to your keymap:

1. Drop the files compose_key.c, compose_key.h and compose_tree.c into your keymap folder. These can be obtained from my [Kyria keymap](../../../../keyboard-firmware/tree/master/kyria-rsthd-prime).

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

9. Edit compose_tree.c to suit yourself. My version should demonstrate how to use the macros to write the tree. To output of more than a single keycode, use the callback. **Make sure** that each node of the tree ends in COMPOSE_END.

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
