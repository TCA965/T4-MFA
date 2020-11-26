//MFA für T4 unter Verwendung von MCP2515 und MCP2551 für die Kommunikation mit dem CAN-Bus
//DOGM128-R wird als Display verwendet
//Optokoppler über Software-Serial mit K-Linie verbunden, um Fehlercodes auszulesen
//Thaddäus Köhler / Januar 2018
// Frei Pins: A0, A1, A2, 7, 10


//Verwendete Bibliotheken
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Dogm.h>
#include <EEPROM.h>
//#include "EEPROMAnything.h"
#include "Symbols.h"
#include "mcp_can.h"
#include <math.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 8

#define pinKLineRX 4
#define pinKLineTX 3

Adafruit_BMP280 bme(BMP_CS); // hardware SPI


//Pindefinitionen
//Hintergrundbeleuchtung GLCD
const byte LED_Backlight = 5;

//Interrupt-Pin für MCP2515
const byte interruptPin = 2;

//Adresspin für Dogm-Modul
int a0Pin = 6;

//Pins für MFA-Hebel
const int Hebel = A3;

const int Oel_Temp_Pin = A5;
const int Aussen_Temp_Pin = A4;


//Software-Serial RX, TX
SoftwareSerial obd(pinKLineRX, pinKLineTX);

//CAN Chip Select Pin 9
MCP_CAN CAN(9);

//Zuweisung Pin zu LCD-Modul
Dogm dogm(a0Pin);


//Array und Längenfeld für CAN-Nachrichten
unsigned char len = 0;
unsigned char buf[8];
int canID = 0;

//Byte-Variablen
byte Byte_0x280_0, Byte_0x280_3, Byte_0x280_4, Byte_0x280_5, Byte_0x288_1, Byte_0x288_2;
byte Byte_0x288_4, Byte_0x320_0, Byte_0x320_1, Byte_0x320_2, Byte_0x320_4, Byte_0x388_1;
byte Byte_0x420_3, Byte_0x420_4, Byte_0x420_5, Byte_0x480_2, Byte_0x480_3;

byte Stellung;

//Variablen für berechnete Werte
int Geschwindigkeit,  Drehzahl, Oeltemp, Wassertemp1, Wassertemp2, Gaspedal, Tank, seite;
double Oel_Temp, Oel_Temp_alt, Aussen_Temp, BMP_Temp, BMP_Druck, BMP_Hoehe;
int Tempo_geschwindigkeit = 0;
float Tank_berechnet;
double Reichweite = 0;
double Geschwindigkeit_trip = 0;
unsigned long CurrentVB = 1;
unsigned long CurrentVBold = 0;
unsigned long LastVB = 0;
//True heißt getreten oder aktiv (oder offen)
boolean bremse, kupplung, handbremse, licht, fahrer, tempo_an, tempo_aktiv, tempo_set, tempo_reset;

//VB = Verbrauch, VBh = Verbrauch pro Stunde, VBkm = Verbrauch auf 100km, VBges = Verbrauch seit Reset, VBtrip = Verbrauch seit Zündung an
//VBtrip ist Faktor 100 zu groß, bei Ausgabe durch 100 teilen um 2 Nachkommastellen zu erhalten
double VB, VBh, VBkm, VBges, VBtrip;

//"Zündungs"Variablen
unsigned long liter = 0;
unsigned long strecke = 0;
boolean speichern = false;

//Gesamt Variablen
//Strecke in m (Hilfsvariable)
float _strecke = 0;
//Strecke in m
unsigned long liter_ges = 0;
unsigned long strecke_ges = 0;


//Timer für Verbrauchsberechnung, Intervall 1000ms
unsigned long currentMillis;
unsigned long VB_lastMillis = 0;
unsigned long VB_interval = 500;

//Timer für Ausgabe, Intervall 350ms
unsigned long out_lastMillis = 0;
unsigned long out_interval = 350;

//Timer für Druck-Ausgabe, Intervall 3000ms
unsigned long bmp_lastMillis = 0;
unsigned long bmp_interval = 3000;

//Timer für gefahrene Strecke und Fahrzeit (in Sekunden)
unsigned long strecke_lastMillis = 0;
unsigned long strecke_interval = 1000;
unsigned long fahrzeit = 0;

//Timer für Reset erkennung (Taste muss für 3s gedrückt werden)
unsigned long reset_start = 0;
boolean reset_pressed = false;

