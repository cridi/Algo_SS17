// User.cpp : header file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "user.h"
#include "graphics\graphicfunctions.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#ifndef _USE_OLD_OSTREAMS
using namespace std;
#endif 



#include "math.h"

#define _pi					3.1415926535							// Die Zahl Pi.
#define _180_durch_pi		57.2957795147							// = 180 * pi. Dient zur Laufzeitoptinierung.
#define _sinus(winkel)		sin((winkel) / _180_durch_pi)			// Funktion im Gradmass.
#define _cosinus(winkel)	cos((winkel) / _180_durch_pi)			// Funktion im Gradmass.

COLORREF Colref[] = { BLACK,RED,GREEN,BLUE,YELLOW,BROWN };
int Colind = 0;

//-------------------------------------------------------------------------------------------------------//
const int IDENTIFIER = 4;
const int INTEGER1 = 5;
const int TOKENSTART = 300;

//USER PROGRAMM
vector<string>SpaceLess;
string BauteilToken, NetworkToken;
string OuterSternPins = "";

class Network {
	/*********************************************************
	Die Klasse Beschreibt das Netzwerk im Allgemeinen wie es von
	Außen zu betrachten ist. Es werden Knotenarten definiert um 
	später unterscheiden zu können ob diese wegrationalisiert 
	werden dürfen. 
	Für die Berechnung der Übertragungsfunktion werden diese 
	Informationen ebenfalls verwendet.
	***********************************************************/
public:
	string INPUT;
	string OUTPUT;
	string CMN;
	vector<string> INTERNALS;

	Network();
	Network(string INPUT, string OUTPUT, string CMN, vector<string> INTERNALS);
};
Network::Network() {}
Network::Network(string a, string b, string c, vector<string> d) {
	INPUT = a;
	OUTPUT = b;
	CMN = c;
	INTERNALS = d;
}

class Bauteil {
	/*********************************************************
	Diese Klasse beschreibt Bauteile wie sie in RLC Netzwerken 
	vorkommen
	Bauteile haben die Eigenschaften Name, Art, Pin1 und Pin2. 
	Da nur absrakte Übertragungsfunktionen erzeugt werden, sind
	keine Numerischen Werte vorgesehen
	***********************************************************/
public:
	string Name;
	string Art;
	string Pin1;
	string Pin2;

	Bauteil(string Name, string Art, string Pin1, string Pin2);
	Bauteil();
};

Bauteil::Bauteil(string a, string b, string c, string d) {
	Name = a;
	Art = b;
	Pin1 = c;
	Pin2 = d;
}
Bauteil::Bauteil() {};

Network Netzwerk;
vector<Bauteil*> Bauteile;
vector<Bauteil*> serial_Bauteile;
vector<char*> GraphicOutput;

void Is_parallel() {
	/*********************************************************
	Diese Funktion überprüft ob zwei Bauteile parallel sind. 
	Ist dies der Fall so werden diese zusammengefasst und an das
	Ende des Bauteilvektors geschrieben.
	Die Zusammengefassten Bauteile werden anschließend gelöscht.
	***********************************************************/
	int outerSweep = Bauteile.size();		
	int innerSweep = Bauteile.size();

	for (int i = 0; i < outerSweep; i++) {
		for (int ii = 0; ii < innerSweep; ii++) {

			if (i == ii) {
				ii++;
				if (ii == innerSweep)break;
			}

			if (((Bauteile.at(i)->Pin2 == Bauteile.at(ii)->Pin1)					//Überprüfung ob zwei Bauteile parallel sind
				&& (Bauteile.at(i)->Pin1 == Bauteile.at(ii)->Pin2))					//TRUE Wenn beide Pins gleich sind
				||
				((Bauteile.at(i)->Pin2 == Bauteile.at(ii)->Pin2) 
				&& (Bauteile.at(ii)->Pin1 == Bauteile.at(i)->Pin1))) {			

						Bauteile.push_back(new Bauteil(Bauteile.at(i)->Name 
							+ "||" +												//Neues Bauteil mit den Namen der Bauteile die
							Bauteile.at(ii)->Name,									//getrennt von einem ||
							"Z", 
							Bauteile.at(i)->Pin1,									//Da beide Bauteile an den gleichen Pins hängen
							Bauteile.at(i)->Pin2));			

						cout << "Zusammenfassung: " << Bauteile.at(i)->Name			//Konsolenausgabe
							<< "||" << Bauteile.at(ii)->Name << endl;
						
						string BufferStr = Bauteile.at(i)->Name + "||" +			//für die Ausgabe im GDE muss std::string konvertiert
										   Bauteile.at(ii)->Name;					//werden

						char* Buffer = new char [BufferStr.size() + 1];				//Dafür wird Speicherplatz freigegeben 
						strcpy(Buffer, BufferStr.c_str());							//Und der konvertierte String hinkopiert
																					//Anschließend kann die Adresse an die graf Ausg.
						GraphicOutput.push_back(Buffer);							//übergeben werden.

					if (i < ii) {
						Bauteile.erase(Bauteile.begin() + i);						//Wenn zuerst das Element an dem kleineren iterator
						Bauteile.erase(Bauteile.begin() + ii - 1);					//Wert gelöscht wird, muss beim zweiten löschen die
					}																//kleinere Vektorgröße bedacht werden.
					else {
						Bauteile.erase(Bauteile.begin() + ii);						
						Bauteile.erase(Bauteile.begin() + i - 1);
					}

					return;
			}
		}
	}

}

