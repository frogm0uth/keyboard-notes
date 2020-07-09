# Layer-tap-toggle - QMK notes

Layer-tap-toggle is my way of getting a uniform layer switching method on my QMK keyboards.

**Rationale**

QMK provides a range of keycodes for layer switching - MO, TG, TT, LT, and OSL. However, I found that I ended up with a mish-mash using more than one of these, which then confused me. I felt that what I really wanted was a single hybrid of LT and TG.

Therefore, the layer-tap-toggle code here firstly allows you to hold to activate a layer or tap to get a keycode - it is like LT except that the layer activates immediately with no lag. In addition, you can toggle the layer on by holding Shift when activating it, or lock it later with a dedicated "lock" key.

As a side-benefit, the tap keycode can be any keycode, it's not limited to just basic keycodes.

**How to use layer-tap-toggle**

Note: the following assumes that you have a keymap.h file which is included by keymap.c and has the definitions of the `layers` and `custom_keycodes` enums. If not, consider creating one. Otherwise, you may have to adapt in some other way (e.g. a brute force method would be to just copy the contents of layer_tap_toggle.c into keymap.c).

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

5. Add layer keys to your `custom_keycode` enum. In this example, CU_BASE will switch back to the base layer, CU_LLOCK is the layer lock key, and CU_NUMPAD and CU_MOUSE switch to the NUMPAD and MOUSE layers:

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

8. Add this call to your matrix_scan_user() function:

   ```c
   #ifdef LAYER_TAP_TOGGLE
     ltt_update_timer();
   #endif
   ```



If you decide you don't want to use layer-tap-toggle any more, simply change the line in rules.mk:

```
LAYER_TAP_TOGGLE = no
```

You will also need to remove the keycodes from your keymap i.e. change them to normal QMK layer switching codes. Alternatively, in your keymap.h, define macros to substitute for the CU_..... keycodes (see my Kyria RSTHD' keymap for an example).



**Summary**

Once you have everything set up:

1. Tap the key to produce the tap keycode. In the example above, tapping the NumPad layer key produces Enter.
2. Hold the layer key and the layer will be activated while it's held.
3. Hold Shift, then tap the layer key to toggle/lock the layer.
4. Hold the layer key, then while holding, tap the CU_LLOCK key to lock the layer on. Then you can release the layer key.
5. If the layer is toggled/locked, press the layer key to deactivate it.
6. Press the CU_BASE key to deactivate all layers.

