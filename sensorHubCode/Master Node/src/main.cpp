#include "RF24/RF24.h"
#include "RF24Network/RF24Network.h"
#include "RF24Mesh/RF24Mesh.h"
#include <curl/curl.h>
#include <time.h>
#include <unistd.h>

void sendMetric(CURL *curl, CURLcode res, const char *metric, float value, char *timeSTR);
void updateTime(char *timeSTR, size_t lenSTR);

RF24 radio(22,0);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

typedef struct sensorData_t{
        float tempC;
        float RH;
        float heatIndex;
        float windSpeed;
        char initAHT22;
}sensorData_t;

int main(int argc, char** argv){
        const char *metricNames[] = {"Temperature", "Relative Humidity", "Heat Index", "Wind Speed"};
        int numberOfMetrics = sizeof(metricNames) / sizeof(metricNames[0]);

        // RF24
        mesh.setNodeID(0);
        radio.begin();
        radio.setPALevel(RF24_PA_MIN, 0);
        // Libcurl
        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        char timeSTR[30];

        if(!mesh.begin()){
                printf("Radio hardware not responding.\n");
                return 1;
        }

        printf("Start\n");
        radio.printDetails();

        sensorData_t packet = {0,0,0,0,1};
        float* metrics[] = {&packet.tempC, &packet.RH, &packet.heatIndex, &packet.windSpeed};

        while(1){
                mesh.update();
                mesh.DHCP();

                while(network.available()){
                        RF24NetworkHeader header;
                        network.peek(header);

                        updateTime(timeSTR, sizeof(timeSTR));
                        // printf("UTC time is: %s\n",timeSTR);

                        switch(header.type){
                                case 'M':
                                        network.read(header, &packet, sizeof(packet));
                                        //printf("RCV temp_c: %.2f RH: %.2f heatIndex: %.2f wind speed: %.2f from 0%o at UTC time: %s\n", packet.tempC,packet.RH, packet.heatIndex, packet.windSpeed, header.from_node, timeSTR);
                                        break;
                                default:
                                        network.read(header, 0, 0);
                                        printf("Rcv bad type %d from 0%o\n", header.type, header.from_node);
                                        break;
                        }                

                            for(int i = 0; i < numberOfMetrics; i++){
                                if(((metricNames[i] == "Temperature") || (metricNames[i] == "Relative Humidity") || (metricNames[i] == "Heat Index")) && (packet.initAHT22 == 0)){
                                    continue;
                                }
                                sendMetric(curl, res, metricNames[i], *metrics[i], timeSTR);
                                if(res == CURLE_OK){
                                    printf("\n");
                                    printf("%s: %.2f\n", metricNames[i], *metrics[i]);
                                }
                            }
                            printf("\n");
                    }
                usleep(5000);
        }
        return 0;
}

void sendMetric(CURL* curl, CURLcode res, const char* metricName, float value, char *timeSTR){
        if(curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "https://api.vinnievertongen.com.au/metrics");
                curl_easy_setopt(curl, CURLOPT_USERPWD, "weathersensor:examplePass");

                // const char *json_data = "{\"bucket\": \"weather-station\", \"org\": \"home\", \"measurement\": \"test-metric\", \"tags\": {\"location\": \"test\"}, \"fields\": {\"value\": 5}, \"timestamp\": \"2024-11-08T08:32:01Z\"}";
                char json_data[1024];
                snprintf(json_data, sizeof(json_data),
                 "{\"bucket\": \"weather-station\", \"org\": \"home\", \"measurement\": \"%s\", "
                 "\"tags\": {\"location\": \"weatherStation\"}, \"fields\": {\"value\": %.2f}, "
                 "\"timestamp\": \"%s\"}",
                 metricName, value, timeSTR);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
                struct curl_slist *headers = NULL;
                headers = curl_slist_append(headers, "Content-Type: application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                res = curl_easy_perform(curl);
                if(res != CURLE_OK) {
                        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                }
        }
}

void updateTime(char *timeSTR, size_t lenSTR){
        time_t rawTime;
        struct tm *info;

        time(&rawTime);
        info = gmtime(&rawTime);

        // 2024-11-08T08:32:01Z
        strftime(timeSTR, lenSTR, "%Y-%m-%dT%H:%M:%SZ", info);
}