bool Is_serial_Pin(string Pin_to_check) {
	/*********************************************************
	Diese Funktion überprüft ob ein Pin ein serieller Pin ist.
	Ist dies der Fall gibt Sie ein TRUE zurück.
	Einen seriellen Pin klassifiziert, dass nur 2, Bauteile an 
	diesem angeschlossen sind.
	Diese Information wird später benötigt um Serielle Bauteile
	zusammen zu fassen. 
	***********************************************************/
	int checksum = 0;

	for (int i = 0; i < Netzwerk.INTERNALS.size(); i++) {

		if (!Pin_to_check.compare(Netzwerk.INTERNALS.at(i))) {							//Überprüfung ob der Pin ein Internal ist
																						//Der auch weggekürzt werden darf
			for (unsigned int i = 0; i < Bauteile.size(); i++) {						//Bauteil für Bauteil wird überprüft ob...
				if (!((Bauteile.at(i)->Pin1).compare(Pin_to_check))) { checksum++; };	//exakt zwei Bauteile an einem Pin hängen
				if (!((Bauteile.at(i)->Pin2).compare(Pin_to_check))) { checksum++; };	
			}
		}
	}

	if (checksum != 2) return false;													//Wenn dies der Fall ist so gibt die Funktion ein
	if (checksum == 2) return true;														// TRUE zurück
}

void find_serial_Bauteil(string serial_Pin) {
	/*********************************************************
	Diese Funtkion überprüft welche Bauteile an dem zuvor 
	gefundenen seriellen Pin angeschlossen sind und speichert
	diese zwischen in einem neuen Vektor
	***********************************************************/
	for (unsigned int i = 0; i < Bauteile.size(); i++) {
		if ((!((Bauteile.at(i)->Pin1).compare(serial_Pin))) || (!((Bauteile.at(i)->Pin2).compare(serial_Pin)))) {
			serial_Bauteile.push_back(Bauteile.at(i));
			break;
		}
	}
}

void reverse_find_serial_Bauteil(string serial_Pin) {
	/*********************************************************
	Diese Funktion funktioniert wie find_serial_Bauteil() mit 
	dem Unterschied, dass Sie die Bauteile von hinten überprüft.
	So wird sicher gestellt, dass beide an dem seriellen Pin 
	befindlichen Bauteile gefunden werden. 
	***********************************************************/
	for (unsigned int i = Bauteile.size() - 1; i >= 0; i--) {
		if ((!((Bauteile.at(i)->Pin1).compare(serial_Pin))) || (!((Bauteile.at(i)->Pin2).compare(serial_Pin)))) {
			serial_Bauteile.push_back(Bauteile.at(i));
			break;
		}
	}
}

void find_outer_Pins(string serial_Pin) {

	/********************************************************
	Diese Funktion überprüft die Bauteile die als 
	Reihenschaltung zusammengefasst werden sollen und stellt 
	fest was die äußeren Pins dieser Reihenschaltung sind. 
	********************************************************/
	
	if ((serial_Pin.compare(serial_Bauteile.at(0)->Pin2))) {
		(serial_Bauteile.at(0)->Pin1) = (serial_Bauteile.at(0)->Pin2);		//Wenn ein Pin eines Bauteils nicht mit dem
	}																		//seriellen Pin übereinstimmt, so wird er als
	if ((serial_Pin.compare(serial_Bauteile.at(1)->Pin2))) {				//Pin 1 definiert. So wird sicher gestellt, dass 
		(serial_Bauteile.at(1)->Pin1) = (serial_Bauteile.at(1)->Pin2);		//in Pin 1 immer die äußeren Pins stehen
	}
}

void create_new_serial_Bauteil() {

	/********************************************************
	Diese Funktion fasst zwei Bauteile zu einem seriellen 
	Bauteil zusammen und löscht die alten Bauteile.
	********************************************************/

	Bauteile.push_back(new Bauteil("(" + serial_Bauteile.at(0)->Name + " + " +  //Ein neues serielles Bauteil wird erstellt.
									serial_Bauteile.at(1)->Name + ")",			//Der Name besteht aus den beiden Namen der Bauteile
									"Z",										//und wird durch ein Plus getrennt.
									serial_Bauteile.at(0)->Pin1,				//das Bauteil bekommt die äußeren Pins der einzelnen
									serial_Bauteile.at(1)->Pin1));				//Bauteile
	
	cout << "Zusammenfassung: " << serial_Bauteile.at(0)->Name << "+" 
		<< serial_Bauteile.at(1)->Name << endl;									//Ausgabe der Konsole

	string BufferStr = serial_Bauteile.at(0)->Name + " + " +					//für die Ausgabe im GDE muss std::string konvertiert
		serial_Bauteile.at(1)->Name;											//werden

	char* Buffer = new char[BufferStr.size() + 1];								//Dafür wird Speicherplatz freigegeben 
	strcpy(Buffer, BufferStr.c_str());											//Und der konvertierte String hinkopiert
																				//Anschließend kann die Adresse an die graf Ausg.
	GraphicOutput.push_back(Buffer);											//übergeben werden.

	for (int i = 0; i < Bauteile.size(); i++) {
		if (!(serial_Bauteile.at(0)->Name.compare(Bauteile.at(i)->Name)) ||		//Die Bauteile die zusammengefasst wurden 
			(!(serial_Bauteile.at(1)->Name.compare(Bauteile.at(i)->Name))))		//werden gesucht und gelöscht.
		{
			Bauteile.erase(Bauteile.begin() + i);								//Da der Vektor nun ein Bauteil kürzer ist
			i = -1;																//Muss der Iterator gelöscht werden.
		}
	}

	serial_Bauteile.erase(serial_Bauteile.begin() + 1);							//Der Buffer für die Seriellen Bauteile wird gelöscht
	serial_Bauteile.erase(serial_Bauteile.begin() + 0);

}

