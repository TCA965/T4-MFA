boolean _sperre = true;

void calc()
{
  if (Geschwindigkeit > 5)
  {
    VBkm = (VBh * 100) / Geschwindigkeit;
  }
  else VBkm = VBh;

  //Alle 1000ms wird über die aktuelle Geschwindigkeit die gefahrene Strecke ermittelt
  if (currentMillis - strecke_lastMillis >= strecke_interval)
  {
    strecke_lastMillis = currentMillis;
    //Geschwindigkeit in km/h : 3.6 = Geschwindigkeit in m/s
    //Intervall von 1s = Strecke nach 1 s = Geschwindigkeit in m/s
    _strecke = _strecke + (Geschwindigkeit / 3.2 ); // Geschwindigkeit / 3.6 (m/s) * 0.88 (Gemessene Abweichung) == Geschwindigkeit / 3,2
    //Nach 100m werden die Variablen für die Strecke erhöht
    if (_strecke >= 100)
    {
      strecke++ ;
      strecke_ges++;
      _strecke = 0;
      FuenfKmStrecke++;
    }

    //Alle 1 km (10 * 100m) den Zähler für den Letzt-5-km-Schnitt erhöhen und die Hilfsvariablen leeren
    if (strecke % 10 == 1) _sperre = false;
    if (strecke % 10 == 0 && _sperre == false) {
      FuenfKmZaehler++;
      FuenfKmStrecke = 0;
      FuenfKmLiter = 0;
      _sperre = true;
      if (FuenfKmZaehler >= 5) FuenfKmZaehler = 0;
    }
    fahrzeit++;
  }
  //Durchschnittsverbrauch: 100km / Gefahrene Strecke in km * Verbrauchte menge in l
  // 100km = 1.000 (100*m)
  if (strecke > 0 && liter > 0)
  {
    float _hilf = (1000 / (float)strecke);
    float _hilf2 = ((float)liter / 1000000);
    VBtrip = (_hilf * _hilf2);


    


  }
  else
  {
    VBtrip = 0;
  }

  if (FuenfKmStrecke > 0 && FuenfKmLiter > 0) {
    //Auswerten des Letzt-5-km-Schnitts
    float _hilf = (1000 / (float)FuenfKmStrecke);
    float _hilf2 = ((float)FuenfKmLiter / 1000000);
    FuenfKm[FuenfKmZaehler] = (_hilf * _hilf2);

    int _hilf3 = 0;

    for (int i = 0; i < 5; i++) {
      if (FuenfKm[i] != 0) _hilf3++;
      FuenfKmRes = FuenfKm[0] + FuenfKm[1] + FuenfKm[2] + FuenfKm[3] + FuenfKm[4];
    }

    FuenfKmRes = FuenfKmRes / _hilf3;
  }
  //Durchschnittsverbrauch seit manuellem Reset in 10*ml / 100 km
  if (strecke_ges > 0)
  {
    float _hilf = (1000 / (float)strecke_ges);
    float _hilf2 = ((float)liter_ges / 1000000);
    VBges = (_hilf * _hilf2);
    Reichweite = ((100 * Tank_berechnet) / (float)VBges);
  }
  else
  {
    VBges = 0;
    Reichweite = 0;
  }
  //Fahrzeit in s, Strecke in 100m = Durchschnittsgeschwindigkeit
  if (fahrzeit > 0)
  {
    Geschwindigkeit_trip = (360 * ((float)strecke / (float)fahrzeit));
  }
  else Geschwindigkeit_trip = 0;

  //Wenn der Motor läuft, aber kein Verbrauch erkannt wird, liegt offenbar ein Fehler vor und die CAN-Kommunikation startet neu
  if (Drehzahl > 800 &&  VB < 1) {
    //restart();
  }
  //Wenn der Motor über Leerlaufdrehzahl läuft, aber keine Strecke zurückgelegt wird (über einen längeren Zeitraum), liegt auch ein Fehler vor
  if (Drehzahl > 1000 && _strecke < 1 && fahrzeit > 2 && strecke < 1) {
    //restart();
  }
}

