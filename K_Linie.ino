int blockCounter;

//Sobald die Zündung eingeschaltet wird, soll die ECU nach Fehlern gefragt werden
//1. Sende mit 5 baud 0x01 an die ECU um die Kommunikation zu aktivieren
//2. Initialisiere Kommunikation
//3. ECU schickt ihre Daten (HW&SW Vers.)
//4. ACK an ECU
//5. Anfrage Fehlercodes an ECU
//6. Antwort ECU
//7. ACK an ECU
//8a. Falls kein Fehler vorliegt, Kommunikation beenden
//8b. Fehler anzeigen, wenn vorhanden, Kommunikation mit ACKs aufrecht erhalten, bis Nutzer entschieden hat (löschen/nicht löschen --> 9a.)
//9. Fehler löschen an ECU
//10. ACK von ECU
//11. ACK an ECU --> 9a.

//Performancetechnisch nicht ideal, das Hauptprogramm wird erst ausgeführt, wenn hier alles beendet ist.


//1. Initialisierung
void ECU_init()
{
  digitalWrite(3, HIGH);
  delay(2000);
  digitalWrite(3, LOW);
  delay(200);
  digitalWrite(3, HIGH);
  delay(200);
  digitalWrite(3, LOW);
  delay(1400);
  digitalWrite(3, HIGH);
  ECU_anmeldung();
}

//2. Parameteraushandlung
void ECU_anmeldung() {
  int _step = 0;
  byte in;
  while (_step != 3) {
    if (obd.available()) {
      in = obd.read();
      //Initialisierung: 0x55 Sync Byte zum festlegen der Baudrate
      if (in == 0x55 && _step == 0) _step = 1;
      //LSB
      else if (in == 0x01 && _step == 1) _step = 2;
      //HSB
      else if (in == 0x8A && _step == 2) {
        _step = 3;
        //Ist alles korrekt Komplement zu 0x8A senden (0x75)
        obd.write((0xff - in));
        //Puffer leeren, Software Serial ist nur halbduplex
        obd.flush();
        ECU_data();
      }
    }
  }
}

//3. ECU Daten Empfangen und bestätigen
void ECU_data() {
  delay(5);
  byte in;
  int _step = 0;
  int counter = 0;
  int blockLength;
  while (_step != 5) {
    if (obd.available()) {
      in = obd.read();

      //Nachdem im Schritt 2 das Komplement als Bestätigung gesendet wurde, schickt die ECU als erstes die Block-Länge der nachfolgenden Daten
      if (_step == 0) {
        blockLength = in;
        //Der counter zählt die gesendeten Nachrichten, sobald counter = Blocklänge, ist dieser Schritt beendet
        counter++;
        //Der Step-counter ist eine Hilsvariable
        _step++;
        delay(5);
        //Bestätigung der Block-Länge durch das Komplement
        obd.write((0xff - in));
        obd.flush();
      }
      //Als nächstes Schickt die ECU den Block-Counter, dies ist eine Variable, die mit jeder Aktion hochgezählt wird (also jedes ACK und alle Daten)
      else if (_step == 1) {
        blockCounter = in;
        counter++;
        _step++;
        delay(5);
        obd.write((0xff - in));
        obd.flush();
        //Siehe oben, counter und step-counter werden erhöht, Komplement als Bestätigung
      }
      //* Dieser Schritt steht hier um Platz zu sparen. Am Ende dieses Abschnitts, schickt der µC ein ACK zur ECU. Die Antwort auf das ACK wird hier behandelt. Danach wird die Schleife verlassen*
      else if (_step == 2 && in == 0x09) {
        counter++;
        //Verlasse die Schleife
        _step = 5;
        delay(5);
        obd.write((0xff - in));
        obd.flush();
      }
      //Die nächste Aktion der ECU ist die Mitteilung um welche Art der Daten es sich handelt. Dies wird im Block Title mitgeteilt. Hier sind es ASCII-Daten (0xf6)
      else if (_step == 2 && in == 0xf6) {
        counter++;
        _step++;
        delay(5);
        obd.write((0xff - in));
        obd.flush();
      }
      //Ab hier kommen Daten. Da ich die Daten ignoriert habe, wird auf das Ende der Daten gewartet. Das Ende wird duch counter = Block-Länge erkannt.
      else if (_step == 3) {
        counter++;
        delay(5);
        //Alle Daten mit ihrem Komplement bestätigen
        obd.write((0xff - in));
        obd.flush();
        //Ende erreicht
        if (counter == blockLength) {
          //Ende der Daten erreich, step-counter wird erhöht
          _step++;
        }

      }
      //Die ECU signalisiert des Ende dieses Teils mit 0x03. Die ECU erwartet hier ein ACK als Bestätigung für den gesamten Block
      else if (_step == 4 && in == 0x03) {
        //Step wird auf 5 erhöht, damit wird die while Schleife beendet
        _step++;
        //Doch vorher wird ein ACK an die ECU geschickt
        ECU_ack();
      }
    }
  }
  //Sobald die Ausführung des Programms hier angekommen ist, wird mit Schritt 5., der Anfrage der Fehlercodes begonnen
  ECU_fehler();
}


void ECU_ack() {
  byte in;
  //µC teilt mit 0x03 sein Sende-Vorhaben an
  obd.write(0x03);
  obd.flush();
  int _step = 0;
  while (_step != 3) {
    while (obd.available()) {
      in = obd.read();
      //Die ECU muss auch mit Komplementen alle Schritte bestätigen. Hier wird das Komplement zu 0x03 erwartet
      if (in == (0xFF - 0x03) && _step == 0) {
        //Prüfen ob Block-Counter übergelaufen ist. Wenn ja, bei 0 beginnen
        if (blockCounter == 0xff) blockCounter = 0x00;
        blockCounter++;
        _step++;
        delay(5);
        //Aktuellen Block-Counter an ECU senden
        obd.write(blockCounter);
        obd.flush();
      }
      //Warte auf Komplement zu Block-Counter
      else if (in == (0xFF - blockCounter) && _step == 1) {
        _step++;
        delay(5);
        //Sende ACK Befehel (0x09)
        obd.write(0x09);
        obd.flush();
      }
      //Erwarte Komplement zu 0x09
      else if (in == (0xFF - 0x09) && _step == 2) {
        _step++;
        delay(5);
        //Signalisiere Ende des Befehls
        obd.write(0x03);
        obd.flush();
        //Die ECU wird wiederrum einen ACK Block schicken. Obiger Programmteil verarbeitet auch diesen. Siehe *
        ECU_data();
      }
    }
  }
}

void ECU_fehler()
{


}
