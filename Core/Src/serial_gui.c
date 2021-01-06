//////////////////////////////////////////////////////
//																									//
//         Python - stm32 gui tasarimi						  //																						
//																									//
//////////////////////////////////////////////////////


// UYARI: Bu kutuphaneyi kullanmak istiyorsaniz;globalde tanimlandan degiskenleri main'de tanimlamamalisiniz. 

#include "serial_gui.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define buffer_size 50

char controlBuffer[20] = "";
char crcBuffer[4]      = "";
char pData_gui[1]      = "";

char sendCrc[1]  = "";
char sendCrc2[1] = "";
char sendCrc3[1] = "";
char sendCrc4[1] = "";

char reciveCrc[1]  = "";
char reciveCrc2[1] = "";
char reciveCrc3[1] = "";
char reciveCrc4[1] = "";


	
char sendmodABuf[20][100];
char sendmodBBuf[20][50];
char sendmodCBuf[20][50];
char sendmodlength[2];
char sendmodCpackets[20][50];
char sendmodBpackets[20][50];

float myBfloatValues[4][20];
float myCfloatValues[7][20];


int ii         = 0;
int crc_length = 0;
int length 		 = 0;
int length2 	 = 0;
uint16_t crc   = 0;


int s      = 0;
int a 	   = 0;
int k 		 = 0;

//byte okuma
char readByte(UART_HandleTypeDef* huart)
{
	*pData_gui = 0;
	HAL_UART_Receive(huart, (uint8_t*)pData_gui, 1, 10);
	
	return *pData_gui;	
}

