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

void sendPacket(const void* packet, uint8_t msg_type, size_t packetSize);