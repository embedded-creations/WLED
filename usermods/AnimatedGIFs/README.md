# AnimatedGIFs Usermod

## Compiling

- Depends on UserFX and FL_Wrapper to be enabled
- define `USERMOD_ANIMATEDGIFS` in my_config.h or PlatformIO config
- Add the AnimatedGIFs_Effect using UserFX, e.g.

```
#define USERFX2_H "../usermods/AnimatedGIFs/AnimatedGIFs_Effect.h"
#define USERFX2_NAME AGIFS_TEST
```

- Add this line to `my_config.h`
	- `#include "../usermods/AnimatedGIFs/animatedgifs_common.h"`
	- This is a bit of a hack right now, the UserFX AGIFS_TEST effect needs to access a usermod header file, but usermod header files aren't normally exposed to FX.h
- If you want to add GIFs that are compiled into the WLED binary:
	- Edit usermod_animatedgifs.h adding GIFs in C header file format, and adding references to the GIF content in the lists (see top of the file)
		- There's limited space, not much room for many GIFs
- If you want to enable SD card support:
	- define `AGIFS_USE_SD` in my_config.h or PlatformIO config
- You should see a "*2D AnimatedGIFs" effect added towards the top of the effects list once compiled and uploaded

## Usage

- On first boot, the usermod adds config values to cfg.JSON
- defaults:

```
	"um": {
		"agif": {
			"vsW": 64,
			"vsH": 64,
			"dtSec": 10,
			"numCyc": 100,
			"playMode": 0,
			"sdClk": -1,
			"sdMiso": -1,
			"sdMosi": -1,
			"sdCs": -1
		}
	}
```

- "vsW" - virtual screen width, the width of the virtual screen buffer used to the decode the GIF
	- Set to the max width of the 2D segment(s) displaying the AGIFS_TEST effect
	- A larger than necessary value just costs more memory and potentially more CPU (more pixels can be decoded)
- "vsW" - virtual screen height, the height of the virtual screen buffer used to the decode the GIF
	- Set to the max height of the 2D segment(s) displaying the AGIFS_TEST effect
- "dtSec" - display time in seconds - how long do you want to display the GIF before cycling to the next GIF?
- "numCyc" - number of cycles to play the GIF before playing the next GIF
- "playMode" - the logic that will determine when to play the next GIF
	- playModeFirst = 0 - play the next GIF after "dtSec" OR "numCyc" is reached, whichever comes first
	- playModeBoth = 1 - play the next GIF after "dtSec" AND "numCyc" is reached
- "sdClk", "sdMiso", "sdMosi", "sdCs"
	- Pins used to connect with SD card, leave as -1 if not using SD
	- "sdCS" - chip select is optional, set to -1 to disable (and connect the SD card's CS pin to ground)
	- This pinout is working with the M5Stack Atom Lite:

```
		"sdClk": 23,
		"sdMiso": 33,
		"sdMosi": 19,
		"sdCs": -1
```

- Edit cfg.json (using the `/edit` filesystem editor feature), save, and reboot for settings to have effect

### Storing GIFs in LittleFS

- Load GIFs using the `/edit` filesystem editor built into WLED
- Nested folders aren't working properly with the `/edit` filesystem editor, so store GIFs in the root folder instead
- The Usermod is configured to load GIFs from Memory (compiled in to the usermod), LittleFS, and SD (if enabled) on boot, and will print out debug messages with the names of the files seen
- Reboot after adding or removing GIFs in LittleFS

### Storing GIFs in SD

- Store GIFs in a `/gifs` folder in the root of the SD card
- The Usermod is configured to load GIFs from Memory (compiled in to the usermod), LittleFS, and SD (if enabled) on boot, and will print out debug messages with the names of the files seen
- If your SD card isn't recognized ("Card Mount Failed...") after working previously, you may need to increase the `SD_NUM_TRIES` value in the code, and or repeatedly reset.  If that still doesn't work, try removing the SD card and connecting it again before resetting, or power cycling (with a long pause to let capacitors drain).
	- There's an issue with the ESP32 SD library where a reset while the card was active can keep it from loading properly after reset.

### Using Effect

The intensity slider can be used to select the index of the next GIF.  When the slider changes, the new index is sent to the usermod, and a flag is set to change the gif on the next frame

## Todo

- GIF playing can prevent deleting that file from /edit
- Deleted files (or added files) can screw up enumeration, force re-enumeration upon
	- Certain types of file errors
	- Flag set after LittleFS number of files changes?
- Disable GIF Decoding when not needed?
	- Starting a GIF effect can start the decoder
	- How to disable, after timeout with no request for virtual screen?
	- Enumerate when the decoder starts?

