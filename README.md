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

## Power Issues and Resolution

## Future Improvements
