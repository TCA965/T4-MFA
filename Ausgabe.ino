//VBtrip = Durchschnittsverbrauch seit Zündung, durch Faktor 100 dividieren
//strecke = Gefahrene Strecke seit Zündung in m, durch Faktor 100.000 dividieren
//fahrzeit = Zeit seit Zündung an in 1/10s, durch Faktor 600 teilen für Minuten
//Geschwindigkeit_trip = Durchschnittsgeschwindigkeit
//VBkm = Momentanverbrauch in l/100km (oder l/h bei Stillstand)
//Geschwindigkeit = Momentane Geschwindigkeit
//Reichweite = Restreichweite
//Oeltemp = Öltemperatur
//Wassertemp1 = Wassertemperatur

void dogm_print()
{
  if (currentMillis - out_lastMillis >= out_interval)
  {
    out_lastMillis = currentMillis;
    switch (seite)
    {
      case 0:
        seite0();
        break;
      case 1:
        seite1();
        break;
      case 2:
        seite2();
        break;
      case 3:
        seite3();
        break;
      case 4:
        seite4();
        break;
      case 5:
        seite5();
        break;
    }
    analog_temp();
  }
}

void seite0()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    schreibeOben_Unten("MFA");

    //Verbrauch seit Zündung
    dog_SetBitmapP(20, 43, avg, 8, 8);

    dogm.setXY(35, 35);
    dogm.setFont(font_7x14);
    dogm.print(VBtrip);
    if (VBtrip > 10)
    {
      // l/100 km Symbol
      dog_SetBitmapP(77, 46, ver11, 8, 8);
      dog_SetBitmapP(85, 46, ver12, 8, 8);
      dog_SetBitmapP(87, 46, ver13, 8, 8);
      dog_SetBitmapP(77, 38, ver21, 8, 8);
      dog_SetBitmapP(85, 38, ver22, 8, 8);
      dog_SetBitmapP(87, 38, ver23, 8, 8);
    }
    else
    { // l/100 km Symbol
      dog_SetBitmapP(70, 46, ver11, 8, 8);
      dog_SetBitmapP(78, 46, ver12, 8, 8);
      dog_SetBitmapP(80, 46, ver13, 8, 8);
      dog_SetBitmapP(70, 38, ver21, 8, 8);
      dog_SetBitmapP(78, 38, ver22, 8, 8);
      dog_SetBitmapP(80, 38, ver23, 8, 8);
    }


    //Reichweite
    dog_SetBitmapP(20, 27, tankstelle, 8, 8);

    dogm.setXY(35, 19);
    dogm.setFont(font_7x14);
    dogm.print(Reichweite,0);
    dogm.setFont(font_5x8);
    dogm.print(" km");

  } while ( dogm.next() );
}

int fahrzeit_h, fahrzeit_min, fahrzeit_mod;

void seite1()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    schreibeOben_Unten("Tempomat");
    //Momentanverbrauch
    dogm.setXY(35, 35);
    dogm.setFont(font_7x14);
    dogm.print(VBkm);
    dogm.setFont(font_5x8);
    if (Geschwindigkeit > 5)
    {
      if (VBkm > 10)
      {
        // l/100 km Symbol
        dog_SetBitmapP(77, 46, ver11, 8, 8);
        dog_SetBitmapP(85, 46, ver12, 8, 8);
        dog_SetBitmapP(87, 46, ver13, 8, 8);
        dog_SetBitmapP(77, 38, ver21, 8, 8);
        dog_SetBitmapP(85, 38, ver22, 8, 8);
        dog_SetBitmapP(87, 38, ver23, 8, 8);
      }
      else
      { // l/100 km Symbol
        dog_SetBitmapP(70, 46, ver11, 8, 8);
        dog_SetBitmapP(78, 46, ver12, 8, 8);
        dog_SetBitmapP(80, 46, ver13, 8, 8);
        dog_SetBitmapP(70, 38, ver21, 8, 8);
        dog_SetBitmapP(78, 38, ver22, 8, 8);
        dog_SetBitmapP(80, 38, ver23, 8, 8);
      }
    }
    else
    {
      // l/h Symbol
      dog_SetBitmapP(62, 46, lh1, 8, 8);
      dog_SetBitmapP(62, 38, lh2, 8, 8);
    }
    //Tempomat
    //Tempomat Symbol
    if (tempo_aktiv == true) dog_SetBitmapP(15, 31, tempo11, 8, 8);
    else dog_SetBitmapP(15, 31, tempo_11, 8, 8);
    dog_SetBitmapP(23, 31, tempo12, 8, 8);
    dog_SetBitmapP(15, 23, tempo21, 8, 8);
    dog_SetBitmapP(23, 23, tempo22, 8, 8);

    dogm.setXY(35, 19);
    dogm.setFont(font_7x14);
    if (Tempo_geschwindigkeit < 25) dogm.print("Aus");
    else dogm.print(Tempo_geschwindigkeit);
    dogm.setFont(font_5x8);
    if (Tempo_geschwindigkeit > 25) dogm.print(" km/h");



  } while ( dogm.next() );
}

void seite2()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    schreibeOben_Unten("Trip 1");

    //Verbrauchte Liter
    dogm.setXY(4, 35);
    dogm.setFont(font_6x10);
    dogm.print("Verbraucht ");

    dogm.setFont(font_7x14);
    dogm.print(((float)liter / 1000000));
    dogm.setFont(font_5x8);
    dogm.print(" l");

    //Strecke
    dogm.setXY(4, 19);
    dogm.setFont(font_6x10);
    dogm.print("Strecke ");

    dogm.setFont(font_7x14);
    dogm.print(((float)strecke / 10));
    dogm.setFont(font_5x8);
    dogm.print(" km");

  } while ( dogm.next() );
}

