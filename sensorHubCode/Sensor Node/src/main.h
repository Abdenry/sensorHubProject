#include <avr/io.h>
#include <avr/interrupt.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <SPI.h>
#include <DFRobot_AHT20.h>
#include <LPS.h>
#include <DHT.h>
#include "Arduino.h"


#define F_CPU 16000000UL
#define CE 9
#define CS 10
#define nodeID 4
#define DHTPIN 8
#define DHTTYPE DHT22
#define Qh 4
#define SHLD 5
#define CLK 6
#define CLKINH 7

const float anemometerArmDistMetres = 0.1;
const float conversionRatio = (2 * PI) / 60;

typedef struct sensorData_t
{
  float tempC;
  float RH;
  float heatIndex;
  float windSpeed;
  char initAHT22;
  uint8_t windDirection;
}sensorData_t;

void softwareTimerSetup();

void anemometerSetup();

void windVaneSetup();

void radioSetup();

void sendPacket(sensorData_t *packet, uint8_t msg_type, size_t packetSize);

void getWindspeed(sensorData_t *packet, float anemometerArmDistMetres, float conversionRatio, volatile int* revolutionsAnemometerCount);

void getWindDirection(sensorData_t *packet);

void getAHT22(sensorData_t *packet, DHT dhtSensor);

