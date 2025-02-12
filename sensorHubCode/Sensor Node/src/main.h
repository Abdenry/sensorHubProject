#include <avr/io.h>
#include <avr/interrupt.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <SPI.h>
#include <DFRobot_AHT20.h>
#include <LPS.h>

#define CE 9
#define CS 10
#define nodeID 4

const float anemometerArmDistMetres = 0.1;
const float conversionRatio = (2 * PI) / 60;


// typedef struct sensorData_t {
//     bool initAHT20;
//     float aht20tempC;
//     float aht20RH;
//     bool initLSP;
//     float lspBar;
//     float windSpeed;
// } sensorData_t;


void sendPacket(const void* packet, uint8_t msg_type, size_t packetSize);

void getWindspeed(float anemometerArmDistMetres, float conversionRatio, volatile int* revolutionsAnemometerCount);

// void getAHT20Data(sensorData_t *packet, DFRobot_AHT20 aht20);