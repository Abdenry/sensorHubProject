# Sensor Hub

<p align="center">
  <img width="1280" height="720" alt="weatherStation" src="https://github.com/user-attachments/assets/38e16e49-e8b6-4397-b52e-08d43c67ff3d" />
</p>

A small, portable, self-sustaining weather station built to provide data to an external web interface hosted with Grafana via a Raspberry Pi Zero 2 W. This station is able to collect various metrics from on board sensors such as temperature, humidity, heat index, wind speed, and wind direction. A **DHT22** sensor is used to collect relative humidity and temperature metrics with an accuracy of +-2% and +-0.5C, while metrics such as wind speed and wind direction are collected with custom built sensors. The heat index is calculated in software using the [DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library).

Communication is handled through the NRF24l01 radio modules in a mesh configuration. The mesh routing of data packets is automatically managed with `RF24.h`, `RF24Network.h`, and `RF24Mesh.h` provided by the [RF24](https://github.com/nRF24/RF24?tab=readme-ov-file) library, specifically chosen for its meshing capabilities. It provides dynamic configuration of the NRF24l01 modules, meaning that in the scenario where the weather station loses connection with the Raspberry Pi for any reason (power down, range lost, etc.), it can automatically reconfigure and connect to the Raspberry Pi given its under normal operable condition (i.e. has power and is in range). This allows additional stations to be installed with potentially different purposes with all metrics collected to be routed back to the Raspberry Pi. Alternatively, if the distance between the Raspberry Pi and the weather station is much greater than the range of a single NRF24l01 module, an additional node can be configured and placed in between the weather station and Raspberry Pi acting as a range extender. The RF24 library will automatically handle the routing and pass the data collected from the weather station to the Raspberry Pi via that additional node. There is no limitation that prevents an additional station from also being a range extending node.

<p align="center">
  <img width="1126" height="371" alt="nrf24WeatherStationDiagram4" src="https://github.com/user-attachments/assets/51690f78-8cd4-4f10-8633-32393c1eef1d" />
</p>

### Wiring Diagram
<p align="center">
  <img width="1573" height="1013" alt="circuitDiagram3" src="https://github.com/user-attachments/assets/024698c7-ac21-417f-8059-7c26dc810be3" />
</p>

### CAD Model
<p align="center">
  <img width="1224" height="628" alt="weatherStationCAD" src="https://github.com/user-attachments/assets/9dd55f6e-4fa3-49e8-8538-ed2ac55cb923" />
</p>

## Sensor Design
The following sensor assemblies were all 3D printed out of white PET-G filament for its UV resistance, and to reflect as much heat as possible to avoid compromising the temperature and humidity readings of the DHT22 sensor.
### Solar Radiation Shield
The DHT22 sensor used to collect temperature and humidity data was housed in a solar radiation shield. This is necessary to prevent direct sunlight from heating the sensor over time creating higher than ambient temperature readings, simultaneously it protects the sensor from rainfall, which would also cause a higher than ambient humidity reading if it gets wet.

### Wind Vane
Wind direction was measured using eight AH180 omni-polar hall effect sensors and a magnet on the underside of the vane. Each hall effect sensor acts as one of the four cardinal or ordinal directions (North, North East, East, etc.) which is activated only in the presence of the magnet. The vane will point in the direction the wind is blowing **from**. With limited IO available on the Arduino Nano/ATmega328p the SN74HC175N Parallel In Serial Out (PISO) Shift Register was used to reduce the eight outputs of the wind vane to one output + three inputs, halving the total amount of connections to the Arduino Nano/ATmega328p.

<p align="center">
  <img width="1000" height="500" alt="windVane" src="https://github.com/user-attachments/assets/9f178057-7930-4306-a32d-2dcd017dacab" />
</p>

### Anemometer
Wind speed was measured using two magnets and a single US1881 latching hall effect sensor connected to the Arduino Nano D2 pin which corresponds to the ATmega328p external interrupt pin 0 (PD2). The magnets attached to the underside of the anemometer rotate over the US1881 sensor triggering either a high or low state depending on the magnets polarity, generating an interrupt which increments a revolution counter. The `getWindSpeed()` function is called periodically to calculate the wind speed using the amount of revolutions counted within that period, resetting the revolution counter once calculation has completed, and repeats this process. This assembly was tested by having the anemometer attached externally to a moving car, logging its calculated wind speed and comparing it to that the vehicles actual speed. It proved accurate to +-2 km/h with a vehicle speed of 80 km/h.
<p align="center">
  <img width="1000" height="500" alt="anemometer" src="https://github.com/user-attachments/assets/a5d4ef46-5419-46ed-bece-f4bc87d31a2c" />
</p>

## Grafana Dashboard
The Grafana dashboard conviently displays historical readings for temperature, humidity, wind speed, and direction. It also provides an updated reading of the stations current temperature, humidity, and calculated heat index. The dashboard has been configured to retain this information for up to two weeks however this can be further increased if desired.
<p align="center">
  <img width="1903" height="816" alt="dashboard" src="https://github.com/user-attachments/assets/b66015e0-ad85-482d-a699-5bfeac061ae0" />
  <img width="1897" height="495" alt="successfulTransmission" src="https://github.com/user-attachments/assets/e2d66b08-44b0-486b-84bd-03d61edd50ec" />
</p>

## Power Supply and Challenges Faced
Power for the station comes from a 1100 mAh LiPo battery, supported by four 1W solar panels connected in parallel. A solar power manager with Maximum Power Point Tracking (MPPT) is implemented to maximise the efficiency of the solar panels, switching between charging the station and its battery based on sunlight availability. It ensures that the battery remains charged during daylight hours, with enough capacity to keep the station operational overnight. After optimisations, the weather station consumes approximately 0.021 A at 3.72 V, providing an estimated 52 hours of operation without any solar input.

Prior to optimising power consumption, a major challenge faced during development was the intermittent loss of communications particularly over night.

<p align="center">
  <img width="1901" height="497" alt="dropOuts" src="https://github.com/user-attachments/assets/1828a394-d64a-4bb9-8ae9-6ea01c60ebe1" />
</p>

Initially, it was suspected that the issue was not related to power due to the assumption that the 4 watt solar panel array was capable of powering and charging the station which had a measured active consumption of 0.24W (65mA @ 3.7V). After investigating the issue for a significant period of time, no definitive answer could explain the intermittent communication loss overnight other than insufficient solar power. To prove this was the root of the problem the station was connected to a bench top power supply with a constant 3.7V over night. This resulted in no communication drop outs over night nor for the next couple of days as the battery was sufficiently charged from the constant 3.7 V provided by the power supply. Optimisations for the code then proceeded to lower the active consumption.

### Optimisations
The first optimisation was replacing the software timer used to intermittently send weather metrics to the Raspberry Pi to a Watchdog Timer. This did not directly improve the power consumption of the station, however, it would allow the station to wake up from a power down mode, which was the second optimisation performed.

Entering the Arduino Nano/ATmega328p into a power down mode and only powering up after the Watchdog Timer expired directly reduced power consumption from 0.24W (65mA @ 3.7V) to 0.208W (56mA @ 3.7V), a 13% power efficiency improvement.

This was further improved by 64% by powering down the NRF24l01 radio module when not in use. This lowered the active consumption of the station to just 0.0744W (20mA @ 3.7V). This is a total 69% improvement from the initial 0.24W (65mA @ 3.7V) observed prior to optimising.

With the stations active power consumption optimised, the 4W solar panel array was able to sufficiently charge and power the station eliminating the intermittent communication loss.

## Future Improvements
- This station could be improved in the future with the inclusion of a rainfall sensor to collect metrics on the volume and rate of rainfall. One possible design for this sensor could involve a tipping bucket mechanism, where a known volume of rain causes a bucket to tip, triggering a hall-effect sensor to log the event. By counting the frequency of tipping, the system could accurately measure rate and cumulative rainfall over time.
- Improvements could also be made to the assembly of the custom sensors to be more accessible for servicing as the current design is finicky to put together, not suitable for consistently taking apart and putting back together.
- The internal wiring of the station could also be significantly improved by switching the single core wire used for **each** connection, to a multi-core cable. This could make the internal wiring much tidier and easier to access/service.
