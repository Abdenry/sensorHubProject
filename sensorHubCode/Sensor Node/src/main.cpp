#include <main.h>

/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(CE, CS);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
uint32_t displayTimer = 0;

/**** Configure the AHT20****/
DFRobot_AHT20 aht20;
uint8_t status;
uint32_t tempC = 0;
uint32_t RH = 0;

/**** Configure the LPS331AP****/
LPS lps331ap;
uint32_t mBar;
uint32_t altitude;


struct payload_t
{
  unsigned long ms;
  unsigned long counter;
};

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
}

void loop()
{

  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000)
  {
    displayTimer = millis();
    
    if(aht20.startMeasurementReady(true)){
      tempC = (uint32_t)aht20.getTemperature_C();
      RH = (uint32_t)aht20.getHumidity_RH();
    }

    mBar = (uint32_t)lps331ap.readPressureMillibars();
    altitude = (uint32_t)lps331ap.pressureToAltitudeMeters(mBar);

    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&mBar, 'M', sizeof(tempC)))
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
      Serial.print("Send OK: ");
      Serial.println(mBar);
    }
  }

  while (network.available())
  {
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.println(payload.ms);
  }
}