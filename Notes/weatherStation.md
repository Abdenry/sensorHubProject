# Weather Station

The goal is to create an affordable meteorological station for homeowners. This station will enable users to conduct informal studies on local weather patterns, identify optimal gardening techniques, and predict weather events to better prepare for potential threats to their homes.

The station will have a myriad of sensors used to identify the correct weather trends and are listed in detail below. The goal is to construct a weather station for less then cheap commercially [available options](https://www.bunnings.com.au/holman-aspect-wireless-data-centre-weather-station_p3110644).

## Functionality

| Atmospheric Conditions                                       | Sensor          |
| ------------------------------------------------------------ | --------------- |
| The weather station shall measure Dry Ambient Temperature.   | (ASAIR - AHT20) |
| The weather station shall measure Percent Relative Humidity. | (ASAIR - AHT20) |
| The weather station shall measure Atmospheric Pressure.      | (ST - LPS331AP) |

| Wind Measurements                                 | Sensor                     |
| ------------------------------------------------- | -------------------------- |
| The weather station shall measure Wind Speed.     | (Diodes Corporate - AH180) |
| The weather station shall measure Wind Direction. | (Diodes Corporate - AH180) |

| [Precipitation Monitoring](https://smartsolutions4home.com/ss4h-rg-rain-gauge/) | Sensor                     |
| ------------------------------------------------------------------------------- | -------------------------- |
| The weather station shall measure Rate of Rainfall.                             | (Diodes Corporate - AH180) |
| The weather station shall measure Amount of Rainfall.                           | (Diodes Corporate - AH180) |

| Severe Weather Detection                             | Sensor |
| ---------------------------------------------------- | ------ |
| The weather station should detect lightning strikes. | TBD    |

## Design

| Radiation Shield                                                                                                                                                                                                                                                                                                                                                                                                   |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| Both the humidity and temperature sensors will be contained within a radiation shield. This is to prevent direct and indirect heat sources compromising the reliability/accuracy of the senors. It also acts to shield the sensor against rain and fog. [Learn More](https://www.instrumentchoice.com.au/news/instrument-choice-experiment-what-is-the-function-of-a-solar-radiation-shield-on-a-weather-station). |

| Microcontroller                                                                                                                                                                            |
| ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| The microcontroller will be contained in a waterproof container. Any connections made between the microcontroller that is outside the container, will be waterproof and weather resistent. |

| Materials                                                                                                                                                                   |
| --------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Materials used in the design will be durable to the Australia weather. Materials should be UV resistant and should have a heat deflection rating higher than $55\degree$ C. |

| Power |
| ----- |
| A battery will be used to power the device and sensors. The battery will be contained within a water proof container with any connections between it and devices outside the container, waterproof and weather resistent. The battery will be rechargeable via a solar panel mounted to the weather station. |

## Notes

- The chosen [magnets](https://www.digikey.com.au/en/products/detail/radial-magnets-inc/8015/15276818) to be used have been calculated based on the surface Gauss rating and the AH180 operation specs outlined in the datasheet. The information while not rigorous, has been marked against this [calculator](https://www.kjmagnetics.com/calculator.asp?calcType=disc),
  and hand calculated based on the inverse square law. Noting that for both the calculator and [datasheet](https://docs.rs-online.com/c755/0900766b813e101b.pdf) for a similar magnet with a defined Gauss rating at ~1mm. Both seemed to lose ~40% of strength @ 1mm from surface rating.
  Using this as a basis for the chosen magnets it was estimated that at 1mm from surface would have a Gauss rating of ~1335.5, following the inverse square law with this in mind, it was estimated that at 5 mm from the surface of the chosen magnet it would have a strength of ~53.42 Gauss.
  This should be sufficient for the AH180 operation.

- There are two method to measure apparent temperature; one with solar radiance and one without. Both require the dry ambient temperature, the relative humidity percent, and wind speed (m/s). The wind speed needs to be measured at an elevation of 10 meters.
