/*  ----------------------------------------------------------------
  --------------------------------------------------------------------
*/

#include <SpecialFunctions.h>
#include <BT_Net.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <HC11RF.h>

String theMessage = "", cmd_rcvd = "";
int msg[1], cmd_disp = 0, valor = 1, reconfig = 0, TXattempts = 0;

unsigned long t0, t1, t, Duty_us = 5000;

bool ok = false, done = false;

SoftwareSerial RF(9,8); 							// HC-11 TX Pin, HC-11 RX
HC11RF HC11(RF,38400);

String pipe[2];

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
	delay(20);

	Serial.begin(38400);
	Serial.println("Inicializando Comm");

	disp.reeprom();

	disp.wd(0, "02@05");
	disp.wn(0, "noname");
	
	if((disp.Dispositivo[1].direccion[0]>47) && (disp.Dispositivo[1].direccion[0]<123)){
		Serial.print("Direccion ");
		Serial.print(String(disp.Dispositivo[1].direccion));
		Serial.println(" Ok ");
	}
	else{
		Serial.println("Direccion NOK, se carga dir: 02@05");
		disp.wd(1,"02@05");
	}

	pipe[0] = selectPipe(String(disp.Dispositivo[1].direccion));
	pipe[1]=selectPipeL(String(disp.Dispositivo[1].direccion));

	disp.weeprom();
	disp.reeprom();

	Serial.println(String(disp.Dispositivo[0].direccion));
	Serial.println(String(disp.Dispositivo[1].direccion));

	HC11.cmdPin(10);
	HC11.ATmode(true);

	Serial.println("Configurando modulo HC11");

	Serial.println(HC11.Baudios("38400"));
	Serial.println(RF.readString());

	Serial.println(HC11.Addr(selectPipe(disp.Dispositivo[1].direccion)));
	Serial.println(RF.readString());

	Serial.println(HC11.Canal(selectPipeL(disp.Dispositivo[1].direccion)));
	Serial.println(RF.readString());

	Serial.println(HC11.Potencia("8"));
	Serial.println(RF.readString());

	Serial.println(HC11.Info());
	Serial.println(RF.readString());

	Serial.println(HC11.Version());
	Serial.println(RF.readString());

	Serial.println(HC11.Function("3"));
	Serial.println(RF.readString());

	HC11.ATmode(false);

	blinkOn.TON.pre = 5;							//Inicilizando Timers
	blinkOff.TON.pre = 25;
	blinkOn.TON.en = 0;
	blinkOff.TON.en = 0;

	RF.setTimeout(20);

	attachInterrupt(digitalPinToInterrupt(3), Dimmer, RISING);
}

void loop(void)
{
	blinkOff.init();
	blinkOn.init();
	blinkOn.TON.en = 1;
	blinkOff.TON.en = 1;

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
				blinkOn.TON.en = 0;
				done = false;
			}

			if (TXattempts > 5) {
				Serial.println("Falla al transmitir;");
				blinkOn.TON.en = 0;
				done = false;
			}
		}

	if ( RF.available() && !done && blinkOff.TON.dn )  // Si hay datos disponibles.
	{
		cmd_rcvd = "";
		cmd_rcvd = RadioRead();

		Serial.print("Datos Recibidos: ");
		Serial.println(cmd_rcvd);

		RadioCmds();

		ok = false;
		TXattempts = 0;
		done = true;
		blinkOff.TON.en = 0;
	}
	
	if(Serial.available()>3)
	{
	localCmds();	
	}

	if (reconfig == 1)
	{

	}

	if (!(valor == 1 || valor == 0)) 
	{
		if (crossZero && !done) 
		{
			crossZero = false;
			digitalWrite(5, LOW);
			delayMicroseconds(Duty_us);
			digitalWrite(5, HIGH);
		}
	} 
	else
	{
		if (valor == 1)digitalWrite(5, HIGH);

		if (valor == 0)digitalWrite(5, LOW);
	}
}


void Dimmer() {
	crossZero = true;
	digitalWrite(5, LOW);
}
