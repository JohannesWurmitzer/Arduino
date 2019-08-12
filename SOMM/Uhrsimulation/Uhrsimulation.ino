#pragma region Bausteinkopf
/*
 Helferlein.ino
 Datenerfassung des Bogens mit Kommunikation zum übergeordneten System
*/
#pragma region Versionierung
/*
 Versionsgeschichte
 19.03.2019		V 1		Markus Emanuel Wurmitzer
	- Erstellung (Kommunikation der SMC simulieren)
*/
#pragma endregion
#pragma endregion

#pragma region Eingebundenes
#pragma endregion

#pragma region Ein- und Ausgänge
const int DA_LED = 13;                              // Diagnose-LED, soll während des Betriebes blinken
#pragma endregion

#pragma region Zyklen
const unsigned long ZYK_INT = 1;					// Hauptintervall der Zykluszeit [ms]
const unsigned long ZYK_INT10 = 10;					// Hauptintervall 10-fach verlangsamt [ms]
const unsigned long ZYK_SEK = 1000 / ZYK_INT;       // Zyklen pro Sekunde
unsigned long gulZykSt;                             // Startzeit des aktuellen Zyklus [ms]
unsigned long gulZykStN;                            // Startzeit des nächsten Zyklus [ms]
bool gboZykUL;                                      // Zykluszeit Überlauf erkannt
bool gboLED;                                        // Merker der Diagnose-LED
#pragma endregion

#pragma region Kommunikation
char gbyKom[256];									// Kommunikation Eingang, maximal X Byte, davon gibt es ein Start und Endezeichen!
int giKomIdx;										// Kommunikation Index im Eingangsfeld
#pragma endregion

#pragma region Parameter
char gfchVNr[4] = "100";							// Versionsnummer
char gfchSNr[11] = "00001";							// Seriennummer
char gfchDat[11] = "dd.MM.yyyy";					// internes Datum (nur simuliert)
int giStd = 0;										// interne Uhrzeit Stunde
int giMin = 0;										// interne Uhrzeit Minute
int giSek = 0;										// interne Uhrzeit Sekunde
#pragma endregion

#pragma region Programe
byte gfbyJAP[53];									// Jahresprogramm
byte gfbyWOP[53][7];								// Wochenprogramme
byte gfbySZP[25][8];								// Schaltzeitprogramme
#pragma endregion	

// Initialisierung nach dem Neustart, bzw. Rücksetzen des Systems
void setup() {
	// Ein- und Ausgänge initialisieren
	pinMode(DA_LED, OUTPUT);

	// serielle Kommunikation initialisieren
	Serial.begin(115200);

	// Zykluszeiten initialisieren
	gulZykStN = millis() + ZYK_INT;
}

// Hauptschleife
void loop() {
	// Startzeit ermitteln
	gulZykSt = millis();

	// Überlaufsmerker zurücksetzen
	if (gboZykUL && (gulZykSt <= gulZykStN))
	{
		gboZykUL = false;
	}

	// Hauptzykluszeit = X ms
	if (gulZykSt >= gulZykStN)
	{
		// Startzeit des aktuellen Zyklus speichern
		gulZykStN += ZYK_INT;

		// Überlauf detektieren
		if (gulZykStN < gulZykSt)
		{
			gboZykUL = true;
		}

		/*
		Zyklen aufrufen
		*/
		HauptZyklus();			// Hautpzyklus  
		Int10Zyklus();			// 10-fach verlangsamter Takt   
		SekZyklus();			// Sekundentakt
	}
}

// Hauptzyklus
void HauptZyklus()
{
}

// 10-fach verlangsamter Zyklus
void Int10Zyklus()
{
	static unsigned int uiInt10Z = 0;

	// 10-fach verlangsamten Zyklus
	uiInt10Z++;

	// Zyklus ausführen
	if (uiInt10Z > ZYK_INT10)
	{
		// Diagnosenachricht
		// Serial.println(gulZykSt);

		// Zähler zurücksetzen
		uiInt10Z = 1;

		// Hilfsvariable - Kommunikationseingang erkannt
		bool boKomEin = false;

		// auf Dateneingang warten
		while ((Serial.available() > 0) && !boKomEin)
		{
			// Eingangsbyte lesen
			gbyKom[giKomIdx] = Serial.read();

			// Protokollanfang prüfen
			if (gbyKom[giKomIdx] == '#')
			{
				// neuer Protokollanfang erkannt, alte Daten verwerfen
				gbyKom[0] = '#';
				giKomIdx = 1;
			}
			else if ((giKomIdx != 0) || (gbyKom[0] == '#'))
			{
				// Protokollende abfangen
				if (gbyKom[giKomIdx] == 13)
				{
					boKomEin = true;
				}

				// Index erhöhen
				giKomIdx++;

				// ungültige Daten erhalten, Inhalt löschen
				if ((giKomIdx >= 256) && !boKomEin)
				{
					giKomIdx = 0;
				}
			}
		}

		// Debugnachricht
		/*
		if (giKomIdx > 0)
		{
			Serial.print("Debug Eingangspuffer: ");
			Serial.println(gbyKom);
			Serial.flush();
		}
		*/
		

		// Protokoll auswerten
		if (boKomEin)
		{
			// Hilfsvariablen für die Zwischenspeicherung der Daten
			int iID = 0;
			int iID2 = 0;
			int iID3 = 0;
			char fchH[201];

			// Protokollvariablen
			int iPrN = 0;				// Protokollnummer
			int iPrL = 0;				// Protokolllänge
			char fchTeB[5] = "";		// Telegrammbefehl
			char fchTeZ[2] = "";		// Telegrammzugriff
			int iTeA = -1;				// Telegrammadresse
			char fchTeD[201] = "";		// Telegrammdaten
			int iPrP = -1;				// Protokollprüfsumme

			// Felder auswerten (Start- und Endezeichen ignorieren)
			for (int i = 1; i < giKomIdx - 1; i++)
			{
				// Protokoll in die einzelnen Teile aufspalten
				if (gbyKom[i] != ',')
				{
					// Zwischenspeicher der Protokollteile
					fchH[iID++] = gbyKom[i];
				}
				else
				{
					// Zeichenkette abschließen
					fchH[iID] = 0;

					// Spalte detektiert
					if (iPrN == 0)
					{
						// Protokollnummer auslesen
						iPrN = atoi(fchH);
					}
					else if (iPrL == 0)
					{
						// Protokolllänge auslesen
						iPrL = atoi(fchH);
					}
					else if (strlen(fchTeB) == 0)
					{
						// Telegrammbefehl auslesen
						strcpy(fchTeB, fchH);
					}
					else if (strlen(fchTeZ) == 0)
					{
						// Telegrammzugriffsart auslesen
						strcpy(fchTeZ, fchH);
					}
					else if (iTeA == -1)
					{
						// Telegrammadresse auslesen
						iTeA = atoi(fchH);
					}
					else
					{
						// neue Daten anhängen
						strcpy(fchTeD, fchH);
					}

					// Index rücksetzen
					iID = 0;
				}
			}
			// Zeichenkette abschließen
			fchH[iID] = 0;
			// der letzte Inhalt sollte immer die Prüfsumme sein
			iPrP = atoi(fchH);

			// Hilfszeichenkette Telegramm
			sprintf(fchH, "%s,%s,%d", fchTeB, fchTeZ, iTeA);
			// Telegrammdaten nur bei Vorhandensein anhängen
			if (strlen(fchTeD) > 0)
			{
				strcat(fchH, ",");
				strcat(fchH, fchTeD);
			}

			// Prüfsumme ermitteln
			int iPS = Pruefsumme(fchH);

			// Debugausgabe
			/*
			sprintf(gbyKom, "Debug Eingang: PrN: %d PrL: %d TeB: %s TeZ: %s TeA: %d TeD: %s PrP: %d\r", iPrN, iPrL, fchTeB, fchTeZ, iTeA, fchTeD, iPrP);
			Serial.print(gbyKom);
			Serial.flush();
			*/
				
			// Überprüfung
			if (iPrP != iPS)
			{
				// Prüfsummenfehler entdeckt (Prüfsumme erhalten / Prüfsumme berechnet)
				sprintf(fchTeD, "FE,PS,%d,%d", iPrP, iPS);
			}
			else if (iPrL != strlen(fchH))
			{
				// Telegrammlänge fehlerhaft
				sprintf(fchTeD, "FE,LA,%d", iPrL);
			}
			else
			{
				// ungültiger Zugriff
				bool boFeZu = true;

				// Befehle abfragen
				if (strcmp(fchTeB, "VER") == 0)
				{
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Versionsnummer abfragen
						strcpy(fchTeD, gfchVNr);
						// Zugriffsfehler löschen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "SER") == 0)
				{
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Seriennummer abfragen
						strcpy(fchTeD, gfchSNr);
						// Zugriffsfehler löschen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Seriennummer schreiben
						if (strlen(fchTeD) < sizeof(gfchSNr))
						{
							// Länge passt
							strcpy(gfchSNr, fchTeD);
						}
						else
						{
							// ungültige Länge
							strcpy(fchTeD, "FE,SN");
						}
						// Zugriffsfehler löschen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "DAT") == 0)
				{
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Datum und Uhrzeit lesen
						sprintf(fchTeD, "%s\t%02d:%02d:%02d", gfchDat, giStd, giMin, giSek);
						// Zugriffsfehler löschen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Datum und Uhrzeit schreiben
						sscanf(fchTeD, "%s\t%2d:%2d:%2d", gfchDat, &giStd, &giMin, &giSek);
						// Zugriffsfehler löschen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "JAP") == 0)
				{
					// Jahresprogramm lesen / schreiben
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Daten hinzufügen
						for (int i = 0; i < sizeof(gfbyJAP); i++)
						{
							if (i > 0)
							{
								// Einträge mit Leerzeichen einfügen
								sprintf(fchH, "\t%d", gfbyJAP[i]);
							}
							else
							{
								// erster Eintrag, ohne Leerzeichen
								sprintf(fchH, "%d", gfbyJAP[i]);
							}

							// Daten anhängen
							strcat(fchTeD, fchH);
						}
						// Zugriffsfehler löschen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Daten speichern
						iID = 0;
						iID2 = 0;

						// Telegrammdaten durchgehen
						for (int i = 0; i < strlen(fchTeD); i++)
						{
							if (fchTeD[i] == '\t')
							{
								// neuer Wert
								fchH[iID] = 0;
								gfbyJAP[iID2++] = atoi(fchH);
								iID = 0;
							}
							else
							{
								// Wert zusammenbauen
								fchH[iID++] = fchTeD[i];
							}
						}
						// letzter Wert
						fchH[iID] = 0;
						gfbyJAP[iID2] = atoi(fchH);

						// Zugriffsfehler löschen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "WOP") == 0)
				{
					// Wochenprogramme lesen / schreiben
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Adresse prüfen
						if (iTeA < 53)
						{
							// Daten hinzufügen
							for (int i = iTeA; i < iTeA + 1; i++)
							{
								// nur gültige Werte einfügen
								if (i < 53)
								{
									if (i > iTeA)
									{
										// Einträge mit Trennzeichen einfügen
										sprintf(fchH, ";%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d", "test", gfbyWOP[i][0], gfbyWOP[i][1], gfbyWOP[i][2], gfbyWOP[i][3], gfbyWOP[i][4], gfbyWOP[i][5], gfbyWOP[i][6]);
									}
									else
									{
										// erster Eintrag, ohne Trennzeichen
										sprintf(fchH, "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d", "test", gfbyWOP[i][0], gfbyWOP[i][1], gfbyWOP[i][2], gfbyWOP[i][3], gfbyWOP[i][4], gfbyWOP[i][5], gfbyWOP[i][6]);
									}

									// Daten anhängen
									strcat(fchTeD, fchH);
								}
							}
						}
						else
						{
							// Adresse ungültig
							strcpy(fchTeD, "FE,AD");
						}

						// Zugriffsfehler löschen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Adresse prüfen
						if (iTeA < 53)
						{
							// Daten speichern
							iID = 0;
							iID2 = 0;
							iID3 = iTeA;

							// Telegrammdaten durchgehen
							for (int i = 0; i < strlen(fchTeD); i++)
							{
								if ((fchTeD[i] == '\t') || (fchTeD[i] == ';'))
								{
									// neuer Wert
									if (iID2++ == 0)
									{
										// Namen der Wochenprogramme überspringen
									}
									else
									{
										// Wochenprogramm Tagesliste
										fchH[iID] = 0;
										gfbyWOP[iID3][iID2++] = atoi(fchH);
										iID = 0;
									}
									// Adresswechsel
									if (fchTeD[i] == ';')
									{
										iID3++;
										iID2 = 0;
									}
								}
								else
								{
									// Wert zusammenbauen
									fchH[iID++] = fchTeD[i];
								}
							}
							// letzter Wert
							fchH[iID] = 0;
							gfbyWOP[iID3][iID2] = atoi(fchH);
						}
						else
						{
							// Adresse ungültig
							strcpy(fchTeD, "FE,AD");
						}

						// Zugriffsfehler löschen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "SZP") == 0)
				{
					// Schaltzeitprogramme lesen / schreiben
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Adresse prüfen
						if (iTeA < 25)
						{
							// Daten hinzufügen
							for (int i = iTeA; i < iTeA + 8; i++)
							{
								// nur gültige Werte einfügen
								if (i < 25)
								{
									if (i > iTeA)
									{
										// Einträge mit Trennzeichen einfügen
										sprintf(fchH, ";%d\t%02d:%02d:%02d\t%02d:%02d:%02d\t%d", gfbySZP[i][0], gfbySZP[i][1], gfbySZP[i][2], gfbySZP[i][3], gfbySZP[i][4], gfbySZP[i][5], gfbySZP[i][6], gfbySZP[i][7]);
									}
									else
									{
										// erster Eintrag, ohne Trennzeichen
										sprintf(fchH, "%d\t%02d:%02d:%02d\t%02d:%02d:%02d\t%d", gfbySZP[i][0], gfbySZP[i][1], gfbySZP[i][2], gfbySZP[i][3], gfbySZP[i][4], gfbySZP[i][5], gfbySZP[i][6], gfbySZP[i][7]);
									}

									// Daten anhängen
									strcat(fchTeD, fchH);
								}
							}
						}
						else
						{
							// Adresse ungültig
							strcpy(fchTeD, "FE,AD");
						}

						// Zugriffsfehler löschen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Adresse prüfen
						if (iTeA < 25)
						{
							// Daten speichern
							iID = 0;
							iID2 = iTeA;

							// Hilfsvariablen
							byte byH0, byH1, byH2, byH3, byH4, byH5, byH6, byH7;

							// Telegrammdaten durchgehen
							for (int i = 0; i < strlen(fchTeD); i++)
							{
								if (fchTeD[i] == ';')
								{
									// neuer Wert
									fchH[iID] = 0;
									sscanf(fchH, "%d\t%2d:%2d:%2d\t%2d:%2d:%2d\t%d", &byH0, &byH1, &byH2, &byH3, &byH4, &byH5, &byH6, &byH7);

									// der direkte Zugriff funktioniert sonst nicht
									gfbySZP[iID2][0] = byH0;
									gfbySZP[iID2][1] = byH1;
									gfbySZP[iID2][2] = byH2;
									gfbySZP[iID2][3] = byH3;
									gfbySZP[iID2][4] = byH4;
									gfbySZP[iID2][5] = byH5;
									gfbySZP[iID2][6] = byH6;
									gfbySZP[iID2][7] = byH7;

									// Indizes anpassen
									iID = 0;
									iID2++;
								}
								else
								{
									// Wert zusammenbauen
									fchH[iID++] = fchTeD[i];
								}
							}
							// letzter Wert
							fchH[iID] = 0;
							sscanf(fchH, "%d\t%2d:%2d:%2d\t%2d:%2d:%2d\t%d", &byH0, &byH1, &byH2, &byH3, &byH4, &byH5, &byH6, &byH7);

							// der direkte Zugriff funktioniert sonst nicht
							gfbySZP[iID2][0] = byH0;
							gfbySZP[iID2][1] = byH1;
							gfbySZP[iID2][2] = byH2;
							gfbySZP[iID2][3] = byH3;
							gfbySZP[iID2][4] = byH4;
							gfbySZP[iID2][5] = byH5;
							gfbySZP[iID2][6] = byH6;
							gfbySZP[iID2][7] = byH7;
						}
						else
						{
							// Adresse ungültig
							strcpy(fchTeD, "FE,AD");
						}

						// Zugriffsfehler löschen
						boFeZu = false;
					}
				}
				else
				{
					// unbekannter Befehl
					strcpy(fchTeD, "FE,?");
					// Zugriffsfehler löschen
					boFeZu = false;
				}

				// Zugriffsfehler
				if (boFeZu)
				{
					strcpy(fchTeD, "FE,ZG");
				}
			}

			// Telegramm für die Antwort erstellen
			sprintf(fchH, "%s,%s,%d", fchTeB, fchTeZ, iTeA);
			// Telegrammdaten bei Vorhandensein einfügen
			if (strlen(fchTeD) > 0)
			{
				strcat(fchH, ",");
				strcat(fchH, fchTeD);
			}

			// Debugausgabe
			/*
			sprintf(gbyKom, "Debug Ausgang: TeB: %s TeZ: %s TeA: %d TeD: %s\r", fchTeB, fchTeZ, iTeA, fchTeD);
			Serial.print(gbyKom);
			Serial.flush();
			*/

			// Sendeprotokoll zusammenbauen
			sprintf(gbyKom, "#%d,%d,%s,%d\r", iPrN, strlen(fchH), fchH, Pruefsumme(fchH));
				
			// Antwort senden
			Serial.print(gbyKom);
			Serial.flush();

			// Variablen zurücksetzen
			giKomIdx = 0;
		}
	}
}

// Sekundenzyklus
void SekZyklus()
{
	static unsigned int uiSekZ = 0;

	// Sekundenzähler erhöhen
	uiSekZ++;

	// Zyklus ausführen
	if (uiSekZ > ZYK_SEK)
	{
		// Diagnose LED umschalten
		gboLED = !gboLED;
		if (gboLED)
		{
			digitalWrite(DA_LED, HIGH);
		}
		else
		{
			digitalWrite(DA_LED, LOW);
		}

		// Zähler rücksetzen
		uiSekZ = 1;

		// Uhrzeit anpassen
		giSek++;

		if (giSek >= 60)
		{
			// Sekunden löschen
			giSek -= 60;

			// Minute erhöhen
			giMin++;

			if (giMin >= 60)
			{
				// Minuten löschen
				giMin -= 60;

				// Stunden erhöhen
				giStd++;

				if (giStd >= 24)
				{
					// Stunden löschen
					giStd -= 24;
				}
			}
		}
	}
}

// Prüfsummenberechnung
int Pruefsumme(const char* pchZK)
{
	// Hilfsvariable
	byte crc = 0xFF;

	// Berechnung durchführen
	for (int i = 0; i < strlen(pchZK); i++)
	{
		byte c = byte(pchZK[i]);
		for (int j = 1; j <= 8; j++)
		{
			byte f = byte(1 & (c ^ crc));
			crc /= 2;
			c /= 2;
			if (f > 0)
			{
				crc ^= byte(0x8C);
			}
		}
	}

	// Resultat ausgeben
	return int(crc);
}

