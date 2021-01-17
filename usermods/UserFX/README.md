# UserFX - Add your own effects to WLED

There are two "sets" of UserFX effects you can add to WLED (it's easy to add more if two are not enough).  Each set has two header files, one to hold code (`.cpp.h`) and one to hold info on the modes to include into `FX.h` (`.h`)

## To include the effects in set UserFX1

Define `USERFX1_H` with the pathname of to `UserFX1.h`, and `USERFX1_CPP_H` with the pathname of to `UserFX1.cpp.h`.  Add to myconfig.h:

```
#define USERFX1_H       "../usermods/UserFX/UserFX1.h"
#define USERFX1_CPP_H   "../usermods/UserFX/UserFX1.cpp.h"
```
or Add to PlatformIO Config:

```
'-D USERFX1_H="../usermods/UserFX/FX_user.h"'
'-D USERFX1_CPP_H="../usermods/UserFX/FX_user.cpp.h"'
```

## Add custom effect to a set

- Add a new mode function in the `.cpp.h` file, following the existing example
- Edit the `.h` file to add the new mode, increasing the count, adding a new definition, map entry, add the mode function to the list, and add the mode name to the JSON array
