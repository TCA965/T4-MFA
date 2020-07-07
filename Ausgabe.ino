//VBtrip = Durchschnittsverbrauch seit Zündung, durch Faktor 100 dividieren
//strecke = Gefahrene Strecke seit Zündung in m, durch Faktor 100.000 dividieren
//fahrzeit = Zeit seit Zündung an in 1/10s, durch Faktor 600 teilen für Minuten
//Geschwindigkeit_trip = Durchschnittsgeschwindigkeit
//VBkm = Momentanverbrauch in l/100km (oder l/h bei Stillstand)
//Geschwindigkeit = Momentane Geschwindigkeit
//range = Restreichweite
//Oeltemp = Öltemperatur
//Wassertemp1 = Wassertemperatur

void dogm_print()
{
  if (currentMillis - out_lastMillis >= out_interval)
  {
    HebelAuswerten();
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
    }
  }
}

void seite0()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    dogm.setFont(font_5x8);
    dogm.setXY(55, 53);
    dogm.print("MFA");
    dog_SetBitmapP(87, 60, thermo, 8, 8);
    dogm.setXY(100, 53);
    dogm.print(Wassertemp2);
    dogm.print(" C");
    //Trennstrich zu Hauptansicht
    dogm.setSize(0);
    dogm.drawLine(0, 51, 127, 51);


    //Momentanverbrauch
    dogm.setXY(4, 36);
    dogm.setFont(font_6x10);
    dogm.print("Mom.Verbr. ");

    dogm.setFont(font_7x14);
    dogm.print(VBkm);
    dogm.setFont(font_5x8);
    if (Geschwindigkeit > 5) dogm.print(" l/100km");
    else dogm.print(" l/h");

    //Reichweite
    dogm.setXY(4, 21);
    dogm.setFont(font_6x10);
    dogm.print("Reichweite ");

    dogm.setFont(font_7x14);
    dogm.print(range);
    dogm.setFont(font_5x8);
    dogm.print(" km");

    //Tempomat
    dogm.setXY(4, 6);
    dogm.setFont(font_6x10);
    dogm.print("Tempomat ");

    dogm.setFont(font_7x14);
    if (Tempo_geschwindigkeit < 30) dogm.print("Aus");
    else dogm.print(Tempo_geschwindigkeit);
    dogm.setFont(font_5x8);
    if (Tempo_geschwindigkeit > 30) dogm.print(" km/h");

  } while ( dogm.next() );
}

int fahrzeit_h, fahrzeit_min, fahrzeit_mod;
void seite1()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    dogm.setFont(font_5x8);
    dogm.setXY(50, 53);
    dogm.print("Trip 1");
    dog_SetBitmapP(87, 60, thermo, 8, 8);
    dogm.setXY(100, 53);
    dogm.print(Wassertemp2);
    dogm.print(" C");
    //Trennstrich zu Hauptansicht
    dogm.setSize(0);
    dogm.drawLine(0, 51, 127, 51);


    //Durchschnittsgeschwindigkeit
    dogm.setXY(4, 36);
    dogm.setFont(font_6x10);
    dogm.print("Durchschn.");

    dogm.setFont(font_7x14);
    dogm.print(Geschwindigkeit_trip);
    dogm.setFont(font_5x8);
    dogm.print(" km/h");

    //Fahrzeit
    dogm.setXY(4, 21);
    dogm.setFont(font_6x10);
    dogm.print("Fahrzeit ");

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

    //Strecke
    dogm.setXY(4, 6);
    dogm.setFont(font_6x10);
    dogm.print("Strecke ");

    dogm.setFont(font_7x14);
    dogm.print(((float)strecke / 10));
    dogm.setFont(font_5x8);
    dogm.print(" km");

  } while ( dogm.next() );
}

void seite2()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    dogm.setFont(font_5x8);
    dogm.setXY(50, 53);
    dogm.print("Trip 2");
    dog_SetBitmapP(87, 60, thermo, 8, 8);
    dogm.setXY(100, 53);
    dogm.print(Wassertemp2);
    dogm.print(" C");
    //Trennstrich zu Hauptansicht
    dogm.setSize(0);
    dogm.drawLine(0, 51, 127, 51);



    //Verbrauch seit Zündung
    dogm.setXY(4, 30);
    dogm.setFont(font_6x10);
    dogm.print("Verbr.Trip ");

    dogm.setFont(font_7x14);
    dogm.print(VBtrip);
    dogm.setFont(font_5x8);
    dogm.print(" l/100km");

    //Verbrauchte Liter
    dogm.setXY(4, 10);
    dogm.setFont(font_6x10);
    dogm.print("Verbr.Liter ");

    dogm.setFont(font_7x14);
    dogm.print(((float)liter / 1000000));
    dogm.setFont(font_5x8);
    dogm.print(" l");

  } while ( dogm.next() );
}

void seite3()
{
  //5 px Breit, 8 px Hoch
  dogm.start();
  do {
    dogm.setFont(font_5x8);
    dogm.setXY(50, 53);
    dogm.print("Gesamt");
    dog_SetBitmapP(87, 60, thermo, 8, 8);
    dogm.setXY(100, 53);
    dogm.print(Wassertemp2);
    dogm.print(" C");
    //Trennstrich zu Hauptansicht
    dogm.setSize(0);
    dogm.drawLine(0, 51, 127, 51);



    //Verbrauch Gesamt
    dogm.setXY(4, 36);
    dogm.setFont(font_6x10);
    dogm.print("Verbr.Ges. ");

    dogm.setFont(font_7x14);
    dogm.print(VBges);
    dogm.setFont(font_5x8);
    dogm.print(" l/100km");
    
     //Verbrauchte Liter
    dogm.setXY(4, 21);
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
    
     //Verbrauchte Liter
    dogm.setXY(4, 6);
    dogm.setFont(font_6x10);
    dogm.print("Tank");
    dogm.setFont(font_5x8);
    dogm.print(" berechnet");
    dogm.setFont(font_6x10);
    dogm.print(" ");
    
    dogm.setFont(font_7x14);
    dogm.print((int)Tank_berechnet);
    dogm.setFont(font_5x8);
    dogm.print(" l");

  } while ( dogm.next() );
}
