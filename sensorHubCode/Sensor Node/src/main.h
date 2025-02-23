#include <avr/io.h>
#include <avr/interrupt.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <SPI.h>
#include <DFRobot_AHT20.h>
#include <LPS.h>
#include "Arduino.h"

#define F_CPU 16000000UL
#define CE 9
#define CS 10
#define nodeID 4

const float anemometerArmDistMetres = 0.1;
const float conversionRatio = (2 * PI) / 60;

typedef struct sensorData_t
{
  float tempC;
  float RH;
  float bar;
  float windSpeed;
  char initAHT20;
  char initLSP;
}sensorData_t;

void lps331apSetup();

void aht20Setup();

void softwareTimerSetup();

void anemometerSetup();

void radioSetup();

void sendPacket(sensorData_t *packet, uint8_t msg_type, size_t packetSize);

void getWindspeed(sensorData_t *packet, float anemometerArmDistMetres, float conversionRatio, volatile int* revolutionsAnemometerCount);

void getWindDirection(sensorData_t *packet);

void getAHT20Data(sensorData_t *packet, DFRobot_AHT20 aht20);

void getlsp331AP(sensorData_t *packet, LPS lps331ap);

