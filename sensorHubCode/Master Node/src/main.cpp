#include "RF24/RF24.h"
#include "RF24Network/RF24Network.h"
#include "RF24Mesh/RF24Mesh.h"

RF24 radio(22,0);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

struct sensorData_t{
	float tempC = 0;
	float RH = 0;
	float mBar = 0;
	float altitude = 0;
};

int main(int argc, char** argv){
	
	mesh.setNodeID(0);
	radio.begin();
	radio.setPALevel(RF24_PA_MIN, 0);

	printf("Start\n");
	if(!mesh.begin()){
		printf("Radio hardware not responding.\n");
		return 0;
	}
	radio.printDetails();
	
	while(1){
		mesh.update();
		mesh.DHCP();

		while(network.available()){
			RF24NetworkHeader header;
			network.peek(header);
			sensorData_t packet;
			switch(header.type){
				case 'M':
					network.read(header, &packet, sizeof(packet));
					printf("RCV temp_c: %f RH: %f mBar: %f altitude_metres: %f from 0%o\n", packet.tempC,packet.RH, packet.mBar, packet.altitude, header.from_node);
					break;
				default:
					network.read(header, 0, 0);
					printf("Rcv bad type %d from 0%o\n", header.type, header.from_node);
					break;

			}
		}
		delay(2);
	}
	return 0;
}
