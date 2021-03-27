# README

## How to install Xcpc

Please read the file [doc/INSTALL.md](doc/INSTALL.md).

## How to run Xcpc

```
Usage: xcpc [toolkit-options] [program-options]

Help options:
    --help                      display this help and exit
    --version                   display the version and exit

Emulation options:
    --model={value}             cpc464, cpc664, cpc6128
    --monitor={value}           color, green, monochrome, ctm640, ctm644, gt64, gt65, cm14, mm12
    --keyboard={value}          qwerty, azerty
    --refresh={value}           50Hz, 60Hz
    --manufacturer={value}      Isp, Triumph, Saisho, Solavox, Awa, Schneider, Orion, Amstrad
    --sysrom={filename}         32Kb system rom
    --rom000={filename}         16Kb expansion rom #00
    --rom001={filename}         16Kb expansion rom #01
    --rom002={filename}         16Kb expansion rom #02
    --rom003={filename}         16Kb expansion rom #03
    --rom004={filename}         16Kb expansion rom #04
    --rom005={filename}         16Kb expansion rom #05
    --rom006={filename}         16Kb expansion rom #07
    --rom007={filename}         16Kb expansion rom #08
    --rom008={filename}         16Kb expansion rom #09
    --rom009={filename}         16Kb expansion rom #10
    --rom010={filename}         16Kb expansion rom #11
    --rom011={filename}         16Kb expansion rom #12
    --rom012={filename}         16Kb expansion rom #13
    --rom013={filename}         16Kb expansion rom #14
    --rom014={filename}         16Kb expansion rom #15
    --rom015={filename}         16Kb expansion rom #16
    --drive0={filename}         drive0 disk image
    --drive1={filename}         drive1 disk image
    --snapshot={filename}       initial snapshot

Misc. options:
    --turbo                     enable the turbo mode
    --no-turbo                  disable the turbo mode
    --xshm                      use the XShm extension
    --no-xshm                   don't use the XShm extension
    --fps                       print framerate
    --no-fps                    don'tprint framerate

Debug options:
    --quiet                     set the loglevel to quiet mode
    --trace                     set the loglevel to trace mode
    --debug                     set the loglevel to debug mode

```

## Release notes

  * Xcpc doesn't have sound emulation yet.
  * You can emulate the joystick with the keypad by disabling the 'Num Lock' key.

The joystick mode can be toggled (enabled/disabled) by pressing the `End` key.
The Up/Down/Left/Right + CTRL-L & ALT-L keys will then emulate the joystick 0.

## License and legal informations

Please read the file [doc/LICENSE.md](doc/LICENSE.md).
