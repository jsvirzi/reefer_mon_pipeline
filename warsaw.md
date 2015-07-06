This is the daughterboard containing the IMU/GPS/LTE interface for the Nauto POLAND offensive.
All related documents are here, except for large binary files.

# BOM

## Gyroscope
The gyroscope is the L3GD20H from ST. We are mounting a breakout board available from [Adafruit](http://www.adafruit.com/products/1032). The interface is I2C.

## Accelerometer and Magnetometer
This is the LSM303 from ST. We are also mounting a breakout board available from [Adafruit](https://www.adafruit.com/products/1714). The interface is I2C. This is the [datasheet]()

## Temperature
We are using the MCP9808 from Microchip. Breakout board from [Adafruit](https://www.adafruit.com/products/1782).
The interface is I2C. This is the [datasheet]()

## GPS
MTK3339. Breakout board from [Adafruit](https://www.adafruit.com/products/746).
The interface is RS-232, 8N1, configurable baud rate but using 115200 bps. This is the [datasheet]()

## LTE
SIM800 from SIMCOM. Breakout board from [Adafruit](https://www.adafruit.com/products/1963).
[This](http://www.adafruit.com/products/1858) is the proposed antenna, which is a low profile right angle mount antenna. ![Image of Antenna](https://github.com/nauto/hardware/blob/master/boards/warsaw/images/warsaw_antenna.jpg)
The intention is to have this antenna take some airspace over the Warsaw board itself.
The device is 2G; the SIM card can be obtained from [Adafruit](http://www.adafruit.com/products/2505).
Forward looking designs should use at least 3G such as the SIM5320 which can be found [here]().
We have these devices [T5320A+G](http://wm.sim.com/producten.aspx?id=1053) in use but they have their own enclosure, 
potentially adding to the product footprint.

## Temperature/Humidity
SHT15. Breakout board from [Adafruit](https://www.adafruit.com/products/1638).
The interface is I2C. This is the [datasheet]()
