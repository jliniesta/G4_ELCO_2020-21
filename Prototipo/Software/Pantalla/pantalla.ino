/*
    LilyGo Ink Screen Series Test
        - Created by Lewis he
*/

// According to the board, cancel the corresponding macro definition
#define LILYGO_T5_V213
//#define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28


#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>
#include <esp_now.h>

// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w
// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.54" b/w  form DKE GROUP

//#include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7" b/w
// #include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w  form DKE GROUP
// #include <GxDEPG0290B/GxDEPG0290B.h>      // 2.9" b/w   form DKE GROUP

// old panel
//#include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w old panel
#include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w old panel

//! The current LilyGo uses the ink screen version DEPG0213BN
//#include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP

// #include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13" b/w  form GoodDisplay 4-color

// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w   form DKE GROUP

// #include <GxQYEG0290BN/GxQYEG0290BN.h>    // 2.9" b/w

// The following screens are not supported
// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13" b/w/r
// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54" b/w/r 200x200
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r
// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r
// #include <GxDEPG0290R/GxDEPG0290R.h>      // 2.9" b/w/r  form DKE GROUP

// #include <GxDEPG0750BN/GxDEPG0750BN.h>    // 7.5" b/w    form DKE GROUP


#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);



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

typedef struct struct_message {
  int puntEnvA;
  int puntEnvB;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.println(myData.puntEnvA);
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


void addPuntA() {


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


  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(40, 20);
  display.println("SCORE ELCO G4");
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(0, 60);
  display.println("A:");
  display.setCursor(50, 60);
  display.println(set1A);
  display.setCursor(80, 60);
  display.println(set2A);
  display.setCursor(110, 60);
  display.println(set3A);
  display.setCursor(0, 100);
  display.println("B:");
  display.setCursor(50, 100);
  display.println(set1B);
  display.setCursor(80, 100);
  display.println(set2B);
  display.setCursor(110, 100);
  display.println(set3B);
  display.setCursor(170, 60);
  display.println(puntA_pantalla);
  display.setCursor(170, 100);
  display.println(puntB_pantalla);



}


















void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
  display.init(); // enable diagnostic output on Serial
  Serial.println("setup done");
  WiFi.mode(WIFI_MODE_STA);
  if (esp_now_init() != ESP_OK) {
  Serial.println("Error initializing ESP-NOW");
  return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {
  if (myData.puntEnvB == 1){
    addPuntB();
    }
  if (myData.puntEnvA == 1){
    addPuntA();
    }
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(1);
  // draw background
  //display.drawExampleBitmap(BitmapExample1, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  printResultado();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);


}
