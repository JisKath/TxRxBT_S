/*  ----------------------------------------------------------------
--------------------------------------------------------------------  
*/

#include <SpecialFunctions.h>
#include <BT_Net.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

String theMessage="", cmd_rcvd="";
int msg[1], cmd_disp=0, valor=1, reconfig=0;

unsigned long t0, t1, t, Duty_us=5000;

RF24 radio(9,10);
uint64_t pipe[2] = {0x63661E616CLL,0x63616E616DLL };

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
	Serial.begin(38400);
	Serial.println("Inicializando Comm");
  
	disp.reeprom();
	
	disp.wd(0,"dflt7");
	disp.wn(0,"noname");
	
	if(selectPipe((disp.Dispositivo[1].direccion))>selectPipe("ZZZZZ"))
		disp.wd(1,"dflt7");
	
	pipe[0]=selectPipe(String(disp.Dispositivo[1].direccion));
	pipe[1]=selectPipe(String(disp.Dispositivo[1].direccion))+1;
	
	disp.weeprom();
	disp.reeprom();
	
	Serial.println(String(disp.Dispositivo[0].direccion));
	Serial.println(String(disp.Dispositivo[1].direccion));
	
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	radio.setRetries(15,15);
	radio.setChannel(30);
	radio.setPALevel(3);
	radio.setPayloadSize(16);
	radio.startListening();
	radio.openReadingPipe(1,pipe[0]);
	radio.openWritingPipe(pipe[1]);
  
	blinkOn.TON.pre=100;							//Inicilizando Timers
	blinkOff.TON.pre=100;
	blinkOn.TON.en=0;
	blinkOff.TON.en=0;

    attachInterrupt(digitalPinToInterrupt(3), Dimmer, RISING);
}
 
void loop(void)
	{
	blinkOff.init();
	blinkOn.init();	
	blinkOn.TON.en=1;

    

	if(blinkOn.TON.tt)
		digitalWrite(13, HIGH);

	if(blinkOn.TON.dn)
		{
		digitalWrite(13, LOW);
		blinkOff.TON.en=1;
		if (blinkOff.TON.dn)
			{
			blinkOn.TON.en=0;
			blinkOff.TON.en=0;
			}
		}

	if ( radio.available() )  // Si hay datos disponibles
	{
		cmd_rcvd="";
		bool done = false;

		cmd_rcvd=RadioRead();


		Serial.print("Datos Recibidos: ");
		Serial.println(cmd_rcvd);


		radio.stopListening();	// Dejamos d escuchar para poder hablar
//		radio.openWritingPipe(pipe[1]);
		delay(100);	// Para dar tiempo al emisor
		
		RadioCmds();
		


  	bool ok = false;
		int TXattempts=0;
		while(!ok && TXattempts<5)
		{
			delay(100);
		  ok = RadioWrite(cmd_rcvd);
			++TXattempts;
		}
		
		Serial.print("Intentos: ");
		Serial.println(TXattempts);
	
		if (ok)
    {
      Serial.println("ok...;"); 
      Serial.print("Dato Enviado: ");
      Serial.println(cmd_rcvd);
    }
   
		else
			Serial.println("Falla al transmitir;");		
		
		
//		bool ok = RadioWrite(cmd_rcvd);

		cmd_rcvd="";
		radio.startListening();    // Volvemos a la escucha para recibir mas paquetes
	}

	if (reconfig==1)
	{
		radio.openReadingPipe(1,pipe[0]);
		radio.openWritingPipe(pipe[1]);
		radio.startListening();    // Volvemos a la escucha para recibir mas paquetes
		reconfig=0;
	
	}


if(!(valor==1 || valor==0)){
  if(crossZero){
    crossZero=false;
    digitalWrite(5, LOW);
    delayMicroseconds(Duty_us);
    digitalWrite(5, HIGH);
    }
}else
{
  if (valor==1)digitalWrite(5, HIGH);

  if (valor==0)digitalWrite(5, LOW);
  }


	
  }

void Dimmer() {
  crossZero = true;
  digitalWrite(5, LOW);
}

  
