#include <main.h>

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

struct sensorData_t
{
  float tempC = 0;
  float RH = 0;
  float bar;
}packet;


void setup()
{
  Serial.begin(115200);
  while (!Serial){}
  Wire.begin();

  while((status = aht20.begin()) != 0){
    Serial.print("AHT20 Sensor init failed. error status: ");
    Serial.println(status);
    delay(1000);    
  }

  if(!lps331ap.init()){
    Serial.println("Failed to autodetect pressure sensor!");
    while(1);
  }
  lps331ap.enableDefault();

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
      while (1){}
    }
  }
  Serial.println(F("Connected!"));
}

void loop()
{

  mesh.update();

  aht20.reset();

  // Send to the master node every 5 seconds
  if (millis() - displayTimer >= 5000)
  {
    displayTimer = millis();
    
    if(aht20.startMeasurementReady(true)){
      packet.tempC = aht20.getTemperature_C();
      packet.RH = aht20.getHumidity_RH();
    }

    float mBar = lps331ap.readPressureMillibars();
    packet.bar = mBar / 1000;

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
    }
  }
}