void Is_serial() {
	/********************************************************
	Diese Funktion sucht nach seriellen Bauteilen im Netzwerk.
	********************************************************/
	vector<string*> serial_Pins;
int c = 0;
int PinFlag = 0;

for (int i = 0; i < Bauteile.size(); i++) {											
	PinFlag = 0;
	if (Is_serial_Pin(Bauteile.at(i)->Pin1)) {										//Alle seriellen Pin des Netzwerks werden detektiert
		for (int ii = 0; ii < serial_Pins.size(); ii++) {							//Und in einem Vektor gespeichert.
			if (!(serial_Pins.at(ii)->compare(Bauteile.at(i)->Pin1))) PinFlag = 1;	//Wenn der Pin bereits als serieller Pin gefunden wurde
		}																			//wird ein EscapeFlag gesetzt, dass dieser nicht
																					//Öfter in den Vektor geschrieben wird.
		if (PinFlag == 0) {
			serial_Pins.push_back(new string(Bauteile.at(i)->Pin1));
		}
	}
	else if (Is_serial_Pin(Bauteile.at(i)->Pin2)) {									//Die selbe Überprüfung findet für Pin 2 statt.

		for (int ii = 0; ii < serial_Pins.size(); ii++) {
			if (!(serial_Pins.at(ii)->compare(Bauteile.at(i)->Pin2))) PinFlag = 1;
		}
		if (PinFlag == 0) {
			serial_Pins.push_back(new string(Bauteile.at(i)->Pin2));
		}
	}
}
for (int i = 0; i < serial_Pins.size(); i++) {										//Hat man nun alle seriellen Pins im Netzwerk gefunden
	find_serial_Bauteil(*serial_Pins.at(i));										//Werden die anliegenden bauteile in einen Buffer 
	reverse_find_serial_Bauteil(*serial_Pins.at(i));								//geschrieben
	find_outer_Pins(*serial_Pins.at(i));											//Die äußeren Pins dieser beiden Bauteile bestimmt
	create_new_serial_Bauteil();													//und schließlich ein neues Bauteil erstellt.
}

}

vector<Bauteil*> SternFind(vector<vector<Bauteil*>> AdjazenzMatrix, string Pins) {
	/********************************************************
	Mit dieser Funktion wird überprüft ob im Netzwerk 
	Sternpunkte vorhanden sind. Anschließend wird eine Stern
	Dreieck Wandlung nötig sein.

	Die Funktion gibt die drei am Sternpunkt hängenden 
	Bauteile in einem Vektor wieder zurück.
	********************************************************/
	vector<Bauteil*> SternBauteile;

	int SternCounter = 0;
	int rows = Pins.size();
	int cols = Pins.size();

	for (int outer = 0; outer < rows; outer++) {									// Durchgehen der Pins (outer) in der Adjazenzmatrix zur Überprüfung ob an einem Pin sich genau 3 Bauteile befinden -> Stern
		for (int i = 0; i < rows; i++) {	
			if (AdjazenzMatrix[i][outer] != NULL) {									//Überprüfung der Zeilen ob an Pin "outer" sich Bauteile befinden 
				SternCounter++;
				SternBauteile.push_back(AdjazenzMatrix[i][outer]);
				OuterSternPins = OuterSternPins + Pins[i];
			}
			if (AdjazenzMatrix[outer][i] != NULL) {									//Überprüfung der Spalten ob an Pin "outer" sich Bauteile befinden
				SternCounter++;
				SternBauteile.push_back(AdjazenzMatrix[outer][i]);
				OuterSternPins = OuterSternPins + Pins[i];
			}
		}

		if (SternCounter == 3) {
			if ((Pins.substr(outer, 1) != Netzwerk.CMN) &&							//Abfangen für den Fall, dass der Sternpin sich an Ground, Output oder Input befindet
				(Pins.substr(outer, 1) != Netzwerk.OUTPUT) &&						//Dies kann, bzw. darf nicht passieren da der Sternpin gelöscht wird, bzw. umgeformt
				(Pins.substr(outer, 1) != Netzwerk.INPUT)) return SternBauteile;
		}
		SternCounter = 0;
		OuterSternPins = "";
		SternBauteile.clear();
	}
}

