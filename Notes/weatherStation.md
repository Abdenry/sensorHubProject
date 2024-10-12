# Weather Station

The goal is to create an affordable meteorological station for homeowners. This station will enable users to conduct informal studies on local weather patterns, identify optimal gardening techniques, and predict weather events to better prepare for potential threats to their homes.

The station will have a myriad of sensors used to identify the currect weather trends and are listed in detail below. The goal is to construct a weather station for less then cheap commerically [available options](https://www.bunnings.com.au/holman-aspect-wireless-data-centre-weather-station_p3110644)

## Atmospheric Conditions

The weather station shall measure:

- Dry Ambient temperature (ASAIR - AHT20)
- Percent relative Humidity (ASAIR - AHT20)
- Atmospheric pressure (ST - LPS331AP)

## Wind Measurements

The weather station shall measure:

- Wind Speed (DiodesZetex - AH180)
- Wind Direction (DiodesZetex - AH180)

Hall effect sensors can be used to measure the current position and the rotational speed of a shaft.

## Precipitation Monitoring

The weather station shall measure:

- Rain Gauge (DiodesZetex - AH180)

Following this [open source design](https://smartsolutions4home.com/ss4h-rg-rain-gauge/)

## Severe Weather Detection

The weather station should have:

- Lightning Detector

# Notes

- There are two method to measure apparent temperature; one with solar radiance and one without. Both require the dry ambient temperature, the relative humidity percent, and wind speed (m/s). The wind speed needs to be measured at an elevation of 10 meters.
