/*
 * RF24Ethernet InteractiveServer example by TMRh20
 * - Webserver controlling an LED example
 *
 * RF24Ethernet uses the uIP stack by Adam Dunkels <adam@sics.se>
 *
 * This example demonstrates how to configure a sensor node to act as a webserver and
 * allows a user to control a connected LED by clicking links on the webpage
 * The requested URL is used as input, to determine whether to turn the LED off or on
 *
 * In order to minimize memory use and program space:
 * 1. Open the RF24Network library folder
 * 2. Edit the RF24Networl_config.h file
 * 3. Un-comment #define DISABLE_USER_PAYLOADS
 *
 *
 * This example uses RF24Mesh. 
 * Set #define UIP_CONF_LLH_LEN 0 in uip_conf.h if used with a TUN(RF24Mesh) or SLIP interface 
 * 
 */

#include <RF24.h>
#include <RF24Network.h>
//#include <printf.h>
#include <RF24Ethernet.h>
#include "HTML.h"
#include <RF24Mesh.h>

/*** Configure the radio CE & CS pins ***/


#if defined (ARDUINO_ARCH_ESP8266)
  #define LED_PIN BUILTIN_LED
#else
  #define LED_PIN A3 //Analog pin A3
#endif


/**********************************************************/
static unsigned short generate_tcp_stats();

void setup() {
  RF24_init(7,8);
  RF24N_init();
  RF24M_init();
  RF24E_init();
  RF24ES_init(1000); 

  Serial.begin(115200);
  //printf_begin();
  Serial.println("start");
  pinMode(LED_PIN, OUTPUT);
  
  IPAddress_ myIP= {10,10,2,4};

  RF24E_begin_i(myIP);
  RF24M_begin( MESH_DEFAULT_CHANNEL, RF24_1MBPS, MESH_RENEWAL_TIMEOUT);

  //Set IP of the RPi (gateway)
  IPAddress_ gwIP={10,10,2,2};

  RF24E_set_gateway(gwIP);

  RF24ES_begin();

}


/********************************************************/

uint32_t mesh_timer = 0;

void loop() {

  // Optional: If the node needs to move around physically, or using failover nodes etc.,
  // enable address renewal
  if(millis()-mesh_timer > 30000){ //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if( ! RF24M_checkConnection() ){
        //refresh the network address        
        RF24M_renewAddress(MESH_RENEWAL_TIMEOUT);
     }
  }

  size_t size;

  RF24ES_available();
  if (RF24EC_valid()) 
  {
    uint8_t pageReq = 0;
    generate_tcp_stats();
    while ((size = RF24EC_available()) > 0)
    {
      // If a request is received with enough characters, search for the / character
      if (size >= 7) {
        RF24EC_findUntil("/", "/");
        char buf[3] = {"  "};
        if(RF24EC_available() >= 2){
        buf[0] = RF24EC_read();  // Read in the first two characters from the request
        buf[1] = RF24EC_read();

        if (strcmp(buf, "ON") == 0) { // If the user requested http://ip-of-node:1000/ON
          led_state = 1;
          pageReq = 1;
          digitalWrite(LED_PIN, led_state);
          
        }else if (strcmp(buf, "OF") == 0) { // If the user requested http://ip-of-node:1000/OF
          led_state = 0;
          pageReq = 1;
          digitalWrite(LED_PIN, led_state);
          
        }else if (strcmp(buf, "ST") == 0) { // If the user requested http://ip-of-node:1000/ST
          pageReq = 2;
          
        }else if (strcmp(buf, "CR") == 0) { // If the user requested http://ip-of-node:1000/CR
          pageReq = 3;
          
        }else if(buf[0] == ' '){
          pageReq = 4; 
        }
      }
      }
      // Empty the rest of the data from the client
      //while (client.waitAvailable()) {
        RF24EC_flush();
      //}
    }
    
    /**
    * Based on the incoming URL request, send the correct page to the client
    * see HTML.h
    */
    switch(pageReq){
       case 2: stats_page(); break;
       case 3: credits_page(); break;
       case 4: main_page(); break;
       case 1: main_page(); break;
       default: break; 
    }    

    RF24EC_stop();
    Serial.println(F("********"));

  }

  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

/**
* This section displays some basic connection stats via Serial and demonstrates
* how to interact directly with the uIP TCP/IP stack
* See the uIP documentation for more info
*/
static unsigned short generate_tcp_stats()
{
  struct uip_conn *conn;

  // If multiple connections are enabled, get info for each active connection
  for (uint8_t i = 0; i < UIP_CONF_MAX_CONNECTIONS; i++) {
    conn = &uip_conns[i];

    // If the application state is active for an available connection, print the info
    if (conn->appstate) {
      Serial.print(F("Connection no "));
      Serial.println(i);
      Serial.print(F("Local Port "));
      Serial.println(htons(conn->lport));
      Serial.print(F("Remote IP/Port "));
      Serial.print(htons(conn->ripaddr[0]) >> 8);
      Serial.print(F("."));
      Serial.print(htons(conn->ripaddr[0]) & 0xff);
      Serial.print(F("."));
      Serial.print(htons(conn->ripaddr[1]) >> 8);
      Serial.print(F("."));
      Serial.print(htons(conn->ripaddr[1]) & 0xff);
      Serial.print(F(":"));
      Serial.println(htons(conn->rport));
      Serial.print(F("Outstanding "));
      Serial.println((uip_outstanding(conn)) ? '*' : ' ');

    }
  }
}