//Timer für die Neustart erkennung (Taste muss für 3s gedrückt werden)
unsigned long restart_start = 0;
boolean restart_pressed = false;

//Variablen zur Entprellung des MFA Hebels
boolean next_pressed = false;
boolean next_pressed_old = false;
boolean next_val = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

//Durchschnittsverbrauch über letzten 5 km ausgeben
float FuenfKm[6] = { 0, 0, 0, 0, 0, 0 };
unsigned long FuenfKmStrecke = 0;
unsigned long FuenfKmLiter = 0;
//int FuenfKmZaehler = 0;
float FuenfKmRes = 0;

void setup()
{
  //Schriftart
  dogm.setFont(font_6x10);
  //Hintergrundbeleuchtung als Ausgang definieren
  pinMode(LED_Backlight, OUTPUT);
  pinMode(Hebel, INPUT);
  pinMode(Oel_Temp_Pin, INPUT);
  pinMode(Aussen_Temp_Pin, INPUT);

  //LCD aus Tag-Modus einschalten
  analogWrite(LED_Backlight, 200);

  //(SW)Serielle-Schnittstelle mit 9600 baud initialisieren (Benötigt vom Motorsteuergerät)
  pinMode(pinKLineTX, OUTPUT);
  digitalWrite(pinKLineTX, HIGH);

  Serial.begin(19200);
  Serial.println(F("START"));

  bme.begin();
  delay(250);

  //CAN-Prozessoren verbinden
START_INIT:

  //VW-Motorcan nutzt 500kB/s
  if (CAN_OK == CAN.begin(CAN_500KBPS))
  {
    delay(150);
  }
  else
  {
    //Wenn nicht verbunden warten und erneut verbinden
    delay(100);
    goto START_INIT;
  }


  //Es werden beide Masken auf 0x7ff (dec 2047) gesetzt.
  //0x7ff sagt aus, das alle 11 Bit der normalen ID mit den Filtern verglichen werden
  //Maske 0 ist für die Filter 0 und 1, Maske 1 ist für die Filter 2-5
  CAN.init_Mask(0, 0, 0x7ff);
  CAN.init_Mask(1, 0, 0x7ff);

  //Nun werden die 6 Filter gesetzt. Dadurch wird der Atmega entlastet, da nur Nachrichten
  //von den benötigten 6 Adressen kommen. Die anderen Nachrichten werden vom MCP2515 ignoriert
  CAN.init_Filt(1, 0, 0x280);                          // Motor 1  (DEC 640)
  CAN.init_Filt(2, 0, 0x288);                          // Motor 2  (DEC 648)
  CAN.init_Filt(3, 0, 0x320);                          // KI 1     (DEC 800)
  CAN.init_Filt(4, 0, 0x388);                          // Motor 3  (DEC 904)
  CAN.init_Filt(5, 0, 0x420);                          // KI 2     (DEC 1056)
  CAN.init_Filt(0, 0, 0x480);                          // Motor 4  (DEC 1152)
  delay(100);
  //Daten aus EEPROM holen
  loadData();
}



void loop()
{
  currentMillis = millis();
  //Empfange CAN-Daten
  CAN_rx();
  //Berechne CAN-Daten
  CAN_calc();
  //Berechne sekündlich den Momentanverbrauch
  CAN_VBcalc();
  //Rechne mit ermittelten Werten weiter
  calc();
  //Messwerte von analogen Temperatursensoren holen
  //analog_temp();
  //Wird alle 0.35s von der Ausgabe getriggert
  //Ausgabe alle 0,35s
  dogm_print();
  //MFA Hebel auswerten
  HebelAuswerten();
}

void loadData()
{
  //Insgesamt Verbrauchte Mikroliter seit Reset, startend an EEPROM-Adresse 100
  //liter_ges =  EEPROMReadlong(100);
  EEPROM.get(110, liter_ges);
  
  //Insgesamt gefahrene Meter seit Reset, startend an EEPROM-Adresse 200
  //strecke_ges =  EEPROMReadlong(200);
  EEPROM.get(210, strecke_ges);
  
  //EEPROMWritelong(200, strecke_ges);
  //Aktuelle MFA Seite laden
  //seite = EEPROMReadlong(300);
  EEPROM.get(310, seite);

  EEPROM.get(400, FuenfKm[0]);
  EEPROM.get(405, FuenfKm[1]);
  EEPROM.get(410, FuenfKm[2]);
  EEPROM.get(415, FuenfKm[3]);
  EEPROM.get(420, FuenfKm[4]);
}

