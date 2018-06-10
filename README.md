# Arduino based yogurt thermometer
Will alert when milk has heated up to 90C (194F).

Will also alert when milk has cooled down to 51C (123F).

Device has been tested with a standard temperature probe found in Polder 362-90 thermometer
![Polder 362-90](https://github.com/lovegandhi/arduino-yogurt-thermometer/blob/master/images/polder.jpg)

### Current design on a prototype board:
![schematic on prototype board](https://github.com/lovegandhi/arduino-yogurt-thermometer/blob/master/images/prototype-board.png)

![prototype board finished](https://github.com/lovegandhi/arduino-yogurt-thermometer/blob/master/images/prototype%20front%20annotated.JPG)

1. Arduino Pro Mini 5v 16MHz
2. USB Boost Converter 3.7v to 5v DC
3. P-Channel Mosfet IRLML2244TRPBF SOT-23 package solderd to an smd to dip board
4. N-Channel Mosfet IRLML6244TRPBF SOT-23 package solderd to an smd to dip board
5. 18650 battery protection board
6. 18650 battery in a holder (pulled from old laptop battery)