//byte gonderim
void writeByte(UART_HandleTypeDef* huart, char pData_gui)
{
	HAL_UART_Transmit(huart, (uint8_t*)&pData_gui, 1, 1000);
}
//string okuma
void readString(UART_HandleTypeDef* huart, char buffer[buffer_size])
{
	HAL_UART_Receive(huart,(uint8_t *)buffer,buffer_size,1000);
}
//string gonderme
void writeString(UART_HandleTypeDef *huart,char Tx_buffer[50])
{
	char sendString[50];
	HAL_UART_Transmit(huart,(uint8_t*)sendString,sprintf((char *)sendString,"%s",Tx_buffer),10); 	
}	
//guiden gelen paketleri okuma
void receiveAsciiPackets(char buffer[buffer_size],char packet[20])
{	
	for(int i=0;i<=20;i++)
	{
		packet[i]        = 0;
		controlBuffer[i] = 0;		
		crcBuffer[i]		 = 0;			
	}
	length = 0;
	crc    = 0;

	length  = charToint(buffer[2]);
	length2 = charToint(buffer[3]);
	
	length = 10*length + length2;
	
	//paketler controlBuffer adli arraye ekleniyor	
	for(int i=4;i<=length+5;i++)
	{
		controlBuffer[i-4] = buffer[i-2];
	}	
	//gelen crc low 2 byte
	for(int i=0;i<=3;i++)
	{
		crcBuffer[i] = buffer[length+4+i];
	}
	//crc toplama
	for(int i=0;i<strlen(controlBuffer)-2;i++)
	{
		crc = crc + controlBuffer[i+2];
	}
	sprintf(reciveCrc ,"%x",crc & 0xF000);
	sprintf(reciveCrc2,"%x",crc & 0x0F00);
	sprintf(reciveCrc3,"%x",crc & 0x00F0);
	sprintf(reciveCrc4,"%x",crc & 0x000F);
	// crc karsilastirma
	if(reciveCrc[0] == crcBuffer[0] && reciveCrc2[0] == crcBuffer[1] && reciveCrc3[0] == crcBuffer[2] && reciveCrc4[0] == crcBuffer[3])
	{
		for(int i=0;i<length;i++)
		{
			packet[i+1] = controlBuffer[i+2];
		}		
	}
}
//guiye  A modundaki paketlerin gonderimi
void sendmodA_Packets(UART_HandleTypeDef *huart,int number_message,char buffer[number_message][100])
{	
	crc = 0;
	
	for(int i=0;i<number_message;i++)
		{
			//her mesajin ilk indeksine A mod secimi girildi
			sendmodABuf[i][0] ='A';
			//her mesajin ikinci indeksine paket sayisi girildi  
			if(strlen(buffer[i]) < 10)
				{
					sendmodABuf[i][1] = '0'; 
					sprintf(sendmodlength,"%d",strlen(buffer[i])); 
					sendmodABuf[i][2] = sendmodlength[0]; 
				}
				
			if(strlen(buffer[i]) >= 10)
				{
					sprintf(sendmodlength,"%d",strlen(buffer[i])); 
					sendmodABuf[i][1] = sendmodlength[0]; 
					sendmodABuf[i][2] = sendmodlength[1]; 
				}	
				
			for(int j=0;j<=strlen(buffer[i])-1;j++)
				{
					sendmodABuf[i][j+3] = buffer[i][j];
					crc = crc + buffer[i][j];
				}		
				
			sprintf(sendCrc ,"%x",crc & 0xF000);
			sprintf(sendCrc2,"%x",crc & 0x0F00);
			sprintf(sendCrc3,"%x",crc & 0x00F0);
			sprintf(sendCrc4,"%x",crc & 0x000F);
			
			sendmodABuf[i][3+strlen(buffer[i])] = sendCrc[0];
			sendmodABuf[i][4+strlen(buffer[i])] = sendCrc2[0];
			sendmodABuf[i][5+strlen(buffer[i])] = sendCrc3[0];
			sendmodABuf[i][6+strlen(buffer[i])] = sendCrc4[0];			
			crc = 0;
		}
		
		for(int i=0;i<number_message;i++)
		{
			for(int j=0;j<=strlen(sendmodABuf[i])-1;j++)
				{
					writeByte(huart,sendmodABuf[i][j]);
				}		
			writeByte(huart,'\n');	
		}		
}
//guiye  B modundaki paketlerin gonderimi
void sendmodB_Packets(UART_HandleTypeDef *huart,float power,float voltage,float current,float resistor)
{
	myBfloatValues[0][0] = power;
	myBfloatValues[1][0] = voltage;
	myBfloatValues[2][0] = current;
	myBfloatValues[3][0] = resistor;

	
	*sendCrc  = 0;
	*sendCrc2 = 0;
	*sendCrc3 = 0;
	*sendCrc4 = 0;
   crc = 0;
	
	for(int i=0;i<=3;i++)
		{
			sprintf(sendmodBBuf[i],"%.3f",myBfloatValues[i][0]);
		}

	
	for(int i=0;i<=3;i++)
		{
			//her mesajin ilk indeksine B mod secimi girildi
			sendmodBpackets[i][0] ='B';
			//her mesajin ikinci indeksine paket sayisi girildi  
			if(strlen(sendmodBBuf[i]) < 10)
				{
					sendmodBpackets[i][1] = '0'; 
					sprintf(sendmodlength,"%d",strlen(sendmodBBuf[i])+1); 
					sendmodBpackets[i][2] = sendmodlength[0]; 
				}
				
			if(strlen(sendmodBBuf[i]) >= 10)
				{
					sprintf(sendmodlength,"%d",strlen(sendmodBBuf[i])+1); 
					sendmodBpackets[i][1] = sendmodlength[0]; 
					sendmodBpackets[i][2] = sendmodlength[1]; 
				}	
				
				sendmodBpackets[0][3] = 'P';
				sendmodBpackets[1][3] = 'V';
				sendmodBpackets[2][3] = 'I';
				sendmodBpackets[3][3] = 'R';
				
			for(int j=0;j<=strlen(sendmodBBuf[i])-1;j++)
				{
					sendmodBpackets[i][j+4] = sendmodBBuf[i][j];
				}	
				
				for(int j=0;j<=strlen(sendmodBBuf[i]);j++)
				{
					crc = crc + sendmodBpackets[i][j+3];
				}	
				
			sprintf(sendCrc ,"%x",crc & 0xF000);
			sprintf(sendCrc2,"%x",crc & 0x0F00);
			sprintf(sendCrc3,"%x",crc & 0x00F0);
			sprintf(sendCrc4,"%x",crc & 0x000F);
			
			sendmodBpackets[i][4+strlen(sendmodBBuf[i])] = sendCrc[0];
			sendmodBpackets[i][5+strlen(sendmodBBuf[i])] = sendCrc2[0];
			sendmodBpackets[i][6+strlen(sendmodBBuf[i])] = sendCrc3[0];
			sendmodBpackets[i][7+strlen(sendmodBBuf[i])] = sendCrc4[0];	
      sendmodBpackets[i][8+strlen(sendmodBBuf[i])] = '\n';				
			crc =  0;
		}
		
		for(int i=0;i<4;i++)
			{
				for(int j=0;j<=strlen(sendmodBpackets[i])-1;j++)
					{
						writeByte(huart,sendmodBpackets[i][j]);
					}		
				//writeByte(huart,'\n');	
			}	
}	
//guiye  C modundaki paketlerin gonderimi
void sendmodC_Packets(UART_HandleTypeDef *huart,float P,float Vrms,float Irms,float pf,float f,float dcCur,float dcVol)
{
	myCfloatValues[0][0] = P;
	myCfloatValues[1][0] = Vrms;
	myCfloatValues[2][0] = Irms;
	myCfloatValues[3][0] = pf;
	myCfloatValues[4][0] = f;
	myCfloatValues[5][0] = dcCur;
	myCfloatValues[6][0] = dcVol;
	
	*sendCrc  = 0;
	*sendCrc2 = 0;
	*sendCrc3 = 0;
	*sendCrc4 = 0;
   crc = 0;
	
	for(int i=0;i<=6;i++)
		{
			sprintf(sendmodCBuf[i],"%.3f",myCfloatValues[i][0]);
		}

	
	for(int i=0;i<=6;i++)
		{
			//her mesajin ilk indeksine C mod secimi girildi
			sendmodCpackets[i][0] ='C';
			//her mesajin ikinci indeksine paket sayisi girildi  
			if(strlen(sendmodCBuf[i]) < 10)
				{
					sendmodCpackets[i][1] = '0'; 
					sprintf(sendmodlength,"%d",strlen(sendmodCBuf[i])+1); 
					sendmodCpackets[i][2] = sendmodlength[0]; 
				}
				
			if(strlen(sendmodCBuf[i]) >= 10)
				{
					sprintf(sendmodlength,"%d",strlen(sendmodCBuf[i])+1); 
					sendmodCpackets[i][1] = sendmodlength[0]; 
					sendmodCpackets[i][2] = sendmodlength[1]; 
				}	
				
			sendmodCpackets[0][3] = 'P';	
			sendmodCpackets[1][3] = 'U';	
			sendmodCpackets[2][3] = 'J';	
			sendmodCpackets[3][3] = 'p';	
			sendmodCpackets[4][3] = 'f';	
			sendmodCpackets[5][3] = 'V';	
			sendmodCpackets[6][3] = 'I';	
				
				
			for(int j=0;j<=strlen(sendmodCBuf[i])-1;j++)
				{
					sendmodCpackets[i][j+4] = sendmodCBuf[i][j];
				}
					
			for(int j=0;j<=strlen(sendmodCBuf[i]);j++)
				{
					crc = crc + sendmodCpackets[i][j+3];
				}	
				
			sprintf(sendCrc ,"%x",crc & 0xF000);
			sprintf(sendCrc2,"%x",crc & 0x0F00);
			sprintf(sendCrc3,"%x",crc & 0x00F0);
			sprintf(sendCrc4,"%x",crc & 0x000F);
			
			sendmodCpackets[i][4+strlen(sendmodCBuf[i])] = sendCrc[0];
			sendmodCpackets[i][5+strlen(sendmodCBuf[i])] = sendCrc2[0];
			sendmodCpackets[i][6+strlen(sendmodCBuf[i])] = sendCrc3[0];
			sendmodCpackets[i][7+strlen(sendmodCBuf[i])] = sendCrc4[0];
			sendmodCpackets[i][8+strlen(sendmodCBuf[i])] = '\n';						
			crc =  0;
		}
		
		for(int i=0;i<7;i++)
			{
				for(int j=0;j<=strlen(sendmodCpackets[i])-1;j++)
					{
						writeByte(huart,sendmodCpackets[i][j]);
					}		
			}		
}
//guiden gelen paketleri temel aritmetik operatorlerle kullanabilmek icin floata donusturme
float packetsTofloatValue(UART_HandleTypeDef* huart, char buffer[20],char packet[20])
{
	float value = 0;
	char myPacket[20];
	receiveAsciiPackets(buffer,packet);
	for(int i=0;i<19;i++)
	{
		myPacket[i] = packet[i+1];
	}
	
	value = stringTofloat(myPacket);
	
	return value;
}



