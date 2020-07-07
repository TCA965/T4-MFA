void CAN_rx()
{
  //Es wird geprüft, ob neue Nachrichten anliegen
  if (CAN_MSGAVAIL == CAN.checkReceive())
  {
    //Falls neue Nachrichten anliegen, wird die Länge und die Nachricht eingelesen
    //len: Länge der Daten, buf: Nachrichten-Bytes, canID: Sendeadresse der Daten
    CAN.readMsgBuf(&len, buf);
    canID = CAN.getCanId();

    //Auswertung der Daten
    switch (canID)
    {
      //Nachricht kommt von Motor 1
      case 0x280:
        {
          Byte_0x280_0 = buf[0];
          Byte_0x280_3 = buf[3];
          Byte_0x280_4 = buf[4];
          Byte_0x280_5 = buf[5];
          break;
        }
      //Nachricht kommt von Motor 2
      case 0x288:
        {
          Byte_0x288_1 = buf[1];
          Byte_0x288_2 = buf[2];
          Byte_0x288_4 = buf[4];
          break;
        }
      //Nachricht kommt von KI 1
      case 0x320:
        {
          Byte_0x320_0 = buf[0];
          Byte_0x320_1 = buf[1];
          Byte_0x320_2 = buf[2];
          Byte_0x320_4 = buf[4];
          break;
        }
      //Nachricht kommt von Motor 3
      case 0x388:
        {
          Byte_0x388_1 = buf[1];
          break;
        }
      //Nachricht kommt von KI 2
      case 0x420:
        {
          Byte_0x420_3 = buf[3];
          Byte_0x420_4 = buf[4];
          Byte_0x420_5 = buf[5];
          break;
        }
      //Nachricht kommt von Motor 4
      case 0x480:
        {
          Byte_0x480_2 = buf[2];
          Byte_0x480_3 = buf[3];
          break;
        }
      default: break;
    }
  }
}

