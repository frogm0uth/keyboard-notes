# Layer-tap-toggle - QMK notes

Layer-tap-toggle is my way of getting a uniform layer switching method on my QMK keyboards.
<!--ts-->
   * [Layer-tap-toggle - QMK notes](#layer-tap-toggle---qmk-notes)
      * [Rationale](#rationale)
   * [Usage](#usage)
      * [How it works](#how-it-works)
      * [How to add to your keymap](#how-to-add-to-your-keymap)
      * [Removing](#removing)
   * [Firmware size](#firmware-size)

<!--te-->

## Rationale

QMK provides a range of keycodes for layer switching - MO, TG, TT, LT, and OSL. However, I found that I ended up with a mish-mash using more than one of these, which then confused me. I felt that what I really wanted was a single hybrid of LT and TG.

Therefore, the layer-tap-toggle code here firstly allows you to hold to activate a layer or tap to get a keycode – it is like LT except that the layer activates immediately with no lag. In addition, you can toggle the layer on by holding Cmd/Gui when activating it, or lock it later with a dedicated "lock" key. Finally, you can use the same key to activate a different layer, by having Ctrl held when pressing it.

As a side-benefit, the tap keycode can be any keycode, it's not limited to just basic keycodes.

# Usage

## How it works

Once you have everything set up:

1. Tap the key to produce the tap keycode. In the example above, tapping the NumPad layer key produces Enter.
2. Hold the layer key and the layer will be activated while it's held.
3. Hold Ctrl, then hold the layer key and a second different layer will be activated while it's held.
4. Hold Cmd/Gui, then tap the layer key to toggle/lock the layer.
5. Hold the layer key, then while holding, tap a "lock" key defined in that layer to lock the layer on. Then you can release the layer key.
6. If the layer is toggled/locked, press the layer key to deactivate it.
7. Press the CU_BASE key to deactivate all layers.

## How to add to your keymap

Note: the following assumes that you have a keymap.h file which is included by keymap.c and contains the definitions of the `layers` and `custom_keycodes` enums. The code can be obtained from my [Kyria keymap](../../../../keyboard-firmware/tree/master/kyria-rsthd-prime).

To add layer-tap-toggle to your keymap:

1. Drop the files layer_tap_toggle.c and layer_tap_toggle.h into your keymap folder.

2. Include layer_tap_toggle.h in your keymap.h:

   ```c
   #include "layer_tap_toggle.h"
   ```

3. Add a NUM_LAYERS field to the very end of your `layers` enum:

   ```c
   enum layers {
       ALPHA = 0,
       ...
       NUM_LAYERS  // the number of layers
   };
   ```
   
4. Add the following to your rules.mk:

   ```cmake
   LAYER_TAP_TOGGLE = yes	# Enable the layer-tap-toggle feature
   ...
   ifeq ($(strip $(LAYER_TAP_TOGGLE)), yes)
   	SRC += layer_tap_toggle.c
   	OPT_DEFS += -DLAYER_TAP_TOGGLE
   endif
   ```

5. Add layer keys to your `custom_keycode` enum. In this example, CU_BASE will switch back to the base layer, CU_LLOCK is the layer lock key (if you want one), and CU_NUMPAD and CU_MOUSE switch to the NUMPAD and MOUSE layers:

   ```c
   enum custom_keycodes {
     CU_IGNORE = SAFE_RANGE,
     ...
     CU_LLOCK,
     CU_BASE,
     CU_NUMPAD,
     CU_MOUSE,
     ...
   }
   ```

6. Add the layer keys to your keymap. Layer keys like CU_NUMPAD and CU_MOUSE are usually in the base layer. In the corresponding layer, those locations must be transparent. CU_LLOCK and CU_BASE are optional but if used should be added in the higher layers. CU_LLOCK locks the current layer. CU_BASE deactivates all layers i.e. you go back to the base layer. The latter is useful if you have a lot of layers: put it in the same location in every layer, then you can always get back to the base layer without having to remember which layer you are in.

7. Add code to process the keys to process_record_user(). The first argument to the layer_tap_toggle() function is the key to send on tap, the second is the layer to activate on hold or shift-tap. If you don't want a tap key, provide KC_NO as the first argument. Here's an example:

   ```c
   #ifdef LAYER_TAP_TOGGLE
         // Activate layers with layer-tap-toggle
     case CU_NUMPAD:
       return layer_tap_toggle(KC_ENTER, NUMPAD, record);
       break;
       
     case CU_MOUSE:
       return layer_tap_toggle(KC_NO, MOUSE, record);
       break;
   ...
         // Lock the current (highest) layer
     case CU_LLOCK:
       return ltt_lock(record);
       break;
       
       // Clear all layers and go back to the default layer
     case CU_BASE:
       return ltt_base();
       break;
   #endif
   ```

8. If you want to activate an alternate layer with Ctrl, call layer_tap_toggle2() instead:

   ```c
    case CU_NUMPAD:
       return layer_tap_toggle2(KC_ENTER, NUMPAD, MOUSE, record);
       break;
   ```
   
9. Add this call to your matrix_scan_user() function:

   ```c
   #ifdef LAYER_TAP_TOGGLE
     ltt_update_timer();
   #endif
   ```

## Removing

If you decide you don't want to use layer-tap-toggle any more, simply change the line in rules.mk:

```
LAYER_TAP_TOGGLE = no
```

You will also need to remove the keycodes from your keymap i.e. change them to normal QMK layer switching codes. Alternatively, in your keymap.h, define macros to substitute for the CU_..... keycodes (see my Kyria keymap for an example).

# Firmware size

With my Kyria firmware, enabling layer-tap-toggle adds about 500 bytes to the firmware size. However, since I implemented layer-tap-toggle, I have been able to disable all QMK tapping features, which saved almost 2 kbytes.

