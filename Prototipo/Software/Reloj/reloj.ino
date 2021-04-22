
#include "Arduino.h"
#include "heltec.h"
#include "WiFi.h"
#include "GFButton.h"
#include <esp_now.h>


GFButton btn1 = GFButton(12);
int set1A = 0;
int set2A = 0;
int set3A = 0;
int set1B = 0;
int set2B = 0;
int set3B = 0;
int puntA = 0;
int puntB = 0;
String puntA_pantalla ; // 0 15 30 40
String puntB_pantalla ; // 0 15 30 40
int set = 1; // set 1, 2, 3
boolean tie = false;

uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x0D, 0x93, 0x5C};
uint8_t broadcastAddress2[] = {0x7C, 0x9E, 0xBD, 0xFB, 0x1D, 0xDC};

typedef struct struct_message {
  int puntEnvA;
  int puntEnvB;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
   char macStr[12];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.println(myData.puntEnvB);

}




void addjuegoA() {
  if (set == 1) {
    set1A ++;
  }
  else if (set == 2) {
    set2A ++;
  }
  else {
    set3A ++;
  }
}

void addjuegoB() {
  if (set == 1) {
    set1B ++;
  }
  else if (set == 2) {
    set2B ++;
  }
  else {
    set3B ++;
  }
}


//sumar punto A
void addPuntA(GFButton & button) {


  puntA ++;
  myData.puntEnvA = 1;
  esp_err_t result = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));


  if (tie == false ) {
    if ((puntA > 3) && (puntA - puntB >= 2)) {
      addjuegoA();
      puntA = 0;
      puntB = 0;
    }

  }  else if (tie == true) {
    if ((puntA >= 7 ) && (puntA - puntB >= 2) ) {
      addjuegoA();
      puntA = 0;
      puntB = 0;
    }
  }
  if (set == 1) {
    if ((set1A >= 6 && ((set1A - set1B) >= 2)) || set1A == 7) {
      set ++;
      tie  = false;
    }

    else if ((set1A == 6) && (set1B == 6)) {
      tie = true;
    }
  }
  if (set == 2) {
    if ((set2A >= 6 && ((set2A - set2B) >= 2)) || set2A == 7) {
      set ++;
      tie  = false;
    }

    else if ((set2A == 6) && (set2B == 6)) {
      tie = true;
    }
  }
  if (set == 3) {
    if ((set3A >= 6 && ((set3A - set3B) >= 2)) || set3A == 7) {
      set ++;
      tie  = false;
    }

    else if ((set3A == 6) && (set3B == 6)) {
      tie = true;
    }
  }


}


// sumar punto B
void addPuntB() {
  
  
    puntB ++;
    myData.puntEnvB = 0;
  
  if (tie == false ) {
    if (( puntB > 3) && (puntB - puntA >= 2)) {

      addjuegoB();
      puntA = 0;
      puntB = 0;

    }
  }  else if (tie == true) {
    if ((puntB >= 7 ) && (puntB - puntA >= 2) ) {
      addjuegoB();
      puntA = 0;
      puntB = 0;
    }
  }
  if (set == 1) {
    if ((set1B >= 6 && ((set1B - set1A) >= 2)) || set1B == 7) {
      set ++;
      tie  = false;
    }
    else if ((set1A == 6) && (set1B == 6)) {
      tie = true;
    }
  }
  if (set == 2) {
    if ((set2B >= 6 && ((set2B - set2A) >= 2)) || set2B == 7) {
      set ++;
      tie  = false;
    }
    else if ((set2A == 6) && (set2B == 6)) {
      tie = true;
    }
  }
  if (set == 3) {
    if ((set3B >= 6 && ((set3B - set3A) >= 2)) || set3B == 7) {
      set ++;
      tie  = false;
    }
    else if ((set3A == 6) && (set3B == 6)) {
      tie = true;
    }
  }


}



void printResultado() {
  if (tie == false) {
    if (puntA == 0) {
      puntA_pantalla = "0";
    }
    else if (puntA == 1) {
      puntA_pantalla = "15";
    }
    else if (puntA == 2) {
      puntA_pantalla = "30";
    }
    else if (puntA == 3) {
      puntA_pantalla = "40";
    }
    else if ((puntA > 3) && (puntA == puntB)) {
      puntA_pantalla = "40";
    }
    else if ((puntA > 3) && (puntA > puntB)) {
      puntA_pantalla = "AD";
    }

    if (puntB == 0) {
      puntB_pantalla = "0";
    }
    else if (puntB == 1) {
      puntB_pantalla = "15";
    }
    else if (puntB == 2) {
      puntB_pantalla = "30";
    }
    else if (puntB == 3) {
      puntB_pantalla = "40";
    }
    else if ((puntB > 3) && (puntA == puntB)) {
      puntB_pantalla = "40";
    }
    else if ((puntB > 3) && (puntA < puntB)) {
      puntB_pantalla = "AD";
    }
  } else {
    puntA_pantalla = (String) puntA;
    puntB_pantalla = (String) puntB;

  }




  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(0, 0, "EQUIPO A");

  Heltec.display->drawString(0, 20, "A:");

  Heltec.display->drawString(0, 40, "B:");

  Heltec.display->drawString(20, 20, String(set1A));

  Heltec.display->drawString(40, 20, String(set2A));

  Heltec.display->drawString(60, 20, String(set3A));

  Heltec.display->drawString(20, 40, String(set1B));

  Heltec.display->drawString(40, 40, String(set2B));

  Heltec.display->drawString(60, 40, String(set3B));

  Heltec.display->drawString(100, 20, puntA_pantalla);

  Heltec.display->drawString(100, 40, puntB_pantalla);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  btn1.setClicksHandler(addPuntA);
  btn1.setDebounceTime(100);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  esp_now_peer_info_t peerInfo;
 
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);       
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);

}

void loop() {
  btn1.process();
  if (myData.puntEnvB == 1){
    addPuntB();
    }
  

  // put your main code here, to run repeatedly:
  Heltec.display->clear();
  printResultado();
  Heltec.display->display();
  delay(5);

}
