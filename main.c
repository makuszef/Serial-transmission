#include <REGX52.H>

/*
Autor Hubert Makowski WCY21KY1S1
Program napisany dla mikrokontrolera AT89C52
Program realizuje transmisje szeregowa synchroniczna 
*/

//sygnaly sterujace
sbit clkM = P2^0;
sbit dtaM = P2^1;
sbit clkS = P2^2;
sbit dtaS = P2^3;

//zmienne wykorzysywane dla procedury czekania
bit bM = 1;
int dt = 10;
int tt = 100;

//zapisywany bajt danych dla Master
volatile unsigned char liczbaM = 0x00;
//zapisywany bajt danych dla Slave
unsigned char liczbaS = 0x00;

/*procedura czekania okreslona przez iteracje petli
 wejscie
    i - liczba calkowita okreslajaca liczbe iteracji petli
 wyjscie - brak
 */
void czekaj(int i) {
	unsigned int k, l, m;
	for(l = 0; l < i; l++){
	   k = 500;
	   m = 1000;
	   k= m*l;
	}
}

  /*procedura obslugi przerwania
 wejscie - brak
 wyjscie - brak
 */

void liczbaInt0()interrupt 0 {
 	EX0 = 0;
 	liczbaM++;
	if(liczbaM == 255)
		liczbaM = 0;
	P1 = liczbaM;
	EX0 = 1;
	bM = 0;
}

 /*procedura zapisu bajtu danych
 wejscie
    bajt - liczba zapisana szestanstkowo
 wyjscie - brak
 */

void zapiszBajt(unsigned char bajt) {
	unsigned char liczbaBitow = 8;
	bit paritybit;
	//przeslij bit startu
    dtaM = 0x00;
	czekaj(dt);
	czekaj(tt);
	clkM = 0;
	czekaj(tt);
	clkM = 1;
	czekaj(tt);
	//koniec przesylania bit startu
	
	//przeslij bity danych
	while(liczbaBitow > 0)
	{
		//czytaj najmniej znaczacy bit
		dtaM = bajt & 0x01;
		czekaj(dt);
		czekaj(tt);
		clkM = 0;
		czekaj(tt);
		clkM = 1;
		czekaj(tt);
		bajt = (bajt>>1);
		//oblicz bit parzystosci
		if (liczbaBitow == 8)
			paritybit = dtaM;
		else {
			paritybit = paritybit ^ dtaM;
		}
		liczbaBitow -= 1;
	}
	//przeslij bit parzystosci
	dtaM = paritybit;
	czekaj(dt);
	czekaj(tt);
	clkM = 0;
	czekaj(tt);
	clkM = 1;
	czekaj(tt);
	//koniec przesylania bit parzystosci

	//przeslij bit stopu
    dtaM = 0x01;
	czekaj(dt);
	czekaj(tt);
	clkM = 0;
	czekaj(tt);
	clkM = 1;
	czekaj(tt);
	//koniec przesylania bit stopu
	}
	
 /*procedura odczytu bajtu danych
 wejscie - brak
 wyjscie 
 wynik - odczytany bajt danych
 */

unsigned char czytajBajt() {
	//liczba bitow do odczytania
	unsigned char liczbaBitow = 11;
	unsigned char wynik = 0x00;
	//zmienne pomocnicze
	int i = 0;
	unsigned char gbit;
	//obliczany bit parzystosci
	bit computedParitybit;
	bit paritybit;
	while (liczbaBitow > 0) 
	{
		while(clkS == 1) {
			czekaj(dt);
		}
		//odczytaj bit startu
		if (liczbaBitow == 11)
		{
			bit bitstartu = dtaS;
			//if (bitstartu != 0)blad transmisji danych
		}
		//odczytaj bity danych
		if (liczbaBitow < 11 && liczbaBitow > 2)
		{
			//oblicz bit parzystosci
			if (liczbaBitow == 10)
			{
				computedParitybit = dtaS;
			}
			else {
				computedParitybit = computedParitybit ^ dtaS;
			}
			gbit = dtaS;
			//oblicz przesylany bit
			wynik |= (gbit << i);
			i++;
		}
		//odczytaj bit parzystosci
		if (liczbaBitow == 2)
		{
			paritybit = dtaS;
		}
		//odczytaj bit stopu
		if (liczbaBitow == 1)
		{
			bit bitstopu = dtaS;
			//if (bitstopu != 1)blad transmisji danych
		}
		while(clkS == 0) {
			czekaj(dt);
		}
		liczbaBitow -= 1;
	}
	//if (computedParitybit ^ paritybit != 0)blad transmisji danych 
	return wynik;
}
void initInt0() {
	liczbaM = 0;

	IT0 = 1;   	//INT0 aktywne zero
	EX0 = 1;    //Wlaczenie INT0
	EA = 1;		//Wlaczenie przerwan
}
int main() {

	initInt0();
	P1 = 0;
	clkM = 1;
	dtaM = 1;

	while (P0_0 == 1) {//Master
		while(bM) {
			czekaj(dt);
		}
		P1 = liczbaM;
		zapiszBajt(liczbaM);
		bM = 1;
	}
	while (P0_0 == 0) {//Slave
		while(clkS == 1) {
			czekaj(dt);
		}
		liczbaS = czytajBajt();
		P1 = liczbaS;
	}	
	return 0;
}