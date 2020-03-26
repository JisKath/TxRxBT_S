
String selectPipe(String _pipe)
{
  //uint64_t ix= _pipe[4] + _pipe[3]*0x100 + _pipe[2]*0x10000 + _pipe[1]*0x1000000 + _pipe[0]*0x100000000;
  String ix="0";
  ix=ix + _pipe.substring(0,2);
  return(ix); 
}

String selectPipeL(String _pipe)
{

  //uint64_t ix= _pipe[0] + _pipe[1]*0x100 + _pipe[2]*0x10000 + _pipe[3]*0x1000000 + _pipe[4]*0x100000000;
  String ix="0";
  ix=ix + _pipe.substring(3,5);
  return(ix);

}

String RadioRead(void)
{
  String Mensaje;
  Mensaje.reserve(16);
  Mensaje.remove(0, 15);
  
  while(RF.available())
	Mensaje=RF.readString();

  return (Mensaje);
}

bool RadioWrite(String theMessage)
{
  bool a=false;
      
  //theMessage.concat(';');
  HC11.sends(theMessage);
  
  a=true;
  
  return (a);
}
