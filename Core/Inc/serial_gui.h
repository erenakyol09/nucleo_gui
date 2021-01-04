/*
-Bu kutuphaneyi hernhangi bir stm32 serisinde kullanabilmek icin CubeMx uzerinden "hal library" kullanilmasi sartiyla asagidaki
 f4' u,kullanacaginiz seri ile degistirmek yeterli olacaktir. 
-Ayrica hazirlanan gui tasarimina özellesmis olup, farkli marka islemci serisine de donusturulebilmesi
kolay olmasi amaclanmistir.	
	
	*örnek--> kullanilacak islemci = stm31f103c8t6 --> #include "stm32f1xx_hal.h" 
	
	*gui'den gelen paket yapisi                    -->  paket sayisi + paketler + crc(low 2 byte)
	
	*gui'ye gonderilen paket yapisi                -->  "A,B,C" + paket sayisi + paketler + checksum(low 2 byte)
		
*/

#include "stm32f7xx_hal.h"

//byte okuma
char readByte(UART_HandleTypeDef* huart);

//byte gonderim
void writeByte(UART_HandleTypeDef* huart, char pData_gui);

//string okuma
void readString(UART_HandleTypeDef* huart, char buffer[20]);

//string gonderme
void writeString(UART_HandleTypeDef *huart,char buffer[20]);

//guiden gelen paketleri okuma
void receiveAsciiPackets(char buffer[50],char packet[50]);

//guiye  A modundaki paketlerin gonderimi
void sendmodA_Packets(UART_HandleTypeDef *huart,int number_message,char buffer[number_message][100]);

//guiye  B modundaki paketlerin gonderimi
void sendmodB_Packets(UART_HandleTypeDef *huart,float power,float voltage,float current,float resistor);

//guiye  C modundaki paketlerin gonderimi
void sendmodC_Packets(UART_HandleTypeDef *huart,float P,float Vrms,float Irms,float pf,float f,float dcCur,float dcVol);

//guiden gelen paketleri temel aritmetik operatorlerle kullanabilmek icin floata donusturme
float packetsTofloatValue(UART_HandleTypeDef* huart, char buffer[20],char packet[20]);

// char - > int 
int charToint(char x);

// char - > float
float charTofloat(char x);

// string - > float
float stringTofloat(char buffer[20]);



