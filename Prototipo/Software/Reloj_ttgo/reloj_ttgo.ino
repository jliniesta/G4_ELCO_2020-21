/*
  Display all the fast rendering fonts.

  This sketch uses the GLCD (font 1) and fonts 2, 4, 6, 7, 8

  Make sure all the display driver and pin comnenctions are correct by
  editting the User_Setup.h file in the TFT_eSPI library folder.

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
*/

#include "config.h"
//#ifdef LILYGO_WATCH_HAS_BUTTON
//
//    /*
//        ttgo->button->setClickHandler([]() {
//            Serial.println("Button2 Pressed");
//        });
//    */
//
//    //Set the user button long press to restart
//    ttgo->button->setLongClickHandler([]() {
//        Serial.println("Pressed Restart Button,Restart now ...");
//        delay(1000);
//        esp_restart();
//    });
//#endif



TTGOClass *ttgo;

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
uint8_t broadcastAddress[] = {0xA4, 0xCF, 0x12, 0x02, 0xBF, 0x9C};

typedef struct struct_message {
  int puntEnvA;
  int puntEnvB;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.println(myData.puntEnvA);

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
void addPuntA() {
  ttgo->tft->fillScreen(TFT_WHITE);
  
    puntA ++;
    myData.puntEnvA = 0;
  
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
//
//
//// sumar punto B
void addPuntB(lv_obj_t *obj, lv_event_t event) {
  ttgo->tft->fillScreen(TFT_WHITE);
  if (event == LV_EVENT_CLICKED) {
    puntB ++;
    myData.puntEnvB = 1;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  }
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

  ttgo->tft->drawString(String(set1A), 0, 0);

  ttgo->tft->drawString( String(set2A), 40, 0);

  ttgo->tft->drawString(String(set3A), 80, 0);

  ttgo->tft->drawString(puntA_pantalla, 160, 0);

  ttgo->tft->drawString(String(set1B), 0, 60);

  ttgo->tft->drawString(String(set2B), 40, 60);

  ttgo->tft->drawString( String(set3B), 80, 60);


  ttgo->tft->drawString(puntB_pantalla, 160, 60);

  lv_task_handler();
}



void setup(void)
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  ttgo->lvgl_begin();

  lv_obj_t *label;


//  lv_obj_t *btn1 = lv_btn_create(lv_scr_act(), NULL);
//  lv_obj_set_event_cb(btn1, addPuntA);
//  lv_obj_set_size(btn1, 100, 100);
//  lv_obj_align(btn1, NULL, LV_ALIGN_IN_LEFT_MID, 0, 40);
//
//  label = lv_label_create(btn1, NULL);
//  lv_label_set_text(label, "suma A");

  lv_obj_t *btn2 = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn2, addPuntB);
  lv_obj_set_size(btn2, 100, 100);
  lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 40);

  label = lv_label_create(btn2, NULL);
  lv_label_set_text(label, "suma B");
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

 
}

void loop()
{
  if (myData.puntEnvA == 1){
    addPuntA();
    }
  

  // First we test them with a background colour set
  ttgo->tft->setTextSize(5);

  ttgo->tft->setTextColor(TFT_GREEN);
  printResultado();


  delay(5);
}
