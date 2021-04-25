/**
*	Pantalla marcador de padel, versión del LILYGO TTGO T5 V2.3 ESP32
*	
*	Autores:
*		Fernando Garcia Gutierrez
*		Javier Lopez Iniesta
*		Ivan Martin Canton
*		Iñigo Montesino Sarmiento
*		Luis de Pablo Beltran
**/

// Se define la version de nuestra pantalla
#define LILYGO_T5_V213

#include <boards.h>
#include <GxEPD.h>
#include <SD.h>
#include <FS.h>
#include <esp_now.h>

// Se incluyen las libreria de nuestro modelo
#include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w old panel
#include GxEPD_BitmapExamples

// FreeFonts de Adafruit_GFX
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>

GxIO_Class io(SPI,  EPD_CS, EPD_DC,  EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

// Se definen las variables para almacenar la puntuación de los equipos.
int set1A = 0;
int set2A = 0;
int set3A = 0;
int set1B = 0;
int set2B = 0;
int set3B = 0;
int puntA = 0;
int puntB = 0;

// Se crean las variables para representar la puntuación en pantalla del marcador.
String puntA_pantalla ; // 0 15 30 40
String puntB_pantalla ; // 0 15 30 40

// Se crean las variables que controlan el funcionamiento del marcador.
int set = 1; // set 1, 2, 3
boolean tie = false;
boolean winA = false;
boolean winB = false;

/* Se crea una estructura que almacena cuando se pulsan los mensajes para poder realizar 
la comunicacion con el resto de dispositivos
*/
typedef struct struct_message {
  int puntEnvA;
  int puntEnvB;
} struct_message;

struct_message myData;

// Funcion que almacena los valores recibidos en una constante de tipo struct_message
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println(myData.puntEnvA);
  Serial.println(myData.puntEnvB);
}

/* Funcion que cuando es llamada, aumenta un juego al equipo A en funcion del set en el que 
se encuentren.
*/
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

/* Funcion que cuando es llamada, aumenta un juego al equipo B en funcion del set en el que se 
encuentren.
*/
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

// Funcion que cuando es llamada, suma un punto al equipo A.
void addPuntA() {

  // Se suma un punto al equipo A.
  puntA ++;
  
  /* Se reinicia el valor de la variable que indica que el equipo A ha ganado un punto para 
  poder detectar el siguiente cambio
  */
  myData.puntEnvA = 0;

  /* Solo añade un juego si el equipo A tiene mas de 3 puntos (o de 7 si estan en tie break) 
  y una diferencia de 2 puntos frente al equipo B.
  */
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
  
  /* Si se encuentra en el primer set y el equipo A tiene mas de 6 juegos y hay una diferencia 
  de 2 o más juegos entre los dos (o si tiene 7 juegos para el tie break), suma un set y tie 
  pasa a valer falso. Si los dos llegan a 6 juegos, tie pasa a valer true.
  */
  if (set == 1) {
    if ((set1A >= 6 && ((set1A - set1B) >= 2)) || set1A == 7) {
      set ++;
      tie  = false;
    }
    else if ((set1A == 6) && (set1B == 6)) {
      tie = true;
    }
  }
  
  /* Exactamente igual que en el set anterior, pero si detecta además que el equipo A ganó 
  el set anterior, el equipo A gana.
  */
  if (set == 2) {
    if ((set2A >= 6 && ((set2A - set2B) >= 2)) || set2A == 7) {
      if (set1A > set1B) {
        winA = true;
      } else {
        set ++;
        tie  = false;
      }
    }
    else if ((set2A == 6) && (set2B == 6)) {
      tie = true;
    }
  }
  
  /* Exactamente igual que el set anterior, pero comprobando el set 1 y el 2 para poder dar la 
  victoria al equipo A.
  */
  if (set == 3) {
    if ((set3A >= 6 && ((set3A - set3B) >= 2)) || set3A == 7) {
      if (set1A > set1B || set2A > set2B) {
        winA = true;
      } else {
        set ++;
        tie  = false;
      }
    }
    else if ((set3A == 6) && (set3B == 6)) {
      tie = true;
    }
  }
}

