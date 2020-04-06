
String selectPipe(String _pipe)
{
	String ix="0";
	ix=ix + _pipe.substring(0,2);
	return(ix); 
}

String selectPipeL(String _pipe)
{
	String ix="0";
	ix=ix + _pipe.substring(3,5);
	return(ix);
}

String RadioRead(void)
{
	String Mensaje;
	Mensaje.reserve(16);
	Mensaje.remove(0, 15);

	while(RF.available()){
		Mensaje=RF.readString();
		RF.flush();
  }
  return (Mensaje);
}

bool RadioWrite(String theMessage)
{
  bool a=false;
  HC11.sends(theMessage);
  a=true;
  return (a);
}
