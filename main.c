/*--------------------------------------------------------------------------------------------------------
Technika Mikroprocesorowa 2 - projekt 
Temat 12: Odbiornik alfabetu Morse’a: czujnik œwiat³a jest odbiornikiem, a
nadajnikiem impulsy z latarki.
autorzy: Greñ Józef, ¯aba Jakub
---------------------------------------------------------------------------------------------------------*/
#include "MKL05Z4.h"	  
#include "pit.h"
#include "ADC.h"
#include <stdlib.h>
#include "lcd1602.h"
#include "frdm_bsp.h"
#include <stdio.h>
#include <math.h>
#include <string.h>



float adc_volt_coeff = ((float)(((float)2.91) / 4095) );			// Wspólczynnik korekcji wyniku, w stosunku do napiecia referencyjnego przetwornika
uint8_t wynik_ok=0;
uint16_t temp;
float	wynik;
uint8_t flaga=0;
uint8_t fstart=0;
uint8_t raz=0;
uint8_t okres=0;
uint8_t pauza=0;
int pozycja=0;
int kolejnaLitera=0;
int tabmorIndeks=0;
void PIT_IRQHandler()
{
	
	if(wynik>2){ //limit od ktorego zczytujemy znaki
		okres++; //liczymy sekundy swiecenia
		pauza=0;
		flaga=1;
		fstart=1; //ustawiamy flagestart na 1
	}
	else{
		pauza++; //liczmy sekundy nie swiecenia :')
		raz=1;

		flaga=0;
		if(pauza==3||pauza==5)//jesli przerwa miedzy 3s a 5s dopisz kolejna litere
			{

				tabmorIndeks=0; //zerujemy pozycje w naszej tablicy
			}

	}
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;		// Skasuj flage zadania przerwania
}

void ADC0_IRQHandler()
{	
	temp = ADC0->R[0];		// Odczyt danej i skasowanie flagi COCO
	if(!wynik_ok)					// Sprawdz, czy wynik skonsumowany przez petle glówna
	{
		wynik = temp;				// Wyslij nowa dana do petli glównej
		wynik_ok=1;
	}
}
int main (void)
{	
	uint8_t	kal_error;
	int mor1[5]={0,0,0,0,0}; //tablica ktora bierze udzial w wypisywaniu znakow w naszym algorytmie
	char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	char display2[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	char letter[33] = "**ETIANMSURWDKGOHVF?L?PJBXCYZQ??"; // tablica zawierajac znaki kodu morsa
	LCD1602_Init();		 // Inicjalizacja wyswietlacza LCD
	LCD1602_Backlight(TRUE);																							 
	PIT_Init();							// Inicjalizacja licznika PIT0
	
	kal_error=ADC_Init();		// Inicjalizacja i kalibracja przetwornika A/C
	if(kal_error)
	{
		while(1);							// Klaibracja sie nie powiodla
	}
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(8);		// Odblokowanie przerwania i wybranie kanalu nr 8




	while(1)
	{
		if(wynik_ok)
		{
			wynik = wynik*adc_volt_coeff;		// Dostosowanie wyniku do zakresu napieciowego
			
			sprintf(display,"U=%.4fV",wynik);
			LCD1602_SetCursor(0,0);
			LCD1602_Print(display);
			
			if(flaga==0&&raz==1){ //jesli jest przerwa(U < 2)
				
				// znak - swiecenie w sekundkach
				// przerwa - brak swiecenia w sekundkach 
				if(okres==1||okres==2){ //jesli swiecenie od 1s do 2s tzw. kropka
					raz=0;
					
					okres=0;

					kolejnaLitera++;
				}
				if(okres>=3){ //jesli swiecenie >= 3 s tzw. kreska
				
					raz=0;
					okres=0;
					mor1[kolejnaLitera]=1;
					kolejnaLitera++;

				}
				
				if(pauza==3&&fstart==1){ //  po przerwie 3s oraz przy fladze start rownej 1
				// JESLI przerwa po swieceniu trwa 3 sekundy to:
					raz=0;
					tabmorIndeks=pow(2,kolejnaLitera); // (pow(x,y) - podniesienie do potegi x do y)
					for(int i=0;i<kolejnaLitera;i++)
					{
							tabmorIndeks+=(pow(2,i)*mor1[kolejnaLitera-1-i]);
							mor1[kolejnaLitera-1-i]=0;
					}
					kolejnaLitera=0;
					pozycja++;
					okres=0;
					LCD1602_SetCursor(pozycja-1,1);
					sprintf(display2,"%c",letter[tabmorIndeks]); // wypisz znak z tablicy morsa
					LCD1602_Print(display2);
					
				}
				if(pauza==7&&fstart==1){
					
					okres=0;
					pozycja++; //robimy tzw "spacje" czyli odstep miedzy kolejnymi znakami 
					pauza=0;
					fstart=0; //zerujemy, poniewaz przechodzimy do kolejnego znaku
					
					pozycja=pozycja%14; // max pozycja 14
					raz=0;
				}
				okres=0;
		}
			wynik_ok=0;
		}
	}
}
