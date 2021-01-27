# UserFX - Add Your Own Effects to WLED

UserFX lets you add one or more header files, each header describing a single custom effect to add to WLED.  There are currently two "slots" for UserFX effects you can add to WLED (more will be added in the future).

## Include an Example Effect

There are two simple effects included for testing: `Example_Blink2.h` and `Example_Blink3.h`.  These are copies of the "Blink" pattern built into WLED with unique names, just to show how the concept works.

To add the "blink 2" effect to WLED:

Define ENABLE_USERFX to enable the UserFX code in WLED.  Either add to `my_config.h`:

`#define ENABLE_USERFX`

or Add to PlatformIO Config:

`-D ENABLE_USERFX`

Now open `Example_Blink2.h` and read through the code.  The effect's name is "BLINK2".  To assign this custom effect to UserFX slot number 1 make definitions for the path and filename of the header file, and the effect name.  Add to myconfig.h:

```
  #define USERFX1_H "../usermods/UserFX/Example_Blink2.h"
  #define USERFX1_NAME BLINK2
```

or Add to PlatformIO Config:

```
  '-D USERFX1_H="../usermods/UserFX/Example_Blink2.h"'
  -D USERFX1_NAME=BLINK2
```

Now compile and upload WLED and you should see a new "blink 2" effect name at the bottom of the Effects list.

## Add a New Effect

- Copy `Example_Blink2.h`
- Decide on new names to replace:
	- BLINK2
	- mode_blink2
	- "blink 2"
	- "Example_Blink2.h"
- Replace those names
- Add defines to compile in the new effect
- Compile and upload and make sure the new effect shows up in the Effects list and works (blinks)
- Now edit the new function to have the functionality you want

## Issues

Sometimes UserFX changes don't get picked up by the compiler (seen with PlatformIO), as they are being made in an included file.  Quick workaround: make a whitespace edit to FX.h, so the compiler knows to recompile looking for changes to that file.

TODO: Will changes be picked up if the header file is put directly into the wled00 directory?  Or to add a `#include` to the filename directly to FX.h?
