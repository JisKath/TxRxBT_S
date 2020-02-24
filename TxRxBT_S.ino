/*  ----------------------------------------------------------------
  --------------------------------------------------------------------
*/

#include <SpecialFunctions.h>
#include <BT_Net.h>
#include <EEPROM.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#include <RF24_config.h>

String theMessage = "", cmd_rcvd = "";
int msg[1], cmd_disp = 0, valor = 1, reconfig = 0, TXattempts = 0;

unsigned long t0, t1, t, Duty_us = 5000;

bool ok = false, done = false;

RF24 radio(9, 10);
uint64_t pipe[2] = {0x63661E616CLL, 0x63616E616DLL };

SpecialFn blinkOn, blinkOff;
BT_Network disp;

#include "RadioComm_S.h"
#include "RadioCmds.h"

volatile bool crossZero = false;

void setup(void)
{
  pinMode(5, OUTPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(13, OUTPUT);



  digitalWrite(5, HIGH);

  delay(2000);
  Serial.begin(19200);
  Serial.println("Inicializando Comm");

  disp.reeprom();

  disp.wd(0, "dflt7");
  disp.wn(0, "noname");

  if((disp.Dispositivo[1].direccion[0]>47) && (disp.Dispositivo[1].direccion[0]<123)){
    Serial.print("Direccion ");
    Serial.print(String(disp.Dispositivo[1].direccion));
    Serial.println(" Ok ");
    }
  else{
    Serial.println("Direccion NOK, se carga dir: dflt0");
    disp.wd(1,"dflt0");
    }

    //disp.wd(1,"dflt7");

  pipe[0] = selectPipe(String(disp.Dispositivo[1].direccion));
  pipe[1]=selectPipeL(String(disp.Dispositivo[1].direccion));

  disp.weeprom();
  disp.reeprom();

  Serial.println(String(disp.Dispositivo[0].direccion));
  Serial.println(String(disp.Dispositivo[1].direccion));

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(15, 15);
  radio.setChannel(30);
  radio.setPALevel(3);
  radio.setPayloadSize(16);
  radio.startListening();
  radio.openReadingPipe(1, pipe[0]);
  radio.openWritingPipe(pipe[1]);

  blinkOn.TON.pre = 5;							//Inicilizando Timers
  blinkOff.TON.pre = 100;
  blinkOn.TON.en = 0;
  blinkOff.TON.en = 0;

  attachInterrupt(digitalPinToInterrupt(3), Dimmer, RISING);
}

void loop(void)
{
  blinkOff.init();
  blinkOn.init();
  blinkOn.TON.en = 1;

  if (blinkOn.TON.tt)
  {};

  if(blinkOn.TON.dn && done)
  if (done && !ok)
  {

    ok = RadioWrite(cmd_rcvd);
    //delay(1);
    ++TXattempts;

    if (ok)
    {
      Serial.print("Intentos: ");
      Serial.println(TXattempts);
      Serial.println("ok...;");
      Serial.print("Dato Enviado: ");
      Serial.println(cmd_rcvd);
      cmd_rcvd = "";
      radio.startListening();    // Volvemos a la escucha para recibir mas paquetes
      blinkOn.TON.en = 0;
      done = false;
    }

    if (TXattempts > 5) {
      Serial.println("Falla al transmitir;");
      blinkOn.TON.en = 0;
      done = false;
      radio.startListening();    // Volvemos a la escucha para recibir mas paquetes

    }

    //		bool ok = RadioWrite(cmd_rcvd);



  }

  if ( radio.available() && !done )  // Si hay datos disponibles.
  {
    cmd_rcvd = "";

    cmd_rcvd = RadioRead();


    Serial.print("Datos Recibidos: ");
    Serial.println(cmd_rcvd);

    radio.stopListening();	// Dejamos d escuchar para poder hablar

    delay(100);	// Para dar tiempo al emisor

    RadioCmds();

    ok = false;
    TXattempts = 0;
    done = true;

  }

  if (reconfig == 1)
  {
    radio.openReadingPipe(1, pipe[0]);
    radio.openWritingPipe(pipe[1]);
    radio.startListening();    // Volvemos a la escucha para recibir mas paquetes
    reconfig = 0;

  }


  if (!(valor == 1 || valor == 0)) {
    if (crossZero && !done) {
      crossZero = false;
      digitalWrite(5, LOW);
      delayMicroseconds(Duty_us);
      digitalWrite(5, HIGH);
    }
  } else
  {
    if (valor == 1)digitalWrite(5, HIGH);

    if (valor == 0)digitalWrite(5, LOW);
  }



}


void Dimmer() {
  crossZero = true;
  digitalWrite(5, LOW);
}
