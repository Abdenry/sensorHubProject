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

int overflow_count = 0;
const float anemometerArmDistMetres = 0.1;
const float conversionRatio = (2 * PI) / 60;

struct sensorData_t
{
  float tempC = 0;
  float RH = 0;
  float bar;
  float windSpeed = 0;
} packet;

void setup()
{
  
  /**** Configure the ANEMOMETER****/
  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);

  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0);
  
  Serial.begin(115200);
  while (!Serial)
  {
  }

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
  }
  if(initLSP){
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
  
  
  if(initAHT20){
    aht20.reset();
  }

  // Send to the master node every x seconds
  if (millis() - displayTimer >= (sendPackIntervalSec * 1000))
  {
    displayTimer = millis();

    packet.windSpeed = anemometerArmDistMetres * (conversionRatio * (((float)revolutionsAnemometerCount / (float)sendPackIntervalSec) * (float)60));
    revolutionsAnemometerCount = 0;
    
    if(aht20.startMeasurementReady(true)){
      packet.tempC = aht20.getTemperature_C();
      packet.RH = aht20.getHumidity_RH();
    }else{
      packet.tempC = 0;
      packet.RH = 0;
    }
    
    if(initLSP){
      float mBar = lps331ap.readPressureMillibars();
      packet.bar = mBar / 1000;
    }else{
      packet.bar = 0;
    }
    
    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&packet, 'M', sizeof(packet)))
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
      Serial.print("\nSend OK: ");
      Serial.print("tempC: ");
      Serial.print(packet.tempC);
      Serial.print(" RH: ");
      Serial.print(packet.RH);
      Serial.print(" bar: ");
      Serial.print(packet.bar);
      Serial.print(" Wind Speed: ");
      Serial.print(packet.windSpeed);
      Serial.println();
    }
  }
}

ISR(INT0_vect)
{
  revolutionsAnemometerCount += 1;
}