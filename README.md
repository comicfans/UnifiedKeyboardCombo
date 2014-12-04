Unified Keyboard Combo 

overview:

Have two keyboards and want to use one-per-hand , combine them as a big 
virtual keyboard ? Unified Keyboard Combo is just for you, which powered by
Linux's evdev and uinput API.

what I mean "Unified Combo" is not only "keys from any kbd just appears",
but also features:

  1. per-keyboard key remapping, etc. map kbd1's 'W' => 'K' but kbd2's 
     'W' => 'P'  at the same time , so you can use same input area 
     of two kbds simultaneously , or even combine two numeric pads as 
     alphanumeric keyboard ! also, you can remap any function key including
     ctrl,alt,shift,meta, or capslock.

  2. function keys (especially capslock, shift) across different physical kbds
     work as one unified keyboard, pressing 'shift'  key on left kbd also 
     gives right kbd's char uppercase ,combined physical kbd just appears as 
     one unified logic kbd .
     
  3. input physical devices not limited to keyboard. it can be anything which
     can produce 'key' like event (technically any evdev node which supports 
     EV_KEY), such like mouse button, joystick button , or even ... poweroff    
     button (press power button to input a char ,crazy ?) the key mapped to 
     can also be any of joy button, mouse left/right button ,sort of.

  4. dynamically configuable, hot-plug awared. UnifiedKeyboardCombo watches 
     the config file , reload it automatically. disconnect any devices without
     any worry, everything still works. when plugined it back ,everything back.
     UnifiedKeyboardCombo uses "matcher" to determine which devices should be 
     used, support common conditions (etc. VID/PID or even topology info) wild 
     match and simple relationship combine (AND/NOT/OR). even if you have two 
     exactly same keyboards, you can still enjoy per-device remapping.
  
  5. config file is just simple json file, you can edit it as you wish , or 
     fire browser, open the gui config html (no server needed ,just local page 
     with some js) to generated a new config, overwrite the old one, done.
     
  6. works in evdev and uinput level ,works no matter in X11, kernel termianl,
     or wayland .

  7. can config to drop none-mapped keys (per-device config),if you only use
     some area of whole kbd, but don't care none-mapped keys triggered. also 
     can drop none-key event from device .useful when input is mouse or 
     multi-media keyboard (mouse/trackpad function intergrated) ,you only need
     their key event, not their move event .


implementation:

  UnifiedKeyboardCombo 's works as follows: (for matched input device)


mapped key event -------> UnifiedVirtualCombo kbd

unmapped key event ----disableUnmappedKey=false---->  UnifiedKeyboardCombo kbd 

unmapped key event ----disableUnmappedKey=true---->  drop

non-key event --- disableNoneKeyEvents=false ------> shadow device of input 

non-key event --- disableNoneKeyEvents=ture ------> drop


  
   UnifiedKeyboardCombo uses evdev to read physcial events, to 'consume' 
mapped original key (makes it invisible to any others),evdev file (match ones
only) is opened exclusive (CGIOGRAB), all events from it eaten by us ,mapped 
result or non-dropped key will be sent to uinput created "UnifiedKeyboardCombo
 virtual keyboard" device(only one) .

   for each match device which supports none key event (and not configed as 
disable),we create a corresponding shadow device for it(one per device), pass 
through none key events through it. for example, you mapped the mouse left 
button to key 'Q', while using left button as 'Q',you can still moving cursor 
since it's move events passthrough the shadow device.




background:
   as programmer, sometimes I'm hiting my keyboard 10 hours per day, makes my
finger and showder uncomfortable very much. so I came to find a better kbd...
first comes the machine keyboard, as I tried, they have very good finger depth
and hiting feeling, but only a few considered ___ except some expensive ones.
such as TEK from japan, or ___ . they 

reference:

