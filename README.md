# Sensor Hub

<p align="center">
  <img width="1280" height="720" alt="weatherStation" src="https://github.com/user-attachments/assets/38e16e49-e8b6-4397-b52e-08d43c67ff3d" />
</p>

A small, portable, self-sustaining weather station built to provide data to an external web interface hosted with Grafana via a Raspberry Pi Zero 2 W. This station is able to collect various metrics from on board sensors such as temperature, humidity, heat index, wind speed, and wind direction. A **DHT22** sensor is used to collect relative humidity and temperature metrics with an accuracy of +-2% and +-0.5C, while metrics such as wind speed and wind direction are collected with custom built sensors. The heat index is calculated in software using the [DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library).

Communication is handled through the NRF24l01 radio modules in a mesh configuration. The mesh routing of data packets is automatically managed with `RF24.h`, `RF24Network.h`, and `RF24Mesh.h` provided by the [RF24](https://github.com/nRF24/RF24?tab=readme-ov-file) library, specifically chosen for its meshing capabilities. It provides dynamic configuration of the NRF24l01 modules, meaning that in the scenario where the weather station loses connection with the Raspberry Pi for any reason (power down, range lost, etc.), it can automatically reconfigure and connect to the Raspberry Pi given its under normal operable condition (i.e. has power and is in range). This allows additional stations to be installed with potentially different purposes with all metrics collected to be routed back to the Raspberry Pi. Alternatively, if the distance between the Raspberry Pi and the weather station is much greater than the range of a single NRF24l01 module, an additional node can be configured and placed in between the weather station and Raspberry Pi acting as a range extender. The RF24 library will automatically handle the routing and pass the data collected from the weather station to the Raspberry Pi via that additional node. There is no limitation that prevents an additional station from also being a range extending node.

<p align="center">
  <img width="1131" height="251" alt="nrf24WeatherStationDiagram3" src="https://github.com/user-attachments/assets/9885c101-9f75-4c3d-95b2-2cd2bea99acb" />
</p>

### Wiring Diagram
<p align="center">
  <img width="1573" height="1013" alt="circuitDiagram3" src="https://github.com/user-attachments/assets/024698c7-ac21-417f-8059-7c26dc810be3" />
</p>

### CAD Model
<p align="center">
  <img width="1224" height="628" alt="waetherStationCAD" src="https://github.com/user-attachments/assets/3ae891b6-66a7-42de-a5d3-a177c26ce002" />
</p>

## Sensor Design

## Grafana Dashboard

## Power Issues and Resolution

## Future Improvements