void analog_temp()
{
  Oel_Temp = analogRead(Oel_Temp_Pin);
  double Temp2;
  Temp2 = log(10000.0 / (1024.0 / Oel_Temp - 1));
  Temp2 = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp2 * Temp2 )) * Temp2 );
  Oel_Temp = Temp2 - 278;            // Convert Kelvin to Celcius und ziehe einen Offset von 4 ab

  if (Oel_Temp < ( Oel_Temp_alt - 10 )) {
    Oel_Temp = Oel_Temp_alt;
  }

  if (Oel_Temp < 70) Oel_Temp = 0.3 * Oel_Temp + 0.7 * Oel_Temp_alt;
  if (Oel_Temp > 70) {
    if (licht) //Falls das Licht an ist, wird der Temperaturwert des Öls verfälscht. Daher wird die Temperatur angepasst
    {
      Oel_Temp = 0.1 * (0.8 * Oel_Temp) + 0.9 * Oel_Temp_alt;
    }
    else {
      Oel_Temp = 0.1 * Oel_Temp + 0.9 * Oel_Temp_alt;
    }
  }
  Oel_Temp_alt = Oel_Temp;



  Aussen_Temp = analogRead(Aussen_Temp_Pin);
  double Temp;
  Temp = log(10000.0 / (1024.0 / Aussen_Temp - 1));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Aussen_Temp = Temp - 273.15;            // Convert Kelvin to Celcius
}


void zaehler_reset()
{
  liter_ges = 0;
  strecke_ges = 0;
  save_Data();
}




void HebelAuswerten()
{
  int _wert = analogRead(Hebel);

  if (_wert < 620 && _wert > 610) //Stellung '0'
  {
    if (reset_pressed == false) {
      reset_pressed = true;
      reset_start = currentMillis;
    }
    if (currentMillis - reset_start > 3000)
    {
      zaehler_reset();
      reset_pressed = false; //Reset zurücksetzen
    }
    restart_pressed = false;
    next_pressed = false;
  }
  else if (_wert < 345 && _wert > 335) //Stellung '1'
  {
    reset_pressed = false; //Reset wurde losgelassen
    restart_pressed = false;
    next_pressed = false;
  }
  else if (_wert < 10) //Stellung '2'
  {
    reset_pressed = false; //Reset wurde losgelassen
    restart_pressed = false;
    next_pressed = false;
  }
  else if (_wert < 515 && _wert > 505) //Stellung '0'
  {
    if (restart_pressed == false) {
      restart_pressed = true;
      restart_start = currentMillis;
    }
    if (currentMillis - restart_start > 1500)
    {
      restart();
      restart_pressed = false; //Reset zurücksetzen
    }
    reset_pressed = false; //Reset wurde losgelassen
    next_pressed = false;
  }
  else if ((_wert < 890 && _wert > 880) || (_wert < 870 && _wert > 860) || (_wert < 860 && _wert > 845) || (_wert < 880 && _wert > 870)) //Stellung 'MFA'
  {
    next_pressed = true;
  }

  if (next_pressed != next_pressed_old) {
    // Reset Debounce Timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    // Taster Status hat sich geändert?
    if (next_pressed != next_val) {
      next_val = next_pressed;

      // Ist Taster gedrückt, schalte Bildschirm um
      if (next_val == true) {
        seite++;
        if (seite >= 7) seite = 0; //Mit DEBUG und NEW Bildschirm
        //if (seite >= 6) seite = 0; //Mit DEBUG Bildschirm
        //if (seite >= 5) seite = 0; //Ohne DEBUG Bildschirm
      }
    }
  }

  next_pressed_old = next_pressed;
}


void restart()
{
  mySerial.end();

  CAN.begin(CAN_5KBPS);

  analogWrite(LED_Backlight, 0);
  delay(1000);

  //(SW)Serielle-Schnittstelle mit 9600 baud initialisieren (Benötigt vom Motorsteuergerät)
  mySerial.begin(9600);

  analogWrite(LED_Backlight, 200);

  //CAN-Prozessoren (neu)verbinden
RESTART_INIT:

  //VW-Motorcan nutzt 500kB/s
  if (CAN_OK == CAN.begin(CAN_500KBPS))
  {
    delay(150);
  }
  else
  {
    delay(100);
    goto RESTART_INIT;
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
}