void CAN_calc()
{
  //Berechnung der Motor 1 Daten
  //Kupplung
  if (Byte_0x280_0 == 97)
  { 
     kupplung = true;
     tempo_aktiv = false;
   }
  else if (Byte_0x280_0 == 105) 
  {
    kupplung = false;
  }

  //Drehzahl
  Drehzahl = (Byte_0x280_4 + 256 * Byte_0x280_3) / 4;

  //Motor war an
  if (Drehzahl > 1000) speichern = true;
  //War der Motor an und wird wieder ausgeschaltet, werden die Gesamt Liter&Strecken-Werte gespeichert. Um den EEPROM zu schonen, wird erst nach dem nächsten Motorstart wieder gespeichert.
  if (Drehzahl < 800 && speichern == true)
  {
    save_Data();
    speichern = false;
  }

  //Gaspedal
  Gaspedal = Byte_0x280_5 / 2.5;

  //Berechnung der Motor 2 Daten
  //Wassertemperatur 2
  Wassertemp2 = (Byte_0x288_1 * 0.75) - 48;
  if (Wassertemp2 < 0) Wassertemp2 = 0;

  //Bremspedal und Tempomat aktiv/inaktiv
  if (Byte_0x288_2 == 16)
  {
    bremse = false;
  }
  else if (Byte_0x288_2 == 19)
  {
    bremse = true;
    tempo_aktiv = false;
  }
  else if (Byte_0x288_2 == 80)
  {
    //Geschwindigkeit Tempomat
    Tempo_geschwindigkeit = Byte_0x288_4 * 1.34;
    tempo_aktiv = true;
  }

  //Berechnung der KI 1 Daten
  //Fahrertür auf/zu
  if (Byte_0x320_0 == 1) fahrer = true;
  else if (Byte_0x320_0 == 0) fahrer = false;

  //Handbremse
  if (Byte_0x320_1 == 2) handbremse = true;
  else if (Byte_0x320_1 == 0) handbremse = false;

  //Tankinhalt
  Tank = Byte_0x320_2;
  if (Tank > 130) Tank -= 130;

  //Geschwindigkeit
  Geschwindigkeit = Byte_0x320_4 * 1.34;



  //Berechnung der Motor 3 Daten
  //Tempomat
  if (Byte_0x388_1 == 1)
  {
    //Tempomat ist eingeschaltet
    tempo_an == true;
    tempo_set == false;
    tempo_reset == false;
  }
  else if (Byte_0x388_1 == 2)
  {
    //Tempomat ist ausgeschaltet
    tempo_an == false;
    tempo_set == false;
    tempo_reset == false;
  }
  else if (Byte_0x388_1 == 21)
  {
    //Tempomat SET wird gedrückt
    tempo_set == true;
    tempo_reset == false;
    //Tempo_geschwindigkeit = Geschwindigkeit;
  }
  else if (Byte_0x388_1 == 41)
  {
    //Nur wenn RESET gehalten wird und der Tempomat aktiv ist, wird die Geschwindigkeit aktualisiert
    //Sonst: V = 30 km/h V_tempo = 60 km/h Druck auf Reset -> V_tempo = 30
    //if(tempo_reset == true && tempo_aktiv == true) Tempo_geschwindigkeit = Geschwindigkeit;
    // Tempo_geschwindigkeit = Geschwindigkeit;
    //Tempomat RESET wird gedrückt
    tempo_set == false;
    tempo_reset == true;
  }

  //Berechnung der KI 2 Daten
  //Öltemperatur
  Oeltemp = Byte_0x420_3 - 60;
  if (Oeltemp < 0) Oeltemp = 0;

  //Wassertemperatur1
  Wassertemp1 = (Byte_0x420_4 - 64) * 0.75;
  if (Wassertemp1 < 0) Wassertemp1 = 0;

  //Licht
  if (Byte_0x420_5 == 0)
  {
    licht = false;
    analogWrite(LED_Backlight, 200);
  }
  else if (Byte_0x420_5 != 0)
  {
    licht == true;
    analogWrite(LED_Backlight, 70);
  }

  //Berechnung der Motor 3 Daten
  //Verbrauch in µl seit Zündung
  //B01111111 = 127
  CurrentVB = Byte_0x480_2 + (Byte_0x480_3 & B01111111) * 256;
}

void CAN_VBcalc()
{
  //Halbsekündliche Berechnung des Verbrauchs in µl
  if (currentMillis - VB_lastMillis >= VB_interval)
  {
    VB_lastMillis = currentMillis;
    //Wenn der aktuelle Wert kleiner als der alte ist, hat ein Überlauf stattgefunden
    if (CurrentVB < LastVB) {
      //Sollte ein Überlauf stattgefunden haben, wird einfach 0x8000 = 32768 dazu addiert (bei diesem Wert, findet der Überlauf statt)
      VB = CurrentVB + 0x8000 - LastVB;
    } else {
      //Fand kein ÜB statt, wird einfach die Differenz gebildet
      VB = CurrentVB - LastVB;
    }
    //Korrekturfaktor: Verbrauch wird circa 7% zu gering angezeigt, daher wird VB mit 1.07 multipliziert
    VB = VB * 1.08;
    //VB entspricht dem Verbrauch in µl pro halber Sekunde, multipliziert man diesen Wert mit 0.0072 kommt man auf l/h
    //Die Variable "liter" enthält die insgesamt verbrauchte Diesel-Menge seit Start (in µL)
    liter = liter + VB ;
    FuenfKmLiter = FuenfKmLiter + VB;
    liter_ges = liter_ges + VB;
    VBh = VB * 0.0072;
    Tank_berechnet = 80.00 - ((float)liter_ges / 1000000);
    LastVB = CurrentVB;
  }
}

void save_Data() {
  //Insgesamt Verbrauchte Mikroliter seit Reset, startend an EEPROM-Adresse 100
  EEPROMWritelong(100, liter_ges);
  //Insgesamt gefahrene Meter seit Reset, startend an EEPROM-Adresse 200
  EEPROMWritelong(200, strecke_ges);
  //Aktuelle MFA Seite
  EEPROMWritelong(300, seite);
}

