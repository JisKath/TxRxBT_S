
uint64_t selectPipe(String _pipe)
{
  uint64_t ix= _pipe[4] + _pipe[3]*0x100 + _pipe[2]*0x10000 + _pipe[1]*0x1000000 + _pipe[0]*0x100000000;
  return(ix); 
}

uint64_t selectPipeL(String _pipe)
{

  uint64_t ix= _pipe[0] + _pipe[1]*0x100 + _pipe[2]*0x10000 + _pipe[3]*0x1000000 + _pipe[4]*0x100000000;

  return(ix);

}

String RadioRead(void)
{
	
  String Mensaje="";
  bool a=false;
  
  char MSG[16];
  radio.read( MSG,sizeof(MSG));

  for (int i = 0; i < sizeof(MSG); i++) 
    Mensaje.concat(MSG[i]);

  return (Mensaje);
}


bool RadioWrite(String theMessage)
{
  bool a=false;
      char charToSend[16]= {' ', ' ', ' ' ,' ' ,' ', ' ',' ', ' ' ,' ', ' ', ' ' ,' ', ' ', ' ' ,' ',' ' };
      
      
  theMessage.concat(';');
  
  int messageSize = theMessage.length();
  if(messageSize>=16)
    messageSize=15;
  
  for (int i = 0; i < messageSize; i++) 
  {
    charToSend[i] = theMessage.charAt(i);
  }

  a=radio.write(charToSend,messageSize);  
  return (a);
}
