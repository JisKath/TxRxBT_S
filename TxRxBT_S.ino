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

unsigned long t0, t1, t;

RF24 radio(9,10);
uint64_t pipe[2] = {0x63616E616CLL,0x63616E616DLL };

SpecialFn timer1;
BT_Network disp;

#include "RadioComm_S.h" 
#include "RadioCmds.h"

void setup(void)
{
   pinMode(5, OUTPUT);
  //pinMode(4, INPUT);

  digitalWrite(5, HIGH);
  
	delay(2000);
	Serial.begin(38400);
	Serial.println("Inicializando Comm");
  
	disp.reeprom();
	
	disp.wd(0,"canal");
	disp.wn(0,"noname");
	
	if(selectPipe(String(disp.Dispositivo[1].direccion))==0)
		disp.wd(1,"canal");
	
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
  
	timer1.TON.pre=1000;
	timer1.TON.en=0;
}
 
void loop(void)
	{

	timer1.init();	
	timer1.TON.en=1;

	if ( radio.available() )  // Si hay datos disponibles
	{
		cmd_rcvd="";
		bool done = false;

		cmd_rcvd=RadioRead();

		Serial.print("Datos Recibidos: ");
		Serial.println(cmd_rcvd);
		delay(100);	// Para dar tiempo al emisor

		radio.stopListening();	// Dejamos d escuchar para poder hablar
		
		RadioCmds();

  	bool ok = false;
		int TXattempts=0;
		while(!ok && TXattempts<30)
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
	
  }


  
