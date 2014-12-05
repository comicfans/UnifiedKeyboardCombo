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
     work as one unified keyboard, pressing 'shift'  key (even this is a 
     mapped shift key) on left kbd also gives right kbd's char uppercase.
     combined physical kbd just appears as one unified logic kbd .
     
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
     
  6. at evdev and uinput level ,works no matter in X11, kernel termianl, or 
     wayland .

  7. can config to drop none-mapped keys (per-device config),if you only use
     some area of whole kbd, but don't care none-mapped keys triggered. also 
     can drop none-key event from device .useful when input is mouse or 
     multi-media keyboard (mouse/trackpad function intergrated) ,you only need
     their key event, but not their move event .


build and run requirements :

  for easy testing ,precompiled binary for i386 and x86_64 are provided. 

  running: linux with evdev uinput kernel support (almost every distro did),
           libevdev ,fedora : sudo yum install libevdev 
                     ubuntu : sudo apt-get install libevdev2
                     (pre-compiled binary linked with version 1.3 libevdev.so.2)
           put example ukc.json and binary into same folder, run it as root 
           (or user has R/W permission of /dev/uinput and /dev/input )
           (you should run it from PWD, this is a shortcoming which will be 
            improved later)

  building:
  
           libevdev (fedora libevdev-devel, ubuntu libevdev-dev), c++11 
           compiler, boost (header only library is enough), use cmake to build. 

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


   UnifiedKeyboardCombo use inotify to monitor its config file ukc.json in PWD 
to do auto reload. if it's not exist, moved or deleted, UnifiedKeyboardCombo 
will auto turn to monitor PWD , if config file re-appears, turn back to 
monitor config file. newly connected devices are also be monitored by inotify,
maybe udev is better for device plug in/out, but as I tried ,inotify+libevdev 
is enough . 

   SIGINT or SIGTERM to exit the daemon.

   UnifiedKeyboardCombo is a simple program so I use bare epoll to do main 
loop ,no need to worry about it eats all your cpu.

weakness, TODO:

   grab physical input device is bad ideal (libevdev said) because incorrect 
match config may grab unexcepted input device ,any physical input may be 
dropped , you can telnet/ssh then kill the daemon makes everything back. I 
recommended not turn on disableNonKeyEvent and disableNonmappedKey (they are 
off by default) to test out your new config first.maybe we should add a 'magic'
key sequence to let daemon release the device.

   grab physical input device is also good ,since you can enjoy all functions
no matter if you work with terminal ,X11, wayland, or any embedded system.

   didn't deal with SYN_DROP currently, should be improved
   
        

background:
   as programmer, typing 8 hours every day makes my finger and shoulder 
uncomfortable very much. so I came to find a better kbd... first comes the 
mechanical keyboard ,  they have better type feel but unfortunately none 
considered Ergonomic,  except ... some most expensive ones, for example “Truly 
Ergonomic keyboard”/μTRON from Japan,Kinesis's Advantage/freestyle2, Ergodox .
I found their's "Ergonomic" design improve almost falls into 2 categories:


    1. keys splited as 2 groups for hand separation , to resolve hand angle
       issue (bending the wrist outward to the sides, or Ulnar deviation)

    2. optimized layout , centralize keys together to reduce finger movement,
       many keys can be pressed by thumb (enter, or ctrl/alt in emacs) . 
       further support hardware level key mapping, you can further customize 
       it as you wish. (remap the key under thumb as 'ESC', makes vim easier)

   so here comes the ideal : why not combine two normal keyboards together to 
make better typing  experience? map used keys under and around fingers to what 
I want, so I got a "Ergonomic" kbd! after some google, I found 'evmapd' and 
'x11-input-evdev-remap' fit my desire some sort of , but not all.(evmapd only 
work with one device, x11-input-evdev-remap can not easily block unwant keys. 
correct me if any please) since evdev+uinput provides every bits for my needs, 
after some hack,here comes the UnifiedKeyboardCombo.

   2 kbds of 104 keys are too space consumed, the only advantage is that 104 
kbd are most common and cheap (even with mechanical switch , I don't know why)
for programmers, numeric pad in 104 is almost useless , but you can buy one a
seperate numeric pad and use it as one input kbd ,such as x-key products. 
Logitech G13 seems fit my need, but it's too expensive. plus I found someone 
complain the driver problem under linux. finally I found a G13 clone, Delux T9.
it lacks the advance function of G13, but has basic key input function 



of course some ergonomic keyboard also featues physcial improve including:
 
    1. palm higher than finger tips ,so fingers naturally dip in and rest on 
       keys . I havn't tried this type (such as MS 4000) ,but I used to lay
       my lower arm on table ,so seems higher finger (not too much) is not a 
       big problem to me.

    2. key columns not staggered,fingers moves in a straight line. actually 
       this is improve combined with " hand separation" . with 2 group keys 
       located on different kbd, you can rotate kbd to proper angle which 
       makes finger moves straight line. 

   
rethinking the keyboard:



windows implementation:
  at very beginning , I'm trying to implements core function as cross-platform,
but after digging I found windows so much different from Linux. if want to 
work as low level as in Linux, the only choice is a upperfilter driver.but 
it's so complicated that I can't guess any meanings from even one line of MS 
example code. I have to failback to windows hook ... but MS makes me crazy ...
another time. 

    you need to modify key message posted to other application,so must hook 

    you can hook key message, but key message has no HW information--failed. 
    rawinput API gives key message with HW information ,but can't be hooked... 

    What The ? when combine them together in standalone app, you will even 
find rawinput and key message are not stack based, means it's not possible to 
avoid key event by drop rawinput ! these 2 event do take same keycode, but 
there's no chain relationship between them.

    some one at codeproject provide a workaround : use app to receive rawinput
background ,use hook to preprocess key event, send it back to background app 
to determine if it should be filtered, but it suffer from problem that 2 type
messages reaching order not determineded (they comes from different thread 
message queue) so sometimes you have to wait ... 

    but in inspired by him, I created a improved version , firstly , hook on 
WH_GETMESSAGE, so we can get all type message of target application including 
rawinput and key, secondly, we register rawinput message in the hook, fakes 
that target application will receive rawinput message. none-stack problem 
still exits, but at least got stable message order (rawinput always before key)
we assume that target application didn't use rawinput (if not ,out logic will 
be too complicated) , we pre-process the rawinput message and filter it out ,
so target application will not notify this changes .

    for gui programming, windows predefined message queue and standard hook 
seems more convient ,but for program like UnifiedKeyboardCombo ,Linux is 
absolutely winner. per-device event is natural because you read per-device ,
virtual input device creation and virtual event generation is just write some 
bits to uinput, crazy simple!



reference:


