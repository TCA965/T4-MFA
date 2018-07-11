# T4-MFA

"Projekt-MFA_update_neues_design.ino" ist das Hauptprogramm, die anderen Dateien sind "Tabs" in der Arduino IDE.
Es enthält die meisten Variablen und Pin-Definitionen und enthält die "setup" und die "loop" Funktion.

"Ausgabe" sorgt, wie der Name schon sagt, für die Ausgabe. Hier werden die umschaltbaren Seiten erstellt.

Im "CAN"-Tab findet die Kommunikation mit den MCPs statt. Zuerst werden die Rohdaten eingelesen und in Variablen der Form "Byte_ADRESSE_IN_HEX_ID" gespeichert.
Diese werden daraufhin in "sinnvolle" Daten gewandelt.
Desweiteren werden einige Werte im EEPROM abgelegt. Dazu zählen Strecke seit manuellem Reset, Verbrauchte Dieselmenge seit man. Reset und die aktuell betrachtete Seite des MFAs.

In "Berechnungen" werden Verbräuche und Co berechnet und der MFA-Hebel abgefragt.

"K_Linie.ino" ist das Subprogramm mit dem Fehler aus der ECU ausgelesen werden sollen. Bisher nicht getestet, kann fehlerhaft sein.

In "symbols.h" werden benutzerdefinierte Symbole erzeugt. (Tempomat, Tanksymbol, Thermometer...)

"EEPROMAnything.h" ist eine Erweiterung der Arduino EEPROM.write() Funktionalität. (EEPROM.write kann nur Byte Datentypen speichern, mit der Routine können auch long-Variablen gespeichert werden.
