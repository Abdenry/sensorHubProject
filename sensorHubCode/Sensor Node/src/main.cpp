#include <main.h>
#define sendPackIntervalSec 5

bool initAHT20 = true;
bool initLSP = true;

/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(CE, CS);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
uint32_t displayTimer = 0;

/**** Configure the AHT20****/
DFRobot_AHT20 aht20;
uint8_t status;

/**** Configure the LPS331AP****/

LPS lps331ap;

/**** Configure the ANEMOMETER****/

volatile int revolutionsAnemometerCount;

sensorData_t packet = {0,0,0,0};

void setup()
{
  /**** Configure the ANEMOMETER****/
  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);

  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0);

  Serial.begin(115200);
  while (!Serial){}

  if ((status = aht20.begin()) != 0)
  {
    Serial.print("AHT20 Sensor init failed. error status: ");
    Serial.println(status);
    initAHT20 = false;
  }

  if (!lps331ap.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    initLSP = false;
  }else{
    lps331ap.enableDefault();
  }

  mesh.setNodeID(nodeID);
  radio.begin();
  radio.setPALevel(RF24_PA_MIN, 0);

  Serial.println(F("Connecting to the mesh..."));
  if (!mesh.begin())
  {
    if (radio.isChipConnected())
    {
      do
      {
        // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
        Serial.println(F("Could not connect to network.\nConnecting to the mesh..."));
      } while (mesh.renewAddress() == MESH_DEFAULT_ADDRESS);
    }
    else
    {
      Serial.println(F("Radio hardware not responding."));
      while (1)
      {
      }
    }
  }
  
  Serial.println(F("Connected!"));
  sei();
}

void loop()
{
  mesh.update();

  if (initAHT20)
  {
    aht20.reset();
  }

  // Send to the master node every x seconds
  if (millis() - displayTimer >= (sendPackIntervalSec * 1000))
  {
    displayTimer = millis();

    getWindspeed(anemometerArmDistMetres, conversionRatio, &revolutionsAnemometerCount);
    getAHT20Data(&packet, aht20);
    getlsp331AP(&packet, lps331ap);
    sendPacket(&packet, 77, sizeof(packet));
  }
}

void sendPacket(const void *packet, uint8_t msg_type, size_t packetSize)
{
  if (!mesh.write(packet, msg_type, packetSize))
  {
    // If a write fails, check connectivity to the mesh network
    if (!mesh.checkConnection())
    {
      // refresh the network address
      Serial.println("Renewing Address");
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS)
      {
        // If address renewal fails, reconfigure the radio and restart the mesh
        // This allows recovery from most if not all radio errors
        mesh.begin();
      }
    }
    else
    {
      Serial.println("Send fail, Test OK");
    }
  }
  else
  {
    Serial.print("Packet sent!\n");
  }
}

void getWindspeed(float anemometerArmDistMetres, float conversionRatio, volatile int* revolutionsAnemometerCount){
  float speed = anemometerArmDistMetres * (conversionRatio * (((float)*revolutionsAnemometerCount / (float)sendPackIntervalSec) * (float)60));
  *revolutionsAnemometerCount = 0;
  packet.windSpeed = speed;
}

void getAHT20Data(sensorData_t *packet, DFRobot_AHT20 aht20){
  if (initAHT20){
      if (aht20.startMeasurementReady(true))
      {
        packet->tempC= aht20.getTemperature_C();
        packet->RH = aht20.getHumidity_RH();
      }
    }else{
      packet->tempC = 0;
      packet->RH = 0;
    }
  }

  void getlsp331AP(sensorData_t *packet, LPS lps331ap){
    if (initLSP)
    {
      float mBar = lps331ap.readPressureMillibars();
      packet->bar = mBar / 1000;
    }else{
      packet->bar = 0;
    }
  }

ISR(INT0_vect)
{
  revolutionsAnemometerCount += 1;
}