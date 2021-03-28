# ArduinoCore-samc
Arduino Core for the SAMC21 microcontroller, specifically SAMC21G18A and SAMC21N18A, found on the FortyTwo systems Uno and Mega boards, respectively.

- bootloaders -> Microchip Studio projects that compile into bootloaders for the boards.
- core -> Arduino IDE compatible core, based on the SAMD21 core.
- libraries -> copied from SAMD21, no modifications
- microchipstudio -> debuggable versions of the core in Microchip Studio.
- variants -> board-specific files

## Microchip Studio Debugging
To debug using Microchip Studio, find the core.a file in the temporary Arduino IDE build folder. Then copy it to the microchipstudio->fortytwo_*** folder 
and rename it libcore.a. You can now debug the core! Make sure to recompile the core and copy it over when you've made any changes to the c or cpp files.

## Contributing
If you'd like to contribute, please submit a pull request. Thanks! 
