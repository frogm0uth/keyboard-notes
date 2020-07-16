# Compose key - QMK notes

A [Compose key](https://en.wikipedia.org/wiki/Compose_key) prefixes a sequence of keys that produce something completely different. This is my implemention on QMK. It's a "dead" key like the QMK Leader, but different in ways listed below.
<!--ts-->
   * [Compose key - QMK notes](#compose-key---qmk-notes)
      * [Description](#description)
      * [LIMITATIONS/TODO](#limitationstodo)
   * [Usage](#usage)
      * [How to add Compose Key to your keymap](#how-to-add-compose-key-to-your-keymap)
      * [OLED Support](#oled-support)
   * [Firmware size](#firmware-size)

<!--te-->

## Description

The Compose key initiates a keyboard mode in which no characters are output until one of a set of specified character sequences is typed. It is like the QMK Leader key, but different in these ways:

1. There is no timeout. The Compose key sequence keeps waiting for you until you complete the sequence. Or, you can cancel by pressing the Compose key again.
2. The keys in the sequence can be in any layer. So you can use punctuation keys in a different layer as part of a sequence.
3. There is no lag. That is, the key is output immediately the match is found. (While in practice this makes no real difference, the lag does prevent you from setting up Leader with a long delay to compensate for item 1.)
4. The sequence completes as soon as a match is found. You can't have both EG and EGG sequences, for example.
5. The sequence is defined as a tree. This tends to work better if you have a shallow, wide tree, whereas Leader tends to encourage completely arbitrary sequences. For example, with the Compose key I have sequences pc, pr, pt, pD, pd, pf. (The first character means "print" and the second indicates the specific symbol: copyright, registered, trademark, double dagger, dagger, and degrees.) With Leader, I have cp, reg, tm, ddag, dag, and deg. I suspect that the Compose key method works better for a large number of sequences as it naturally groups them into categories.

The Compose key is more efficient than Leader in terms of Firmware space, especially if you have more than a few sequences. See the Firmware Size section below.

Finally, the OLED displays the sequence so far and the candidate next characters. (I'm not sure how useful this is yet, but it's there...) 

***This is a very preliminary implementation.***

## LIMITATIONS/TODO

1. The output codes used in compose_tree.c are for Mac. It should be updated to use [OS Shortcuts](../../../../keyboard-notes/tree/master/qmk-os-shortcuts).
2. In theory, it will work well for diacritics, but a working example should be added.
3. I would like to come up with a "mod-tap" key that works with Compose. (Leader needs its own key, which is part of the reason I wrote my own Compose.)
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

If you have an OLED, add this to config.h:

```c
#define COMPOSE_STATUS_ENABLE
```

Add something like this to your `oled_task_user` function: 

```c
...
#ifdef COMPOSE_STATUS_ENABLE
    compose_key_status();
#endif
...
```

Finally, add this to your `matrix_scan_user` function:

```
 // Compose key status timer
#ifdef COMPOSE_STATUS_ENABLE
  compose_status_tick();
#endif
```


# Firmware size

With about 20 sequences in a tree of 5 nodes, Compose Key uses 370 bytes without OLED support. With OLED support enabled, it uses an additional 470 bytes. In comparison, Leader with 12 sequences (and no SEND_STRING) uses 1080 bytes. With more sequences, the difference will be higher.

If you use SEND_STRING() in your Compose callbacks or Leader code, that will add about 500 bytes in either case.

