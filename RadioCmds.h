void RadioCmds(void)												//Funcion para busqueda y ejecucion de comandos tipo _XXXX
{
if(cmd_rcvd.substring(0,1) == "_")
{
		
	int cmdOk=0;

	if(cmd_rcvd.substring(0,6) == "_sndn[") //sndn[0,1]
	{
		Serial.println("_sndn ");
		valor=cmd_rcvd.substring(cmd_rcvd.indexOf(',')+1,cmd_rcvd.lastIndexOf(']')).toInt();
		Duty_us=8300-(83*valor);

		//Serial.println(valor);
		//Serial.println(Duty_us);
		cmdOk=1;
	}
	
	if(cmd_rcvd.substring(0,6) == "_rcvn[") //rcvn[0]
	{
		Serial.println("_rcvn enviando");
		cmd_rcvd=(String(valor,DEC));
		cmdOk=1;

	}
	
	if(cmd_rcvd.substring(0,6) == "_cpyd[") //copy[Estudio,canal]
	{
		Serial.println("_copy Recibido");
		
		Serial.println(String(disp.Dispositivo[0].direccion));
		Serial.println(String(disp.Dispositivo[1].direccion));

		disp.wd(1,cmd_rcvd.substring(cmd_rcvd.indexOf(',')+1,cmd_rcvd.lastIndexOf(']')));
		disp.weeprom();
		disp.reeprom();
   
		Serial.println(String(disp.Dispositivo[0].direccion));
		Serial.println(String(disp.Dispositivo[1].direccion));
		
		HC11.ATmode(true);
		HC11.Addr(selectPipe(disp.Dispositivo[1].direccion));
		HC11.Canal(selectPipeL(disp.Dispositivo[1].direccion));
		HC11.ATmode(false);

		cmdOk=1;
		reconfig=1;

	}
	
	if(cmd_rcvd.substring(0,6) == "_clrd[")
	{
		Serial.println("Borrando EEPROM");
		disp.clreeprom();
		disp.reeprom();
		Serial.println("EEPROM ---> SRAM enviado;");
		cmdOk=1;
	}
	
	if (cmdOk==0)
		Serial.println("Comando no identificado");
	
	
}

}

void localCmds(void)
{
  int cmdOk=0;
	String temp="";
	temp=Serial.readString();
 
	if(temp.substring(0,6) == "_clrd[")
	{
		Serial.println("Borrando EEPROM");
		disp.clreeprom();
		disp.reeprom();
		Serial.println("EEPROM ---> SRAM enviado;");
		cmdOk=1;
	}
	
	if (cmdOk==0)
		Serial.println("Comando no identificado");
	
}
