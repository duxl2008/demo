#include "strbase.h"
char HexToASCII(unsigned char  data_hex)
{
  char  ASCII_Data;
  ASCII_Data=data_hex & 0x0F;
  if(ASCII_Data<10)
    ASCII_Data=ASCII_Data+0x30; //��0--9��
   else
  ASCII_Data=ASCII_Data+0x37;   //��A--F��
  return ASCII_Data;
}
char ASCIIToHex(unsigned char ASCII_Data)
{
  char  HByte;

  if(ASCII_Data>='0' && ASCII_Data<='9')
  {
    HByte=ASCII_Data-'0';
  }
  else if(ASCII_Data>='A' && ASCII_Data<='F')
  {
    HByte=ASCII_Data-'A' +10;
  }
  else if(ASCII_Data>='a' && ASCII_Data<='f')
  {
    HByte=ASCII_Data-'a' +10;
  }
  else HByte=0;
  return HByte;
}

unsigned long StringToBin(char *OutStrBuffer, string tmp)
{
  unsigned long begin = 0;
  unsigned long len=tmp.length();
  unsigned char *cbuf=new unsigned char [len];
  char *p=tmp.begin();
  unsigned long k=0;
  unsigned long i;
  char c;
  for(i=0;i<len;i++)
  {
      c=p[i];
      if(((c>='0')&&(c<='9'))||((c>='a')&&(c<='f'))||((c>='A')&&(c<='F')))
      {
         cbuf[k++]=c;
      }
  }
  len=k/2;
  p=cbuf;
  for(i=0;i<len;i++)
  {
     OutStrBuffer[i]=ASCIIToHex(*p)<<4;
     p++;
     OutStrBuffer[i]|=ASCIIToHex(*p);
     p++;
  }
  delete cbuf;
  return len;
}

void HexToString(char *OutStrBuffer, unsigned char *InHexBuffer, unsigned long HexLength)
{
  unsigned long i, k=0;
  for(i=0;i<HexLength;i++)
  {
    OutStrBuffer[k++]=HexToASCII((InHexBuffer[i]>>4)&0x0F);
    OutStrBuffer[k++]=HexToASCII(InHexBuffer[i]&0x0F);
    if(i!=(HexLength-1))
     OutStrBuffer[k++]=' ';
  }
  OutStrBuffer[k]='\0';
}

char StringToHex(unsigned char *OutHexBuffer, char *InStrBuffer, unsigned int strLength)
{
 unsigned int i, k=0;
 unsigned char HByte,LByte;
 if(strLength%2 !=0)
  return false;

 for(i=0; i<strLength; i=i+2)
 {
    if(InStrBuffer[i]>='0' && InStrBuffer[i]<='9')
    {
      HByte=InStrBuffer[i]-'0';
    }
    else if(InStrBuffer[i]>='A' && InStrBuffer[i]<='F')
    {
      HByte=InStrBuffer[i]-'A' +10;
    }
    else
    {
      HByte=InStrBuffer[i];
      return false;
    }
    HByte=HByte <<4;
    HByte = HByte & 0xF0;
    if(InStrBuffer[i+1]>='0' && InStrBuffer[i+1]<='9')
    {
      LByte=InStrBuffer[i+1]-'0';
    }
    else if(InStrBuffer[i+1]>='A' && InStrBuffer[i+1]<='F')
    {
      LByte=InStrBuffer[i+1]-'A' +10;
    }
    else
    {
      LByte=InStrBuffer[i];
      return false;
    }
    OutHexBuffer[k++]=HByte |LByte;
 }
 return true;
}
