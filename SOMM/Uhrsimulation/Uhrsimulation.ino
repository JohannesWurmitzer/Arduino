#pragma region Bausteinkopf
/*
 Helferlein.ino
 Datenerfassung des Bogens mit Kommunikation zum �bergeordneten System
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

#pragma region Ein- und Ausg�nge
const int DA_LED = 13;                              // Diagnose-LED, soll w�hrend des Betriebes blinken
#pragma endregion

#pragma region Zyklen
const unsigned long ZYK_INT = 1;					// Hauptintervall der Zykluszeit [ms]
const unsigned long ZYK_INT10 = 10;					// Hauptintervall 10-fach verlangsamt [ms]
const unsigned long ZYK_SEK = 1000 / ZYK_INT;       // Zyklen pro Sekunde
unsigned long gulZykSt;                             // Startzeit des aktuellen Zyklus [ms]
unsigned long gulZykStN;                            // Startzeit des n�chsten Zyklus [ms]
bool gboZykUL;                                      // Zykluszeit �berlauf erkannt
bool gboLED;                                        // Merker der Diagnose-LED
#pragma endregion

#pragma region Kommunikation
const unsigned int KOM_BUF = 256;					// Kommunikationsbuffergr��e
char gbyKom[KOM_BUF];								// Kommunikation Eingang, maximal X Byte, davon gibt es ein Start und Endezeichen!
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
byte gfbyJAP[54];									// Jahresprogramm
byte gfbyWOP[53][7];								// Wochenprogramme
#pragma endregion

// Initialisierung nach dem Neustart, bzw. R�cksetzen des Systems
void setup() {
	// Ein- und Ausg�nge initialisieren
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

	// �berlaufsmerker zur�cksetzen
	if (gboZykUL && (gulZykSt <= gulZykStN))
	{
		gboZykUL = false;
	}

	// Hauptzykluszeit = X ms
	if (gulZykSt >= gulZykStN)
	{
		// Startzeit des aktuellen Zyklus speichern
		gulZykStN += ZYK_INT;

		// �berlauf detektieren
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

	// Zyklus ausf�hren
	if (uiInt10Z > ZYK_INT10)
	{
		// Diagnosenachricht
		// Serial.println(gulZykSt);

		// Z�hler zur�cksetzen
		uiInt10Z = 1;

		// Hilfsvariable - Kommunikationseingang erkannt
		bool boKomEin = false;

		// auf Dateneingang warten
		while ((Serial.available() > 0) && !boKomEin)
		{
			// Eingangsbyte lesen
			gbyKom[giKomIdx] = Serial.read();

			// Protokollanfang pr�fen
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

				// Index erh�hen
				giKomIdx++;

				// ung�ltige Daten erhalten, Inhalt l�schen
				if ((giKomIdx >= KOM_BUF) && !boKomEin)
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
			// Hilfsvariablen f�r die Zwischenspeicherung der Daten
			int iID = 0;
			int iID2 = 0;
			int iID3 = 0;
			char fchH[KOM_BUF];

			// Protokollvariablen
			int iPrN = 0;				// Protokollnummer
			int iPrL = 0;				// Protokolll�nge
			char fchTeB[5] = "";		// Telegrammbefehl
			char fchTeZ[2] = "";		// Telegrammzugriff
			int iTeA = -1;				// Telegrammadresse
			char fchTeD[KOM_BUF] = "";	// Telegrammdaten
			int iPrP = -1;				// Protokollpr�fsumme

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
					// Zeichenkette abschlie�en
					fchH[iID] = 0;

					// Spalte detektiert
					if (iPrN == 0)
					{
						// Protokollnummer auslesen
						iPrN = atoi(fchH);
					}
					else if (iPrL == 0)
					{
						// Protokolll�nge auslesen
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
						// neue Daten anh�ngen
						strcpy(fchTeD, fchH);
					}

					// Index r�cksetzen
					iID = 0;
				}
			}
			// Zeichenkette abschlie�en
			fchH[iID] = 0;
			// der letzte Inhalt sollte immer die Pr�fsumme sein
			iPrP = atoi(fchH);

			// Hilfszeichenkette Telegramm
			sprintf(fchH, "%s,%s,%d", fchTeB, fchTeZ, iTeA);
			// Telegrammdaten nur bei Vorhandensein anh�ngen
			if (strlen(fchTeD) > 0)
			{
				strcat(fchH, ",");
				strcat(fchH, fchTeD);
			}

			// Pr�fsumme ermitteln
			int iPS = Pruefsumme(fchH);

			// Debugausgabe
			/*
			sprintf(gbyKom, "Debug Eingang: PrN: %d PrL: %d TeB: %s TeZ: %s TeA: %d TeD: %s PrP: %d\r", iPrN, iPrL, fchTeB, fchTeZ, iTeA, fchTeD, iPrP);
			Serial.print(gbyKom);
			Serial.flush();
			*/
				
			// �berpr�fung
			if (iPrP != iPS)
			{
				// Pr�fsummenfehler entdeckt (Pr�fsumme erhalten / Pr�fsumme berechnet)
				sprintf(fchTeD, "FE,PS,%d,%d", iPrP, iPS);
			}
			else if (iPrL != strlen(fchH))
			{
				// Telegramml�nge fehlerhaft
				sprintf(fchTeD, "FE,LA,%d", iPrL);
			}
			else
			{
				// ung�ltiger Zugriff
				bool boFeZu = true;

				// Befehle abfragen
				if (strcmp(fchTeB, "VER") == 0)
				{
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Versionsnummer abfragen
						strcpy(fchTeD, gfchVNr);
						// Zugriffsfehler l�schen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "SER") == 0)
				{
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Seriennummer abfragen
						strcpy(fchTeD, gfchSNr);
						// Zugriffsfehler l�schen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Seriennummer schreiben
						if (strlen(fchTeD) < sizeof(gfchSNr))
						{
							// L�nge passt
							strcpy(gfchSNr, fchTeD);
						}
						else
						{
							// ung�ltige L�nge
							strcpy(fchTeD, "FE,SN");
						}
						// Zugriffsfehler l�schen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "DAT") == 0)
				{
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Datum und Uhrzeit lesen
						sprintf(fchTeD, "%s %02d:%02d:%02d", gfchDat, giStd, giMin, giSek);
						// Zugriffsfehler l�schen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Datum und Uhrzeit schreiben
						sscanf(fchTeD, "%s %2d:%2d:%2d", gfchDat, &giStd, &giMin, &giSek);
						// Zugriffsfehler l�schen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "JAP") == 0)
				{
					// Jahresprogramm lesen / schreiben
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Daten hinzuf�gen
						for (int i = 0; i < sizeof(gfbyJAP); i++)
						{
							if (i > 0)
							{
								// Eintr�ge mit Leerzeichen einf�gen
								sprintf(fchH, " %d", gfbyJAP[i]);
							}
							else
							{
								// erster Eintrag, ohne Leerzeichen
								sprintf(fchH, "%d", gfbyJAP[i]);
							}

							// Daten anh�ngen
							strcat(fchTeD, fchH);
						}
						// Zugriffsfehler l�schen
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
							if (fchTeD[i] == ' ')
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

						// Zugriffsfehler l�schen
						boFeZu = false;
					}
				}
				else if (strcmp(fchTeB, "WOP") == 0)
				{
					// Wochenprogramme lesen / schreiben
					if (strcmp(fchTeZ, "L") == 0)
					{
						// Adresse pr�fen
						if (iTeA < 53)
						{
							// Daten hinzuf�gen
							for (int i = iTeA; i < iTeA + 7; i++)
							{
								// nur g�ltige Werte einf�gen
								if (i < 53)
								{
									if (i > iTeA)
									{
										// Eintr�ge mit Trennzeichen einf�gen
										sprintf(fchH, ";%d %d %d %d %d %d %d", gfbyWOP[i][0], gfbyWOP[i][1], gfbyWOP[i][2], gfbyWOP[i][3], gfbyWOP[i][4], gfbyWOP[i][5], gfbyWOP[i][6]);
									}
									else
									{
										// erster Eintrag, ohne Trennzeichen
										sprintf(fchH, "%d %d %d %d %d %d %d", gfbyWOP[i][0], gfbyWOP[i][1], gfbyWOP[i][2], gfbyWOP[i][3], gfbyWOP[i][4], gfbyWOP[i][5], gfbyWOP[i][6]);
									}

									// Daten anh�ngen
									strcat(fchTeD, fchH);
								}
							}
						}
						else
						{
							// Adresse ung�ltig
							strcpy(fchTeD, "FE,AD");
						}

						// Zugriffsfehler l�schen
						boFeZu = false;
					}
					else if (strcmp(fchTeZ, "S") == 0)
					{
						// Adresse pr�fen
						if (iTeA < 53)
						{
							// Daten speichern
							iID = 0;
							iID2 = 0;
							iID3 = iTeA;

							// Telegrammdaten durchgehen
							for (int i = 0; i < strlen(fchTeD); i++)
							{
								if ((fchTeD[i] == ' ') || (fchTeD[i] == ';'))
								{
									// neuer Wert
									fchH[iID] = 0;
									gfbyWOP[iID3][iID2++] = atoi(fchH);
									iID = 0;

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
							// Adresse ung�ltig
							strcpy(fchTeD, "FE,AD");
						}

						// Zugriffsfehler l�schen
						boFeZu = false;
					}
				}
				else
				{
					// unbekannter Befehl
					strcpy(fchTeD, "FE,?");
					// Zugriffsfehler l�schen
					boFeZu = false;
				}

				// Zugriffsfehler
				if (boFeZu)
				{
					strcpy(fchTeD, "FE,ZG");
				}
			}

			// Telegramm f�r die Antwort erstellen
			sprintf(fchH, "%s,%s,%d", fchTeB, fchTeZ, iTeA);
			// Telegrammdaten bei Vorhandensein einf�gen
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

			// Variablen zur�cksetzen
			giKomIdx = 0;
		}
	}
}

// Sekundenzyklus
void SekZyklus()
{
	static unsigned int uiSekZ = 0;

	// Sekundenz�hler erh�hen
	uiSekZ++;

	// Zyklus ausf�hren
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

		// Z�hler r�cksetzen
		uiSekZ = 1;

		// Uhrzeit anpassen
		giSek++;

		if (giSek >= 60)
		{
			// Sekunden l�schen
			giSek -= 60;

			// Minute erh�hen
			giMin++;

			if (giMin >= 60)
			{
				// Minuten l�schen
				giMin -= 60;

				// Stunden erh�hen
				giStd++;

				if (giStd >= 24)
				{
					// Stunden l�schen
					giStd -= 24;
				}
			}
		}
	}
}

// Pr�fsummenberechnung
int Pruefsumme(const char* pchZK)
{
	// Hilfsvariable
	byte crc = 0xFF;

	// Berechnung durchf�hren
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