// Funcion que cuando es llamada, suma un punto al equipo B.
void addPuntB() {

  // Se suma un punto al equipo B
  puntB ++;
  
  /* Se reinicia el valor de la variable que indica que el equipo B ha ganado un punto para 
  poder detectar el siguiente cambio
  */
  myData.puntEnvB = 0;

  /* Solo añade un juego si el equipo B tiene mas de 3 puntos (o de 7 si estan en tie break) y 
  una diferencia de 2 puntos frente al equipo A.
  */
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
  
  /* Si se encuentra en el primer set y el equipo B tiene mas de 6 juegos y hay una 
  diferencia de 2 o más juegos entre los dos (o si tiene 7 juegos para el tie break), 
  suma un set y tie pasa a valer falso. Si los dos llegan a 6 juegos, tie pasa a valer true.
  */
  if (set == 1) {
    if ((set1B >= 6 && ((set1B - set1A) >= 2)) || set1B == 7) {
      set ++;
      tie  = false;
    }
    else if ((set1A == 6) && (set1B == 6)) {
      tie = true;
    }
  }
  
  /* Exactamente igual que en el set anterior, pero si detecta además que el equipo B ganó el 
  set anterior, el equipo A pierde.
  */
  if (set == 2) {
    if ((set2B >= 6 && ((set2B - set2A) >= 2)) || set2B == 7) {
      if (set1B > set1A) {
        winB = true;
      } else {
        set ++;
        tie  = false;
      }
    }
    else if ((set2A == 6) && (set2B == 6)) {
      tie = true;

    }
  }
  
  /* Exactamente igual que el set anterior, pero comprobando el set 1 y el 2 para poder dar 
  la victoria al equipo B.
  */
  if (set == 3) {
    if ((set3B >= 6 && ((set3B - set3A) >= 2)) || set3B == 7) {
      if (set1B > set1A || set2B > set2A) {
        winB = true;
      } else {
        set ++;
        tie  = false;
      }
    }
    else if ((set3A == 6) && (set3B == 6)) {
      tie = true;
    }
  }
}

// Funcion que pinta en la pantalla la puntuacion de la partida y el resultado final.
void printResultado() {
	
  /* Se convierten las variables a la puntuacion de padel en funcion de si se encuentran 
  en tie break o no.
  */
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

  // Se pinta la puntuacion mientras no se haya acabado la partida.
  if (winA == false && winB == false) {
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
  
  //Se pinta la pantalla de victora o derrota en funcion del resultado final.
  else if (winA == true) {
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(40, 20);
    display.println("SCORE ELCO G4");
    display.setCursor(0, 60);
    display.println( "Ha ganado el");
    display.setCursor(0, 80);
    display.println( "Equipo A");
	
  } 
  else if (winB == true) {
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(40, 20);
    display.println("SCORE ELCO G4");
    display.setCursor(0, 60);
    display.println( "Ha ganado el ");
    display.setCursor(0, 80);
    display.println( "Equipo B");
  }
}

// Funcion que se ejecuta al comenzar el programa.
void setup(void) {
  
  //Se habilita el puerto serie, el WiFi y la pantalla
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
  
  esp_now_register_recv_cb(OnDataRecv);
}

// Función que se activa continuamente en bucle.
void loop() {
	
  /* Si se detecta que el equipo A o B ha conseguido un punto, se suma 
  un punto al equipo A o B respectivamente.
  */
  if (myData.puntEnvB == 1) {
    addPuntB();
  }
  if (myData.puntEnvA == 1) {
    addPuntA();
  }
  
  // Se borra la pantalla y se vuelve a pintar con los resultados actualizados.
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  printResultado();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
}
