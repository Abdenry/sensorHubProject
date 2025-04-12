#include <main.h>
#define watchdogCounterTarget 1 //watchdogCounterTarget * 8 seconds = WDT Interrupt Period
volatile int watchdogCounter = 0;

/**** Radio ****/
RF24 radio(CE, CS);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**** ANEMOMETER ****/
volatile int revolutionsAnemometerCount;

/**** DHT22 ****/
DHT dht(DHTPIN, DHTTYPE);

sensorData_t packet = {0,0,0,0,0,1};

void setup()
{
  anemometerSetup();
  windVaneSetup();
  Serial.begin(115200);
  while (!Serial){}
  radioSetup();
  dht.begin();
  Serial.println(F("Connected!"));
  watchdogSetup();
  sei();
}

void loop()
{
  if (watchdogCounter >= watchdogCounterTarget){
    radio.powerUp();
    mesh.update();
    getWindspeed(&packet, anemometerArmDistMetres, conversionRatio, &revolutionsAnemometerCount);
    getAHT22(&packet, dht);
    getWindDirection(&packet);
    sendPacket(&packet, 77, sizeof(packet));
    watchdogCounter =0;
    radio.powerDown();
  }
  sleepStation();
}

/**** AUXILIARY FUNCTIONS ****/

void watchdogSetup(){
  // 8s WDT, Interrupt Mode
  MCUSR &= ~(1<<WDRF);
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = (1<<WDIE) | (1<<WDP3) | (1<<WDP0);
}

void sleepStation(){
  // Enable power-down Mode
  SMCR &= ~((1<<SM0) | (1<<SM2));
  SMCR |= (1<<SM1);
  SMCR |= (1<<SE);
  sleep_cpu();
}

void anemometerSetup()
{
  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);
  EICRA |= (1 << ISC01);
  EIMSK |= (1 << INT0);
}

void windVaneSetup(){
  DDRD &= ~(1<<Qh);
  DDRD |= (1<<SHLD) | (1<<CLK) | (1<<CLKINH);
  PORTD &= ~(1<< CLKINH);
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
  }
}

void getWindspeed(sensorData_t *packet, float anemometerArmDistMetres, float conversionRatio, volatile int* revolutionsAnemometerCount){
  float speed = anemometerArmDistMetres * (conversionRatio * (((float)*revolutionsAnemometerCount / (float)(watchdogCounterTarget*8)) * (float)60));
  *revolutionsAnemometerCount = 0;
  packet->windSpeed = speed;
}

void getAHT22(sensorData_t *packet, DHT dhtSensor){
  packet->RH = dhtSensor.readHumidity();
  packet->tempC = dhtSensor.readTemperature();
  packet->heatIndex =dhtSensor.computeHeatIndex(false);
  if (isnan(packet->RH) || isnan(packet->tempC) || isnan(packet->heatIndex)) {
    packet->initAHT22 = false;
  }else{
    packet->initAHT22 = true;
  }
}

void getWindDirection(sensorData_t *packet){
  uint8_t data = 0;
  PORTD &= ~(1<<SHLD);
  delayMicroseconds(1);
  PORTD |= (1<<SHLD);
  
  for(int i = 0; i < 8; i++){
    PORTD |= (1<<CLK);
    delayMicroseconds(1);
    data |= ((PIND & (1 << Qh)) ? 0 : 1) << (7 - i);
    PORTD &= ~(1<<CLK);
  }
  packet->windDirection = (float)data;
}

ISR(INT0_vect)
{
  revolutionsAnemometerCount += 1;
}

ISR(WDT_vect){
  watchdogCounter += 1;
}