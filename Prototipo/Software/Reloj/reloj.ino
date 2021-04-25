/**
*	Reloj marcador de padel, versión del HELTEC WiFi Kit 32
*	
*	Autores:
*		Fernando Garcia Gutierrez
*		Javier Lopez Iniesta
*		Ivan Martin Canton
*		Iñigo Montesino Sarmiento
*		Luis de Pablo Beltran
**/

#include "Arduino.h"
#include "heltec.h"
#include "WiFi.h"
#include "GFButton.h"
#include <esp_now.h>

// Se define un boton en el puerto 12 .
GFButton btn1 = GFButton(12);

// Se definen las variables para almacenar la puntuación de los equipos.
int set1A = 0;
int set2A = 0;
int set3A = 0;
int set1B = 0;
int set2B = 0;
int set3B = 0;
int puntA = 0;
int puntB = 0;

// Se crean las variables para representar la puntuación en pantalla.
String puntA_pantalla ; // 0 15 30 40
String puntB_pantalla ; // 0 15 30 40

// Se crean las variables que controlan el funcionamiento del dispositivo.
int set = 1; // set 1, 2, 3
boolean tie = false;
boolean win = false;
boolean lose = false;

// Se crean las variables para la comunicacion con los otros dispositivos.
uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x0D, 0x93, 0x5C};
uint8_t broadcastAddress2[] = {0x7C, 0x9E, 0xBD, 0xFB, 0x1D, 0xDC};

/* Se crea una estructura que almacena cuando se pulsan los mensajes para poder 
realizar la comunicacion con el resto de dispositivos
*/
typedef struct struct_message {
  int puntEnvA;
  int puntEnvB;
} struct_message;

struct_message myData;

/* Funcion que devuelve por el terminal el estado del envío de la información al 
resto de dispositivos.
*/
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[12];
  Serial.print("Packet to: ");
  // Copia la direccion mac del emisor a una String.
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Funcion que almacena el valor recibido en una constante de tipo struct_message.
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println(myData.puntEnvB);
}

/* Funcion que cuando es llamada, aumenta un juego al equipo A en funcion del set en el 
que se encuentren.
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

/* Funcion que cuando es llamada, aumenta un juego al equipo B en funcion del set en el que
 se encuentren.
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
void addPuntA(GFButton & button) {
	
  // Se suma un punto al equipo A.
  puntA ++;
  
  // Se informa y envia este cambio al resto de dispositivos.
  myData.puntEnvA = 1;
  esp_err_t result = esp_now_send(0, (uint8_t *) &myData, sizeof(myData));

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
  
  /* Si se encuentra en el primer set y el equipo A tiene mas de 6 juegos y 
  hay una diferencia de 2 o más juegos entre los dos (o si tiene 7 juegos 
  para el tie break), suma un set y tie pasa a valer falso. Si los dos llegan 
  a 6 juegos, tie pasa a valer true.
  
  */
  if (set == 1) {
    if ((set1A >= 6 && ((set1A - set1B) >= 2)) || set1A == 7) {
      set ++;
      tie  = false;
    } else if ((set1A == 6) && (set1B == 6)) {
      tie = true;
    }
  }
  
  /* Exactamente igual que en el set anterior, pero si detecta además que el equipo A ganó 
  el set anterior, el equipo A gana.
  */
 if (set == 2) {
    if ((set2A >= 6 && ((set2A - set2B) >= 2)) || set2A == 7) {
      if (set1A > set1B) {
        win = true;
      } else {

        set ++;
        tie  = false;
      }
    } else if ((set2A == 6) && (set2B == 6)) {
      tie = true;
    }
  }
  
  /* Exactamente igual que el set anterior, pero comprobando el set 1 y el 2 para poder 
  dar la victoria al equipo A. 
  */
 if (set == 3) {
    if ((set3A >= 6 && ((set3A - set3B) >= 2)) || set3A == 7) {
      if (set1A > set1B || set2A > set2B) {
        win = true;
      } else {
        set ++;
        tie  = false;
      }
    } else if ((set3A == 6) && (set3B == 6)) {
      tie = true;
    }
  }


}

// Funcion que cuando es llamada, suma un punto al equipo B.
void addPuntB() {

  // Se suma un punto al equipo B
  puntB ++;
  
  /* Se reinicia el valor de la variable que indica que el equipo B ha ganado un punto 
  para poder detectar el siguiente cambio
  */
    myData.puntEnvB = 0;

  /* Solo añade un juego si el equipo B tiene mas de 3 puntos (o de 7 si estan en tie break) 
  y una diferencia de 2 puntos frente al equipo A.
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
  
  /* Si se encuentra en el primer set y el equipo B tiene mas de 6 juegos y 
  hay una diferencia de 2 o más juegos entre los dos (o si tiene 7 juegos para el tie break), 
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
        lose = true;
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
        lose = true;
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


/* Funcion que pinta en la pantalla la puntuacion de la partida y el resultado final del equipo A.
*/
void printResultado() {
	
  // Se convierten las variables a la puntuacion de padel en funcion de si se encuentran en tie break o no.
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
  if (win == false && lose == false) {

  Heltec.display->setFont(ArialMT_Plain_16);

  Heltec.display->drawString(0, 10, "EQUIPO A");

  Heltec.display->drawString(0, 25, "A:");

  Heltec.display->drawString(0, 40, "B:");
  
  Heltec.display->drawString(20, 25, String(set1A));

  Heltec.display->drawString(40, 25, String(set2A));

  Heltec.display->drawString(60, 25, String(set3A));

  Heltec.display->drawString(20, 40, String(set1B));

  Heltec.display->drawString(40, 40, String(set2B));

  Heltec.display->drawString(60, 40, String(set3B));

  Heltec.display->drawString(100, 25, puntA_pantalla);

  Heltec.display->drawString(100, 40, puntB_pantalla);
  
  } 
  
  //Se pinta la pantalla de victora o derrota en funcion del resultado final.
  else if (win == true) {
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 20, "Has ganado");
  Heltec.display->drawString(0, 40, "congrats!!");
  } 
  else if (lose == true) {
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 20, "Has perdido");
  Heltec.display->drawString(0, 40, "pringado!!");
  }
}


// Funcion que se ejecuta al comenzar el programa.
void setup() {
	
  //Se habilita el puerto serie, el WiFi y se crea un boton
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

  // Se establece la conexion con el otro reloj y la pantalla
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);

}

// Función que se activa continuamente en bucle.
void loop() {
  btn1.process();
  
  // Si se detecta que el equipo B ha conseguido un punto, se suma un punto al equipo B.
  if (myData.puntEnvB == 1) {
    addPuntB();
  }

  // Se borra la pantalla y se vuelve a pintar con los resultados actualizados.
  Heltec.display->clear();
  printResultado();
  Heltec.display->display();
  delay(5);
}
