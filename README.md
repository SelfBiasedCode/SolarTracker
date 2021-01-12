# SolarTracker
SolarTracker is a project in which a solar panel is moved automatically to ensure maximum efficiency. It is designed to track the position of the sun while also allowing for manual adjustments and experiments. Limit switches on each axis provide both hardware-level safety and feedback to the user via a simple UI.

## Technical Details
 A 2x2 array of LDRs is read with the Atmel's ADCs to to determine the position of the sun. Smoothing is applied, then the directional errors (azimuth and elevation) are fed to one P-controller per axis, each of which actuates one side of a L298N motor driver. PWM ramping, shadow detection, an input deadband ensure smooth and jerk-free operation for mechanically larger setups. Ontop of that, a minimum PWM setting prevents noise during small adjustments. The limit switches feature an electric circuit that allows the Atmel to read their state, at the same time shutting down the L298N's directional input if actuated. This means that even with the Atmel malfunctioning, the axes will not move past their limit switches.
 This component features multiple optimizations to work around Arduino library limitations. It comes with its own low-level code for reading digital pins to avoid the horribly slow digitalRead() function and is, just like the rest of the code, mostly optimized for performance. RAM and Flash footprint were deemed secondary and can be optimized if necessary, e.g. by disabling caching. 

## UI
A display shield is used, featuring a 1602 LCD display and five buttons plus a reset button. Only one button can be read at a time as the buttons use a resistor array to output a button-specific voltage on pin A0.
The UI features multiple modes as described below.

### Manual Mode
This is the startup state. It allows the user to freely control the position of the panel by pressing the UP, DOWN, LEFT or RIGHT button. The status of the limit switches and the measured azimuth and elevation errors are shown on the display.
By holding SELECT, the user can switch to Auto Mode.

### Auto Mode
This is the main mode of SolarTracker. When activated, it will follow the brightest light source by constantly adjusting both axes. The current direction and speed of both axes are shown on the display.
By pressing UP, DOWN, LEFT or RIGHT the user can go back to manual mode. By holding SELECT, the system will go into test mode.

### Test mode
In Test Mode, the raw values of the LDRs and the status of the limit switches are shown on the display. Movement is disabled in this mode.
By holding SELECT, the system will go back into Manual Mode.

## Known Issues
- Limit switch states are cached in Manual Mode. If, for example, one axis is moved in positive direction until it actuates the limit switch, then moved in the opposite direction, the positive limit switch will still shown to be actuated. This is because the positive limit switch state is only polled during positive movement.