void SternAdjazenz() {
	/********************************************************
	Erkennt und fasst Sternform in Schaltungen zusammen
	********************************************************/
	vector<Bauteil*> SternBauteile;
	string SternPin;
	string Pins = "";


	for (int i = 0; i < Bauteile.size(); i++) {

		if (Pins.find(Bauteile.at(i)->Pin1) == Pins.npos) {								//for-Schleife zum extrahieren aller noch verwendeten Pins und speichern in einem String
			Pins = Pins + Bauteile.at(i)->Pin1;
		}
		if (Pins.find(Bauteile.at(i)->Pin2) == Pins.npos) {
			Pins = Pins + Bauteile.at(i)->Pin2;
		}
	}
	int rows = Pins.size();
	int cols = Pins.size();
	vector<vector<Bauteil*>> AdjazenzMatrix;											//Eine matrix mit Bauteile.size*Bauteile.size
	AdjazenzMatrix.resize(rows);														//Diese ist vom Typ Bauteil

	for (int i = 0; i < rows; i++) {													//Aufbau einer Adjazenz Matrix vom Typ Class Bauteil mit der Länge und Breite unserer Pinanzahl
		AdjazenzMatrix[i].resize(cols);
	}
	for (int i = 0; i < Bauteile.size(); i++) {												
		AdjazenzMatrix[Pins.find(Bauteile.at(i)->Pin1)]									//Befüllen der Matrix mit den Bauteilen ensprechend der Reihenfolge, wie sie im String Pins aufgreiht wurden
					  [Pins.find(Bauteile.at(i)->Pin2)] = Bauteile.at(i);				//und entsprechend wie sie mit diesen Pins verbunden sind
	}

	SternBauteile = SternFind(AdjazenzMatrix, Pins);									//Sternbauteile werden gesucht und wenn 
																						//vorhanden zurückgegeben


	if (SternBauteile.at(0)->Pin1 == SternBauteile.at(1)->Pin1){						//Es wird der Sternpin bestimmt
		SternPin = SternBauteile.at(0)->Pin1;											//Der nach der Wandlung zum 3eck
	}																					//gelöscht wird
	else if (SternBauteile.at(0)->Pin1 == SternBauteile.at(1)->Pin2) {
		SternPin = SternBauteile.at(0)->Pin1;
	}
	else SternPin = SternBauteile.at(0)->Pin2;
	Pins.erase(Pins.find(SternPin), 1);							


	string* ValueBauteile = new string[OuterSternPins.size()];

	for (int i = 0; i < SternBauteile.size(); i++) {									//Die Namen der Sternbauteile werden 
		for (int ii = 0; ii < OuterSternPins.size(); ii++) {										//zwischengespeichert um Sie später als 
			if (SternBauteile.at(i)->Pin1 == OuterSternPins.substr(ii, 1)) {						//Formel ausgeben zu können
				ValueBauteile[ii] = SternBauteile.at(i)->Name;
			}
			else if (SternBauteile.at(i)->Pin2 == OuterSternPins.substr(ii, 1)) {
				ValueBauteile[ii] = SternBauteile.at(i)->Name;
			}
		}
	}

	
	
	GraphicOutput.push_back("");
	GraphicOutput.push_back("***Stern zu Dreieck Wandlung:***");
	GraphicOutput.push_back("********************************");


	Bauteile.push_back(new Bauteil("ZD" + 
		OuterSternPins.substr(0, 1) +
		OuterSternPins.substr(1, 1), "Z",
		OuterSternPins.substr(0, 1),
		OuterSternPins.substr(1, 1)));

	cout << "Zusammenfassung: " << "ZD" + OuterSternPins.substr(0, 1) + OuterSternPins.substr(1, 1) << " = "		//Ausgabe nach Formel der 
		<< ValueBauteile[0] << " + " << ValueBauteile[1] << " + " << "(" <<						//Stern Dreieck Wandlung 
		ValueBauteile[0] << "*"	<< ValueBauteile[1] << ")" << "/" << ValueBauteile[2] << endl;

	string BufferStr = "ZD" + OuterSternPins.substr(0, 1) + OuterSternPins.substr(1, 1) +" = " +					//Dafür wird Speicherplatz freigegeben 
		ValueBauteile[0] + " + " + ValueBauteile[1] + " + " + "(" + ValueBauteile[0] + "*" +	//Und der konvertierte String hinkopiert
		ValueBauteile[1] + ")" + "/" + ValueBauteile[2];										//Anschließend kann die Adresse an die graf Ausg.
		ValueBauteile[1] + ")" + "/" + ValueBauteile[2];										//übergeben werden.


	char* Buffer = new char[BufferStr.size() + 1];
	strcpy(Buffer, BufferStr.c_str());

	GraphicOutput.push_back(Buffer);

	/********************************************************
	Zweites bauteil des Dreieck wird Ausgegeben und in den 
	Bauteile Vector geschrieben
	********************************************************/

	Bauteile.push_back(new Bauteil("ZD" + 
		OuterSternPins.substr(0, 1) +
		OuterSternPins.substr(2, 1), "Z",
		OuterSternPins.substr(0, 1),
		OuterSternPins.substr(2, 1)));

	cout << "Zusammenfassung: " << "ZD" + OuterSternPins.substr(0, 1) + OuterSternPins.substr(2, 1) << " = " <<
		ValueBauteile[0] << " + " << ValueBauteile[2] << " + " << "(" << 
		ValueBauteile[0] << "*" << ValueBauteile[2] << ")" << "/" << ValueBauteile[1] << endl;

	BufferStr = "ZD" + OuterSternPins.substr(0, 1) + OuterSternPins.substr(2, 1) + " = " +
		ValueBauteile[0] + " + " + ValueBauteile[2] + " + " +"(" + 
		ValueBauteile[0] + "*" + ValueBauteile[2] + ")" + "/" + ValueBauteile[1];

	Buffer = new char[BufferStr.size() + 1];
	strcpy(Buffer, BufferStr.c_str());

	GraphicOutput.push_back(Buffer);

	/********************************************************
	Drittes bauteil des Dreieck wird Ausgegeben und in den
	Bauteile Vector geschrieben
	********************************************************/

	Bauteile.push_back(new Bauteil("ZD" + 
		OuterSternPins.substr(1, 1) +
		OuterSternPins.substr(2, 1), "Z",
		OuterSternPins.substr(1, 1),
		OuterSternPins.substr(2, 1)));

	cout << "Zusammenfassung: " << "ZD" + OuterSternPins.substr(1, 1) + OuterSternPins.substr(2, 1) <<" = " <<
		ValueBauteile[1] << " + " << ValueBauteile[2] << " + " <<"(" << 
		ValueBauteile[1] << "*" << ValueBauteile[2] << ")" << "/" << ValueBauteile[0] << endl;

	BufferStr = "ZD" + OuterSternPins.substr(1, 1) + OuterSternPins.substr(2, 1) + " = " +
		ValueBauteile[1] + " + " + ValueBauteile[2] + " + " + "(" + 
		ValueBauteile[1] + "*" + ValueBauteile[2] + ")" + "/" + ValueBauteile[0];

	Buffer = new char[BufferStr.size() + 1];
	strcpy(Buffer, BufferStr.c_str());

	GraphicOutput.push_back(Buffer);
	GraphicOutput.push_back("********************************");
	GraphicOutput.push_back("");


	/********************************************************
	/Bauteile die vereinfacht wurden werden gelöscht
	********************************************************/


	int Size = Bauteile.size();														
	for (int i = 0; i < Size; i++) {
		if (Bauteile.at(i)->Name == SternBauteile.at(0)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;																		//Iterator dekremtent um das gelöschte Element 
		}																				//zu beachten
		else if (Bauteile.at(i)->Name == SternBauteile.at(1)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		else if (Bauteile.at(i)->Name == SternBauteile.at(2)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		Size = Bauteile.size();															//Endpunkt immer Anpassen um nicht über den
	}																					//Vektor hinaus zu iterieren
}

bool DreieckAdjazenz() {
	/********************************************************
	Erkennt und fasst Dreiecksform in Schaltungen zusammen
	********************************************************/
	vector<Bauteil*> DreieckBauteile;
	string Pins = "";

	for (int i = 0; i < Bauteile.size(); i++) {											//Alle im Netzwerk vorhandenen Pins werden
																						//Zwischengespeichert
		if (Pins.find(Bauteile.at(i)->Pin1) == Pins.npos) {								
			Pins = Pins + Bauteile.at(i)->Pin1;
		}
		if (Pins.find(Bauteile.at(i)->Pin2) == Pins.npos) {
			Pins = Pins + Bauteile.at(i)->Pin2;
		}
	}

	if (Pins.size() != 3)return true;													//Interessanterweise treten Dreiecke nur bei
																						//einer Netzwerkgröße von 3 auf. Sonst kann immer
	DreieckBauteile = Bauteile;															//immer auch ein Stern gefunden werden.

	vector<vector<string>> ValueBauteile;
	ValueBauteile.resize(Pins.size());

	for (int i = 0; i < DreieckBauteile.size(); i++) {									//Durchlaufen der Bauteile und Pins, speichern der Bauteile in eine Matrix
		for (int ii = 0; ii < Pins.size(); ii++) {										//Entsprechend der Stelle wie die Pins im String Pins auftreten, werden sie in den Zeilen der Matrix gespeichert
			if (DreieckBauteile.at(i)->Pin1 == Pins.substr(ii, 1)) {					//Auf den Spalten der Matrix stehen immer zwei Bauteile, da in einer Dreiecksform immer zwei Bauteile einem bestimmten Pin zugeordnet werden kann.
				ValueBauteile[ii].push_back(DreieckBauteile.at(i)->Name); 
			}
			else if (DreieckBauteile.at(i)->Pin2 == Pins.substr(ii, 1)) {
				ValueBauteile[ii].push_back(DreieckBauteile.at(i)->Name);
			}
		}
	}
			/********************************************************
			Ausgabe und Bildung der neuen SternBauteile
			********************************************************/

			GraphicOutput.push_back("");
			GraphicOutput.push_back("***Dreieck zu Stern Wandlung:***");
			GraphicOutput.push_back("********************************");

			/********************************************************
			Êrstes bauteil wird gebildet und ausgegeben
			********************************************************/

			Bauteile.push_back(new Bauteil("Z*" +		
				Pins.substr(0, 1), "Z",
				Pins.substr(0, 1), "*"));

			cout << "Zusammenfassung: " << "Z*" + Pins.substr(0, 1) <<" = " << "(" << 
				ValueBauteile[0][0] << "*" << ValueBauteile[0][1] << ")" << "/" << "(" << 
				DreieckBauteile.at(0)->Name << " + " << DreieckBauteile.at(1)->Name << " + " << 
				DreieckBauteile.at(2)->Name << ")" << endl;

			string BufferStr = "Z*" + Pins.substr(0, 1) + " = " + "(" + 
				ValueBauteile[0][0] + "*" + ValueBauteile[0][1] + ")" + "/" +"(" +				//Die Grafische Ausgabe wird vorbereitet
				DreieckBauteile.at(0)->Name + " + " + DreieckBauteile.at(1)->Name +" + " +		//Dafür wird Speicherplatz freigegeben 
				DreieckBauteile.at(2)->Name + ")";												//Und der konvertierte String hinkopiert
																								
			char* Buffer = new char[BufferStr.size() + 1];										//Anschließend kann die Adresse an die graf Ausg.
																								//übergeben werden.
			strcpy(Buffer, BufferStr.c_str());

			GraphicOutput.push_back(Buffer);

			/********************************************************
			Zweites bauteil wird gebildet und ausgegeben
			********************************************************/

			Bauteile.push_back(new Bauteil("Z*" + 
				Pins.substr(1, 1), "Z",
				Pins.substr(1, 1), "*"));

			cout << "Zusammenfassung: " << "Z*" + Pins.substr(1, 1) << " = " << "(" << 
				ValueBauteile[1][0] << "*" << ValueBauteile[1][1] << ")" << "/" <<"(" << 
				DreieckBauteile.at(0)->Name << " + " << DreieckBauteile.at(1)->Name << " + " << 
				DreieckBauteile.at(2)->Name << ")" << endl;

			BufferStr = "Z*" + Pins.substr(1, 1) + " = " + "(" + 
				ValueBauteile[1][0] + "*" + ValueBauteile[1][1] + ")" + "/" +"(" + 
				DreieckBauteile.at(0)->Name + " + " + DreieckBauteile.at(1)->Name + " + " + 
				DreieckBauteile.at(2)->Name + ")";

			Buffer = new char[BufferStr.size() + 1];
			strcpy(Buffer, BufferStr.c_str());

			GraphicOutput.push_back(Buffer);

			/********************************************************
			Drittes bauteil wird gebildet und ausgegeben
			********************************************************/

			Bauteile.push_back(new Bauteil("Z*" + 
				Pins.substr(2, 1), "Z",
				Pins.substr(2, 1), "*"));

			cout << "Zusammenfassung: " << "Z*" + Pins.substr(2, 1) << " = " << "(" << 
				ValueBauteile[2][0] << "*" << ValueBauteile[2][1] << ")" << "/" << "(" << 
				DreieckBauteile.at(0)->Name << " + " << DreieckBauteile.at(1)->Name << " + " << 
				DreieckBauteile.at(2)->Name << ")" << endl;

			BufferStr = "Z*" + Pins.substr(2, 1) + " = " + "(" + 
				ValueBauteile[2][0] + "*" + ValueBauteile[2][1] + ")" + "/" + "(" + 
				DreieckBauteile.at(0)->Name + " + " + DreieckBauteile.at(1)->Name +" + " + 
				DreieckBauteile.at(2)->Name + ")";

			Buffer = new char[BufferStr.size() + 1];
			strcpy(Buffer, BufferStr.c_str());

			GraphicOutput.push_back(Buffer);
			GraphicOutput.push_back("********************************");
			GraphicOutput.push_back("");
			

	int Size = Bauteile.size();																//Die Zusammen gefassten Bauteile
	for (int i = 0; i < Size; i++) {														//werden gelöscht
		if (Bauteile.at(i)->Name == DreieckBauteile.at(0)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;																			//Iterator dekremtent um das gelöschte Element
		}																					//zu beachten
		else if (Bauteile.at(i)->Name == DreieckBauteile.at(1)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		else if (Bauteile.at(i)->Name == DreieckBauteile.at(2)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		Size = Bauteile.size();																//Endpunkt immer anpassen um nicht über
	}																						//den Vektor hinaus zu  lesen
	return true;
}


char * InputImpedanz() {
	/*********************************************************
	Nachdem das Netzwerk maximal zusammengefasst wurde ermittelt
	diese Funktion das Bauteil, dass sich an am Eingang befindet
	und gibt einen Pointer auf den Namen zurück
	***********************************************************/

	string Input;

	for (int i = 0; i < Bauteile.size(); i++) {
		if ((Bauteile.at(i)->Pin1 == Netzwerk.INPUT) || (Bauteile.at(i)->Pin2 == Netzwerk.INPUT))Input = Bauteile.at(i)->Name;
	}

	char* Buffer = new char[Input.size() + 1];
	strcpy(Buffer, Input.c_str());

	return Buffer;
}

char * CMNImpedanz() {
	/*********************************************************
	Nachdem das Netzwerk maximal zusammengefasst wurde ermittelt
	diese Funktion das Bauteil, dass sich an am Ground befindet
	und gibt einen Pointer auf den Namen zurück
	***********************************************************/

	string CMN;

	for (int i = 0; i < Bauteile.size(); i++) {
		if ((Bauteile.at(i)->Pin1 == Netzwerk.CMN) || (Bauteile.at(i)->Pin2 == Netzwerk.CMN))CMN = Bauteile.at(i)->Name;
	}

	char* Buffer = new char[CMN.size() + 1];
	strcpy(Buffer, CMN.c_str());

	return Buffer;

}

void THE_ALGORITHM() {
	/*********************************************************
	In dieser Funktion werden die von uns programmierten 
	Funktionen möglichst intelligent aufgerufen um das RLC
	Netzwerk zusammen zu fassen.
	***********************************************************/
	int serialLimit = Bauteile.size();
	int parallelLimit = Bauteile.size();
	GraphicOutput.push_back("********************************");
	GraphicOutput.push_back("*LOG DER ZUSAMMENFASSUNGEN:*");
	GraphicOutput.push_back("********************************");
	GraphicOutput.push_back("");

	for (int i = 0; i < 10; i++) {

		for (int SerialParallel = 0; SerialParallel < 5; SerialParallel++) {

			for (unsigned int serial = 0; serial < serialLimit; serial++) {
				Is_serial();
				serialLimit = Bauteile.size();
			}

			for (unsigned int parallel = 0; parallel < parallelLimit; parallel++) {
				Is_parallel();
				parallelLimit = Bauteile.size();
			}
		}

		if (Bauteile.size() == 3) {
			if (DreieckAdjazenz())return;
		}
		else if (Bauteile.size() < 3)return;

		SternAdjazenz();
	}

}


//USER FUNCTIONS
void NetworkInput() {
	/*********************************************************
	Funktion zum Auslesen einer Schaltungsdatei in Form von 

	Nets: a:IN; b:Out; c: CMN; d,e: Internal;
	R1:R ( a, d);
	C2:C ( d, b);
	L4:L ( b, c );

	Funktion erfasst die Zeilen, speichert sie als Strings und 
	entfernt alle Leerzeichen und gibt das Ergebnis als Return-Wert aus.
	***********************************************************/
	vector <string> InputBuffer;
	string StreamBuffer;
	string BauteilBuffer;
	string NetzwerkBuffer;

	ifstream FILE;
	string FilePath = "";
																				//Input von Path-String über Standard Eingabe
	cout << "Enter Circuit File Path or \"Beispiel 1-3\" for example Circuits:" << endl;
	getline(cin, FilePath);

	//"C:/Users/Knoedel/Desktop/Schaltung.txt"


	if (FilePath == "") {
		NetzwerkBuffer = "a:IN; b:Out; c: CMN; d,e: Internal;";					// Defaultwerte
		cout << "Default: " << NetzwerkBuffer << endl << endl;

		BauteilBuffer = "R1:R(a, d); C2:C(d, b); L4:L(b, c);";					// Defaultwerte 
		cout << "No File found -> Default: " << BauteilBuffer << endl << endl;
	}
	else if (FilePath == "Beispiel 1") {
		cout << "Beispiel 1 wird ausgeführt: " << endl;
		FilePath = "C:/Users/Knoedel/Desktop/Schaltungen/Schaltung_A.txt";
	}
	else if (FilePath == "Beispiel 2") {
		cout << "Beispiel 2 wird ausgeführt:" << endl;
		FilePath = "C:/Users/Knoedel/Desktop/Schaltungen/Schaltung_B.txt";
	}
	else if (FilePath == "Beispiel 3") {
		cout << "Beispiel 3 wird ausgeführt:"<< endl;
		FilePath = "C:/Users/Knoedel/Desktop/Schaltungen/Schaltung_C.txt";

	}
	else {

	}

	FILE.open(FilePath, fstream::in);											//Öffnen der Datei im Modus Lesen

	while (!FILE.eof()) {														//Einlesen der Zeilen bis zum Ende der File
		getline(FILE, StreamBuffer);
		InputBuffer.push_back(StreamBuffer);
	}

	NetzwerkBuffer = InputBuffer[0];											//Zeile fürs Netzwerk extrahieren
	BauteilBuffer = InputBuffer[1] + InputBuffer[2] + InputBuffer[3];			//Zeilen für die Bauteile

	cout << "Registered String:\t" << NetzwerkBuffer << endl << endl;
	cout << "Registered String:\t" << BauteilBuffer << endl << endl;

	FILE.close();

	SpaceLess.resize(2);
	SpaceLess[0] = "";
	SpaceLess[1] = "";

	for (int spacepos = 0; spacepos < NetzwerkBuffer.size(); spacepos++) {		//Durchlaufen des Strings und bei Detektion eines Leerzeichen überspringen dieses 
		if (isspace(NetzwerkBuffer[spacepos])) {
			spacepos++;
			if (spacepos == NetzwerkBuffer.size()) break;
		}
		SpaceLess[0] = SpaceLess[0] + NetzwerkBuffer[spacepos];					//Speichern der detetektierten Zeichen, da Leerzeichen überspringt werden, löschen sich diese heraus
	}	
	for (int spacepos = 0; spacepos < BauteilBuffer.size(); spacepos++) {		//Durchlaufen des Strings und bei Detektion eines Leerzeichen überspringen dieses 
		if (isspace(BauteilBuffer[spacepos])) {
			spacepos++;
			if (spacepos == BauteilBuffer.size()) break;
		}
		SpaceLess[1] = SpaceLess[1] + BauteilBuffer[spacepos];					//Speichern der detetektierten Zeichen, da Leerzeichen überspringt werden, löschen sich diese heraus
	}
	//cout << SpaceLess << "\n";

	NetworkToken = SpaceLess[0];												//Netzwerk String ohne Space zwischenspeichern
	BauteilToken = SpaceLess[1];												//Bauteil String ohne Space zwischenspeichern

	return;
}
string BauteilInput() {
	/*********************************************************
	Funktion zum Abfragen des Bauteile-Strings in Form von
	"R1:R(a, d); C2:C(d, b); L4:L(b, c);"
	Funktion erfasst den String und entfernt alle Leerzeichen
	und gibt das Ergebnis als Return-Wert aus.
	***********************************************************/

	string InputBuffer;
	string SpaceLess = "";
	string separator = ";\n";
	string Token;

	int endpos = 0;
	int startpos, spacepos = 0;
	int wordCount = 1;

	cout << "Enter String in Format: R1:R(a, d); C2:C(d, b); L4:L(b, c);\n" << "Bauteile:\t";			// Disabled for Testing
	getline(cin, InputBuffer);

	if (InputBuffer == "") {
		InputBuffer = "R1:R(a, d); C2:C(d, b); L4:L(b, c);";											// For Testing only 
		cout << "Default: " << InputBuffer << endl << endl;
	}
	else cout << "Registered String:\t" << InputBuffer << endl << endl;

	for (spacepos; spacepos < InputBuffer.size(); spacepos++) {
		if (isspace(InputBuffer[spacepos])) {
			spacepos++;
			if (spacepos == InputBuffer.size()) break;
		}
		SpaceLess = SpaceLess + InputBuffer[spacepos];
	}
	//cout << SpaceLess << "\n";

	return SpaceLess;
}
int TokenNetwork(string Token) {
	/*********************************************************
	Funktion zum Bearbeiten des von Leerzeichen befreiten
	Netzwerk-Strings.
	Funktion bearbeitet den String, extrahiert alle Pins, welche
	angegeben sind und erstellt ein Netzwerk entsprechend
	der Network Class und ensprechend ihrer Zuordnungen
	***********************************************************/

	int startPos = 5, endPos = 0;
	int CommaPosCheck;
	string separator = ";";
	string subToken[3];
	vector<string> Internals;

	if (Token.find("IN") == Token.npos)return -1;						//Überprüfen, ob sich "IN" im String befindet falls nicht return -1;
	startPos = Token.find_first_not_of(separator, startPos);			//Suche des ersten Zeichens, welches kein ; ist, setze Position als Startwert 
	endPos = Token.find_first_of(':', startPos);						//Suche des ersten : welches die Abtrennung des Pin Namens und Art darstellt, setze Position als Endwert
	subToken[0] = Token.substr(startPos, endPos - startPos);			//Entnehmen String zwischen Start und Ende, welches hier den Pin Namen darstellt

	if (Token.find("Out") == Token.npos)return -1;
	startPos = Token.find_first_of(';', endPos);
	startPos = Token.find_first_not_of(separator, startPos);
	endPos = Token.find_first_of(':', startPos);
	subToken[1] = Token.substr(startPos, endPos - startPos);

	if (Token.find("CMN") == Token.npos)return -1;
	startPos = Token.find_first_of(';', endPos);
	startPos = Token.find_first_not_of(separator, startPos);
	endPos = Token.find_first_of(':', startPos);
	subToken[2] = Token.substr(startPos, endPos - startPos);

	if (Token.find("Internal") == Token.npos)return -1;
	startPos = Token.find_first_of(';', endPos);
	startPos = Token.find_first_not_of(separator, startPos);
	endPos = Token.find_first_of(':', startPos);
	while (startPos != endPos) {

		CommaPosCheck = Token.find_first_of(',', startPos);												//Da bei den Internal Pins mehrere Pins vorhanden sein können, müssen hier die Namen getrennt werden
		if (CommaPosCheck == Token.npos) CommaPosCheck = Token.find_first_of(':', startPos);			//Falls keine "," mehr gefunden werden können sind alle Pins gefunden werden und ":" muss als Ende gesehen werden
		Internals.push_back(Token.substr(startPos, CommaPosCheck - startPos));							//Extrahieren des einzelnen Pins
		startPos = Token.find_first_not_of(',', CommaPosCheck);											//Starten beim nächsten Pin
	}

	Netzwerk = Network(subToken[0], subToken[1], subToken[2], Internals);								//Erstellen eines neuen Netzwerks ensprechend unserer Network Class
	return 0;
}
void TokenBauteil(string Token) {
	/*********************************************************
	Funktion zum Bearbeiten des von Leerzeichen befreiten
	Bauteile-Strings.
	Funktion bearbeitet den String und erstellt eine Bauteil Class
	in welcher die verschiedenen Bauteile anhand ihres Namens, Art,
	und Pins gespeichert werden
	***********************************************************/

	int startPos = 0, endPos = 0;
	char separator;
	string subToken[4];

	while (startPos != Token.npos) {

		separator = ':';												//Extrahieren des Namens, welcher bis zu dem Separator ":" gekennzeichnet wird
		endPos = Token.find_first_of(separator, startPos);
		subToken[0] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_not_of(separator, endPos);

		separator = '(';												//Extrahieren der Art, welche bis zu dem Separator "(" gekennzeichnet wird
		endPos = Token.find_first_of(separator, startPos);
		subToken[1] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_not_of(separator, endPos);

		separator = ',';												//Extrahieren des 1. Pins, welcher bis zu dem Separator "," gekennzeichnet wird
		endPos = Token.find_first_of(separator, startPos);
		subToken[2] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_not_of(separator, endPos);

		separator = ')';												//Extrahieren des 2. Pins, welcher bis zu dem Separator ")" gekennzeichnet wird
		endPos = Token.find_first_of(separator, startPos);		
		subToken[3] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_of(';', endPos);
		startPos = Token.find_first_not_of(';', startPos);

		Bauteile.push_back(new Bauteil(subToken[0], subToken[1], subToken[2], subToken[3]));		//Erstellen einer neuen Bauteile Class mit extrahierter Information
	}
}

void print_network() {
	for (unsigned int i = 0; i < Bauteile.size(); i++) {
		std::cout << Bauteile.at(i)->Name << ":" << Bauteile.at(i)->Art << "(" << Bauteile.at(i)->Pin1 << ":" << Bauteile.at(i)->Pin2 << ")" << "\t";
	}
}

void Restart()
{
	int b, h, x, y;

	get_drawarea(&b, &h);

	textbox(b - 120, h - 40, b - 5, h - 5, 18, BLUE, GREY, GREY, SINGLE_LINE | VCENTER_ALIGN | CENTER_ALIGN, ("Restart"));
	updatescr();

	while (
		!((mouseclick(&x, &y) == 1) &&
		((x > b - 120) && (x < b - 5)) &&
			((y > h - 40) && (y < h - 5))
			)) {
		printf(".");
		if (StopProcess())break;
	};
	GraphicOutput.clear();
	Bauteile.clear();
	
	printf("######################################\n\n");
	clrscr();
	printf("######################################\n\n");
}

void user_main()
{
	int ww, hh;
	set_windowpos(0, 0, 1200, 600);

	while (1) {								// Endlosschleife

		get_windowsize(&ww, &hh);
		set_drawarea(ww, hh);				// Setzen des Zeichenbereiches     
		clrscr();

		if (1) {

			SetConsoleWindowTop();
			Sleep(1000);

			NetworkInput();			//Getting rid of spaces and cutting in blocks separated by ;
			//BauteilToken = BauteilInput();

			TokenNetwork(NetworkToken);				//Aufbau von Netzwerk und Bauteilen
			TokenBauteil(BauteilToken);

			THE_ALGORITHM();						//Ausführen des Zusammenfassungsalgorithmus

			print_network();
		}

		SetGraphicWindowTop();

		int height = 80;
		int width = 10;

		if (Bauteile.size() >= 2 && InputImpedanz() != NULL) {

			string Nenner = CMNImpedanz();
			Nenner.append(" + ");
			Nenner.append(InputImpedanz());
			char* NennerP = new char[Nenner.size() + 1];
			strcpy(NennerP, Nenner.c_str());

			text(0, 35, 30, BLACK, "G=");
			text(35, 25, 25, BLACK, CMNImpedanz());
			line(30, 50, 11*Nenner.size(), 50, BLACK);
			text(35, 50, 25, BLACK, NennerP);
		}
		else {
			text(0, 35, 30, BLACK, "G=");
			text(35, 35, 30, BLACK, "1");
		}

		for (int i = 0; i < GraphicOutput.size(); i++) {
			text(width, height, 15, BLACK, GraphicOutput.at(i));
			height += 15;
			if (height > hh-20) {
				height = 80;
				width += 300;
			}
		}

		Restart();					//Den "Restart"-Button malen und auf eine Aktivierung warten.
		if(StopProcess())break;

	}
}