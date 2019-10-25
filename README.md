# Infinity

Infinity is a tool that enables persistent custom firmware on PSP firmware 6.60 and 6.61 for every model. 

### Installation
To install infinity you must be on either firmware 6.60 or 6.61. If you have an old version of infinity (< 2.0), then you must uninstall it first by using [Chronoswitch 7 or higher][1].

Download and extract the latest version of infinity. The extracted contents should be:
* `standard` folder containing the tool for non-PSPgo models.
* `pspgo` folder contain the tool for PSPgo models.
* `README.md` this readme file.
* `LICENSE` a copy of the MIT license covering this software.

#### Instructions for PSP 1000, 2000, 3000 and Street
Browse into the `standard` directory in the download and copy the file `EBOOT.PBP` to the `PSP/GAME/UPDATE` folder on your PSP. If this folder does not exist you may need to create it.

#### Instructions for PSPgo
Browse into the `pspgo` directory in the download and copy the file `EBOOT.PBP` to the `PSP/GAME/UPDATE` folder on your PSPgo. If this folder does not exist you may need to create it.

#### Instructions for all PSP models
Once the correct `EBOOT.PBP` file has been copied to your PSP, you can run the application to install infinity to your device. Re-running the application after installation will allow you to select which custom firmware you want enabled. You will need to run the installer for the custom firmware before enabling it within the infinity app.

Infinity supports both PRO and ME custom firmwares. Consult the installation material for theses custom firmwares for further instructions.

### Troubleshooting
If you're experiencing some issues with infinity after installation there are a few methods you can use to diagnose the problem.

#### Key-combos
Infinity has several keycombos used for diagnosing issues.

* Holding the `HOME` button on boot will start the PSP without automatically loading a custom firmware. This is useful when you accidentally have a custom firmware enabled but not installed and the PSP is crashing on boot.
* Holding `RIGHT TRIGGER + SQUARE` will boot infinity into a diagnostic mode. In the event where holding the `HOME` button doesn't work this could be a way to figure out if there is some technical issue initiating the kernel exploit. The file will be written to `infinity_error.bin` on the root of the memory stick (even on PSPgo).
* Holding `RIGHT TRIGGER + TRIANGLE` will boot infinity into a special recovery mode. This recovery mode is very basic and simply loads the file `recovery.bin` to an address specified in the file and executes it. **There is currently no recovery software included with infinity.**

#### Removing infinity
To remove infinity, please use [Chronoswitch][1] to reflash the firmware.

### Socials
Follow me on twitter [@DaveeFTW][2], and [check out my blog][3].

### License
This software is licensed under the MIT license. A copy of the license is available in the LICENSE file.

  [1]: https://github.com/DaveeFTW/chronoswitch
  [2]: https://twitter.com/DaveeFTW
  [3]: https://lolhax.org