void seite3()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    schreibeOben_Unten("Trip 2");
    //Durchschnittsgeschwindigkeit
    dog_SetBitmapP(20, 43, avg, 8, 8);

    dogm.setXY(35, 35);
    dogm.setFont(font_7x14);
    dogm.print(Geschwindigkeit_trip,0);
    dogm.setFont(font_5x8);
    dogm.print(" km/h");

    //Fahrzeit
    dogm.setXY(4, 19);
    dogm.setFont(font_6x10);
    dogm.print("Zeit ");

    dogm.setFont(font_7x14);

    fahrzeit_h = fahrzeit / 3600;
    fahrzeit_mod = fahrzeit % 3600;
    fahrzeit_min = fahrzeit_mod / 60;

    if (fahrzeit_h < 1)
    {
      dogm.print(fahrzeit_min);
      dogm.setFont(font_5x8);
      dogm.print(" min");
    }
    else
    {
      dogm.print(fahrzeit_h);
      dogm.print(":");
      dogm.print(fahrzeit_min);
      dogm.setFont(font_5x8);
      dogm.print(" h");
    }


  } while ( dogm.next() );
}

void seite4()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    schreibeOben_Unten("Bilanz");

    //Verbrauch Gesamt
    dog_SetBitmapP(5, 43, avg, 8, 8);
    dogm.setXY(14, 35);
    dogm.setFont(font_5x8);
    dogm.print("Gesamt");

    dogm.setXY(50, 35);
    dogm.setFont(font_7x14);
    dogm.print(VBges);

   if(VBges > 10) 
    { 
    // l/100 km Symbol
    dog_SetBitmapP(87, 46, ver11, 8, 8);
    dog_SetBitmapP(95, 46, ver12, 8, 8);
    dog_SetBitmapP(97, 46, ver13, 8, 8);
    dog_SetBitmapP(87, 38, ver21, 8, 8);
    dog_SetBitmapP(95, 38, ver22, 8, 8);
    dog_SetBitmapP(97, 38, ver23, 8, 8);
    }
    else
    {// l/100 km Symbol
    dog_SetBitmapP(80, 46, ver11, 8, 8);
    dog_SetBitmapP(88, 46, ver12, 8, 8);
    dog_SetBitmapP(90, 46, ver13, 8, 8);
    dog_SetBitmapP(80, 38, ver21, 8, 8);
    dog_SetBitmapP(88, 38, ver22, 8, 8);
    dog_SetBitmapP(90, 38, ver23, 8, 8);
    }

    //Tankinhalt
    /* dogm.setXY(4, 19);
     dogm.setFont(font_6x10);
     dogm.print("Tank");
     dogm.setFont(font_5x8);
     dogm.print(" gemessen");
     dogm.setFont(font_6x10);
     dogm.print(" ");

     dogm.setFont(font_7x14);
     dogm.print(Tank);
     dogm.setFont(font_5x8);
     dogm.print(" l");
    */

    //Tankinhalt
    dogm.setXY(4, 19);
    dogm.setFont(font_6x10);
    dogm.print("Tank");
    dogm.setXY(50, 19);
    dogm.setFont(font_7x14);
    dogm.print(Tank_berechnet,1);
    dogm.setFont(font_5x8);
    dogm.print(" l");

  } while ( dogm.next() );
}

void seite5()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    schreibeOben_Unten("Debug");

    //Gesamte Strecke
    dogm.setXY(4, 35);
    dogm.setFont(font_6x10);
    dogm.print("Strecke");
    dogm.setXY(50, 35);
    dogm.setFont(font_7x14);
    dogm.print((strecke_ges/10));
    dogm.setFont(font_5x8);
    dogm.print(" km");
    
    //Verbrauchte Liter
    dogm.setXY(4, 19);
    dogm.setFont(font_6x10);
    dogm.print("Verbraucht ");
    dogm.setFont(font_7x14);
    dogm.print(((float)liter_ges / 1000000));
    dogm.setFont(font_5x8);
    dogm.print(" l");
    

  } while ( dogm.next() );
}


void schreibeOben_Unten(String Anzeige)
{
  //Oberer Bereich
  dogm.setFont(font_5x8);
  dogm.setXY(7, 53);
  dogm.print(Anzeige);
  dog_SetBitmapP(89, 60, thermo, 8, 8);
  dogm.setXY(100, 53);
  dogm.print(Aussen_Temp,0);
  dog_SetBitmapP(105, 59, grad, 8, 8);
  dogm.setXY(114, 53);
  dogm.print("C");
  //Trennstrich zu Hauptansicht
  dogm.setSize(0);
  dogm.drawLine(0, 51, 127, 51);


  //Unterer Bereich
  //Trennstrich zu Hauptansicht
  dogm.setSize(0);
  dogm.drawLine(0, 13, 127, 13);

  dogm.setFont(font_5x8);
  dog_SetBitmapP(5, 10, wa_thermo, 8, 8);
  dogm.setXY(16, 3);
  dogm.print(Wassertemp2);
  dog_SetBitmapP(21, 9, grad, 8, 8);
  dogm.setXY(30, 3);
  dogm.print("C");

  dog_SetBitmapP(85, 9, oil1, 8, 8);
  dog_SetBitmapP(93, 9, oil2, 8, 8);
  dogm.setXY(100, 3);
  dogm.print(Oel_Temp,0);
  if (Oel_Temp < 100) {
    dog_SetBitmapP(105, 9, grad, 8, 8);
    dogm.setXY(114, 3);
  }
  else {
    dog_SetBitmapP(110, 9, grad, 8, 8);
    dogm.setXY(119, 3);
  }
  dogm.print("C");
}
