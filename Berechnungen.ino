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
    _strecke = _strecke + ((Geschwindigkeit / 3.6 ) * 1.12);
    //Nach 100m werden die Variablen für die Strecke erhöht
    if (_strecke >= 100)
    {
      strecke++ ;
      strecke_ges++;
      _strecke = 0;
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
  //Durchschnittsverbrauch seit manuellem Reset in 10*ml / 100 km
  if (strecke_ges > 0)
  {
    float _hilf = (1000 / (float)strecke_ges);
    float _hilf2 = ((float)liter_ges / 1000000);
    VBges = (_hilf * _hilf2);
    Reichweite = ((100 * (float)Tank_berechnet) / (float)VBges);
    range = (int)Reichweite;
  }
  else
  {
    VBges = 0;
    Reichweite = 0;
    range = (int)Reichweite;
  }
  //Fahrzeit in s, Strecke in 100m = Durchschnittsgeschwindigkeit 
  if (fahrzeit > 0)
  {
    Geschwindigkeit_trip = (360 * ((float)strecke / (float)fahrzeit));
  }
  else Geschwindigkeit_trip = 0;
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
  }

  else if (_wert < 345 && _wert > 335) //Stellung '1'
  {
    reset_pressed = false; //Reset wurde losgelassen
  }
  else if (_wert < 10) //Stellung '2'
  {
    reset_pressed = false; //Reset wurde losgelassen
  }
  else if (_wert < 515 && _wert > 505) //Stellung '0'
  {
    reset_pressed = false; //Reset wurde losgelassen
  }
  else if ((_wert < 890 && _wert > 880) || (_wert < 870 && _wert > 860) || (_wert < 860 && _wert > 845) || (_wert < 880 && _wert > 870)) //Stellung 'MFA'
  {
    seite++;
    if(seite >= 4) seite = 0;
  }
}
