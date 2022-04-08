## JoyCon Emulator

This program takes input from serial port and performs corresponding button and joystick actions on Nintendo Switch, by emulating a controller on a Teensy++ v2.0. It enables automation with external program that sends a pre-defined list of commands through serial port. Wired emulator offers more robustness comparing to Bluetooth-based solutions, making it a better fit for automations that may run for extended period of time without human intervention.

#### How to use

- Set up the Nintendo Switch in dock mode. 
- Connect the USB-TTL serial cable between the controlling PC and Teensy.
- Launch the game on Switch, then press Home button. Cursor should now be on the icon of the game - stay there.
- Connect Teensy to Switch's dock with an ordinary Mini-USB to USB-A cable.
- Once connected to the dock, Teensy will perform the predefined action list to set itself up as a controller in Switch's setting menu. Otherwise it loses control as soon as it returns from system screen (game selection) back into the game you are running. You might want to tweak the actions in the list to make sure the program can navigate from the starting point to the Controller menu - Change Grip/Order, and go back to where you were at.
- Teensy starts to accept input from serial port as soon as it finishes the controller set up.
- Syntax of command over serial port:
  ```![button][,button...]@[duration]#```

This repository has been tested using a Nintendo Switch (MOD. HAC-001), a Teensy 2.0++ and a USB-TTL serial cable with PL2303 compatible IC.

#### Compiling and Flashing onto the Teensy 2.0++
Go to the Teensy website and download/install the [Teensy Loader application](https://www.pjrc.com/teensy/loader.html). For Linux, follow their instructions for installing the [GCC Compiler and Tools](https://www.pjrc.com/teensy/gcc.html). For Windows, you will need the [latest AVR toolchain](http://www.atmel.com/tools/atmelavrtoolchainforwindows.aspx) from the Atmel site. See [this issue](https://github.com/LightningStalker/Splatmeme-Printer/issues/10) and [this thread](http://gbatemp.net/threads/how-to-use-shinyquagsires-splatoon-2-post-printer.479497/) on GBAtemp for more information. (Note for Mac users - the AVR MacPack is now called AVR CrossPack. If that does not work, you can try installing `avr-gcc` with `brew`.)

LUFA has been included as a git submodule, so cloning the repo like this:

```
git clone --recursive git@github.com:carrotcn/joycon-emulator.git
```

will put LUFA in the right directory.

Alternatively download the latest version from the [LUFA website](http://www.fourwalledcubicle.com/LUFA.php) and extract the content under lufa/LUFA.

Now you should be ready to rock. Open a terminal window in the `joycon-emulator` directory, type `make`, and hit enter to compile. If all goes well, the printout in the terminal will let you know it finished the build! Follow the directions on flashing `Joystick.hex` onto your Teensy, which can be found page where you downloaded the Teensy Loader application.

#### Thanks

Thanks to bertrandom for his [Snowball Thrower](https://github.com/bertrandom/snowball-thrower), Shiny Quagsire for his [Splatoon post printer](https://github.com/shinyquagsire23/Switch-Fightstick) and progmem for his [original discovery](https://github.com/progmem/Switch-Fightstick).