// char - > int 
int charToint(char x)
{
	int e = 0;
	switch(x)
	{
		case '0': e = 0 ;break;
		case '1': e = 1 ;break;
		case '2': e = 2 ;break;
		case '3': e = 3 ;break;
		case '4': e = 4 ;break;
		case '5': e = 5 ;break;
		case '6': e = 6 ;break;
		case '7': e = 7 ;break;
		case '8': e = 8 ;break;
		case '9': e = 9 ;break;		
		default:break;
	}
	return e;
}
// char - > float
float charTofloat(char x)
{
	float e = 0;
	switch(x)
	{
		case '0': e = 0 ;break;
		case '1': e = 1 ;break;
		case '2': e = 2 ;break;
		case '3': e = 3 ;break;
		case '4': e = 4 ;break;
		case '5': e = 5 ;break;
		case '6': e = 6 ;break;
		case '7': e = 7 ;break;
		case '8': e = 8 ;break;
		case '9': e = 9 ;break;		
		default:break;
	}
	return e;
}

// string - > float
float stringTofloat(char buffer[20])
{
	
	float	value  = 0;
	float tValue = 0;
	
	s = strlen(buffer);
	
	for(int k=0;k<=s;k++)
	{
		if(buffer[k] == '.')
			{
				a = k;
			}
	}

	//tam sayi olma durumu							
	if(a == 0)
	{

		for(int k=1;k<=s;k++)
			{									
				value = charTofloat(buffer[k-1]);
				value = value * pow(10,s-k);
				tValue = tValue + value;								  					
			}
	}
	// tam sayi olmama durumu						
	if(a != 0)
	{
		for(int k=1;k<=s-a-1;k++)
			{									
				value = charTofloat(buffer[k-1]);
				value = value * pow(10,s-a-k-1);
				tValue = tValue + value;								  					
			}
		for(int k=a+1;k<=s-1;k++)
			{									
				value = charTofloat(buffer[k]);
				value = value / pow(10,k-a);
				tValue = tValue + value;								  					
			}
	}
	return tValue;
}
































