#include <main.h>
#define sendPackIntervalSec 300
volatile int secondsPast = 0;

/**** Radio ****/
RF24 radio(CE, CS);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**** AHT20 ****/
DFRobot_AHT20 aht20;
uint8_t status;
bool initAHT20 = true;

/**** LPS331AP ****/
LPS lps331ap;
bool initLSP = true;

/**** ANEMOMETER ****/
volatile int revolutionsAnemometerCount;

sensorData_t packet = {0,0,0,0,1,1};

void setup()
{
  anemometerSetup();
  softwareTimerSetup();
  Serial.begin(115200);
  while (!Serial){}
  aht20Setup();
  lps331apSetup();
  radioSetup();
  Serial.println(F("Connected!"));
  sei();
}

void loop()
{
  mesh.update();

  if (secondsPast >= sendPackIntervalSec)
  {
    getWindspeed(&packet, anemometerArmDistMetres, conversionRatio, &revolutionsAnemometerCount);

    if(packet.initAHT20){
      aht20.reset();
      getAHT20Data(&packet, aht20);
    }
    
    if(packet.initLSP){
      getlsp331AP(&packet, lps331ap);
    }

    sendPacket(&packet, 77, sizeof(packet));
    secondsPast = 0;
  }
}

/**** AUXILIARY FUNCTIONS ****/

void lps331apSetup()
{
  if (!lps331ap.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    packet.initLSP = 0;
  }
  else
  {
    lps331ap.enableDefault();
  }
}

void aht20Setup()
{
  if ((status = aht20.begin()) != 0)
  {
    Serial.print("AHT20 Sensor init failed. error status: ");
    Serial.println(status);
    packet.initAHT20 = 0;
  }
}

void softwareTimerSetup()
{
  /**** Configure the TIMER1****/
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12) | (1 << CS12);
  OCR1A = (F_CPU / 256) - 1;
  TIMSK1 |= (1 << OCIE1A);
}

void anemometerSetup()
{
  /**** Configure the ANEMOMETER****/
  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);
  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0);
}

void radioSetup()
{
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
}

void sendPacket(sensorData_t *packet, uint8_t msg_type, size_t packetSize)
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

void getWindspeed(sensorData_t *packet, float anemometerArmDistMetres, float conversionRatio, volatile int* revolutionsAnemometerCount){
  float speed = anemometerArmDistMetres * (conversionRatio * (((float)*revolutionsAnemometerCount / (float)sendPackIntervalSec) * (float)60));
  *revolutionsAnemometerCount = 0;
  packet->windSpeed = speed;
}

void getAHT20Data(sensorData_t *packet, DFRobot_AHT20 aht20){
  if (aht20.startMeasurementReady(true))
  {
    packet->tempC= aht20.getTemperature_C();
    packet->RH = aht20.getHumidity_RH();
  }
}

void getlsp331AP(sensorData_t *packet, LPS lps331ap){
  float mBar = lps331ap.readPressureMillibars();
  packet->bar = mBar / 1000;
}

ISR(INT0_vect)
{
  revolutionsAnemometerCount += 1;
}

ISR(TIMER1_COMPA_vect){
  secondsPast += 1;
}