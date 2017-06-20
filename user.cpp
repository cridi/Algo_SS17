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

#define _pi					3.1415926535				// Die Zahl Pi.
#define _180_durch_pi		57.2957795147				// = 180 * pi. Dient zur Laufzeitoptinierung.
#define _sinus(winkel)		sin((winkel) / _180_durch_pi)	// Funktion im Gradmass.
#define _cosinus(winkel)	cos((winkel) / _180_durch_pi)	// Funktion im Gradmass.

COLORREF Colref[] = { BLACK,RED,GREEN,BLUE,YELLOW,BROWN };
int Colind = 0;

//-------------------------------------------------------------------------------------------------------//
const int IDENTIFIER = 4;
const int INTEGER1 = 5;
const int TOKENSTART = 300;

class CParser {
public:
	map<string, int>TokenTable;
	map<int, string>ReverseTokenTable;

	void CParser::init_TokenTable();						//loads the tokens
	void CParser::newTokenEntry(string str, int index);		//load one token
};
void CParser::newTokenEntry(string str, int index) {
	TokenTable[str] = index;
	ReverseTokenTable[index] = str;
}

void CParser::init_TokenTable() {
	newTokenEntry("IDENTIFIER", 4);
	newTokenEntry("INTEGER1", 5);
	int ii = TOKENSTART;
	newTokenEntry("Nets", ii++);
	newTokenEntry("IN", ii++);
	newTokenEntry("OUT", ii++);
	newTokenEntry("CMN", ii++);
	newTokenEntry("INTERNAL", ii++);
	//newTokenEntry("R", ii++);
	//newTokenEntry("C", ii++);
	//newTokenEntry("L", ii++);
}

//USER PROGRAMM
string BauteilToken, NetworkToken;

class Network {
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
public:
	string Name;
	string Art;
	string Pin1;
	string Pin2;

	Bauteil(string Name, string Art, string Pin1, string Pin2);
	string getName();
	string getArt();
	string getPin1();
	string getPin2();
};

Bauteil::Bauteil(string a, string b, string c, string d) {
	Name = a;
	Art = b;
	Pin1 = c;
	Pin2 = d;
}
string Bauteil::getName() {
	return Name;
}
string Bauteil::getArt() {
	return Art;
}
string Bauteil::getPin1() {
	return Pin1;
}
string Bauteil::getPin2() {
	return Pin1;
}
Network Netzwerk;
vector<Bauteil*> Bauteile;
vector<Bauteil*> serial_Bauteile;

void Is_parallel() {
	int outerSweep = Bauteile.size();
	int innerSweep = Bauteile.size();

	for (int i = 0; i < outerSweep; i++) {
		for (int ii = 0; ii < innerSweep; ii++) {

			if (i == ii) {
				ii++;
				if (ii == innerSweep)break;
			}

			if (((Bauteile.at(i)->Pin2 == Bauteile.at(ii)->Pin1) && (Bauteile.at(i)->Pin1 == Bauteile.at(ii)->Pin2)) ||
				((Bauteile.at(i)->Pin2 == Bauteile.at(ii)->Pin2) && (Bauteile.at(ii)->Pin1 == Bauteile.at(i)->Pin1))) {			//Detection is parallel?

						Bauteile.push_back(new Bauteil(Bauteile.at(i)->Name + "||" + Bauteile.at(ii)->Name,
							Bauteile.at(i)->Art, Bauteile.at(i)->Pin1, Bauteile.at(i)->Pin2));				//New Element with outer Pins and new Name
						cout << "Zusammenfassung: " << Bauteile.at(i)->Name << "||" << Bauteile.at(ii)->Name << endl;

					if (i < ii) {
						Bauteile.erase(Bauteile.begin() + i);													//Deleting obsolete Elements
						Bauteile.erase(Bauteile.begin() + ii - 1);
					}
					else {
						Bauteile.erase(Bauteile.begin() + ii);													//Deleting obsolete Elements
						Bauteile.erase(Bauteile.begin() + i - 1);
					}

					//		leftSweep = floor(Bauteile.size() / 2);
					//	rightSweep = ceil(Bauteile.size() / 2);
					//	ii = Bauteile.size() - 1;
					return;
			}
		}
	}

}

bool Is_serial_Pin(string Pin_to_check) {
	int checksum = 0;

	for (int i = 0; i < Netzwerk.INTERNALS.size(); i++) {

		if (!Pin_to_check.compare(Netzwerk.INTERNALS.at(i))) {		//Check if IN OUT or CMN

			for (unsigned int i = 0; i < Bauteile.size(); i++) {
				if (!((Bauteile.at(i)->Pin1).compare(Pin_to_check))) { checksum++; };												//Check if only two times connected
				if (!((Bauteile.at(i)->Pin2).compare(Pin_to_check))) { checksum++; };												//Then it ist realy serial
			}
		}
	}

	if (checksum != 2) return false;
	if (checksum == 2) return true;
}

void find_serial_Bauteil(string serial_Pin) {
	for (unsigned int i = 0; i < Bauteile.size(); i++) {
		if ((!((Bauteile.at(i)->Pin1).compare(serial_Pin))) || (!((Bauteile.at(i)->Pin2).compare(serial_Pin)))) {
			serial_Bauteile.push_back(Bauteile.at(i));
			break;
		}
	}
}

void reverse_find_serial_Bauteil(string serial_Pin) {
	for (unsigned int i = Bauteile.size() - 1; i >= 0; i--) {
		if ((!((Bauteile.at(i)->Pin1).compare(serial_Pin))) || (!((Bauteile.at(i)->Pin2).compare(serial_Pin)))) {
			serial_Bauteile.push_back(Bauteile.at(i));
			break;
		}
	}
}

void find_outer_Pins(string serial_Pin) {
	if ((serial_Pin.compare(serial_Bauteile.at(0)->Pin2))) {
		(serial_Bauteile.at(0)->Pin1) = (serial_Bauteile.at(0)->Pin2);
	}
	if ((serial_Pin.compare(serial_Bauteile.at(1)->Pin2))) {
		(serial_Bauteile.at(1)->Pin1) = (serial_Bauteile.at(1)->Pin2);
	}
}

void create_new_serial_Bauteil() {

			Bauteile.push_back(new Bauteil("(" + serial_Bauteile.at(0)->Name + " + " + serial_Bauteile.at(1)->Name + ")",
											serial_Bauteile.at(1)->Art, serial_Bauteile.at(0)->Pin1, serial_Bauteile.at(1)->Pin1));				//New Element with outer Pins and new Name
			cout << "Zusammenfassung: " << serial_Bauteile.at(0)->Name << "+" << serial_Bauteile.at(1)->Name << endl;

		for (int i = 0; i < Bauteile.size(); i++) {
			if (!(serial_Bauteile.at(0)->Name.compare(Bauteile.at(i)->Name)) || (!(serial_Bauteile.at(1)->Name.compare(Bauteile.at(i)->Name)))) {
				Bauteile.erase(Bauteile.begin() + i);
				i = -1;
			}
		}

		serial_Bauteile.erase(serial_Bauteile.begin() + 1);
		serial_Bauteile.erase(serial_Bauteile.begin() + 0);

}


void Is_serial() {

	vector<string*> serial_Pins;
int c = 0;
int PinFlag = 0;

for (int i = 0; i < Bauteile.size(); i++) {															//Zuordnung äußere Pins, gemeinsamer Pin;
	PinFlag = 0;
	if (Is_serial_Pin(Bauteile.at(i)->Pin1)) {

		for (int ii = 0; ii < serial_Pins.size(); ii++) {
			if (!(serial_Pins.at(ii)->compare(Bauteile.at(i)->Pin1))) PinFlag = 1;
		}

		if (PinFlag == 0) {
			serial_Pins.push_back(new string(Bauteile.at(i)->Pin1));
		}
	}
	else if (Is_serial_Pin(Bauteile.at(i)->Pin2)) {

		for (int ii = 0; ii < serial_Pins.size(); ii++) {
			if (!(serial_Pins.at(ii)->compare(Bauteile.at(i)->Pin2))) PinFlag = 1;
		}
		if (PinFlag == 0) {
			serial_Pins.push_back(new string(Bauteile.at(i)->Pin2));
		}
	}
}
for (int i = 0; i < serial_Pins.size(); i++) {
	find_serial_Bauteil(*serial_Pins.at(i));
	reverse_find_serial_Bauteil(*serial_Pins.at(i));
	find_outer_Pins(*serial_Pins.at(i));
	create_new_serial_Bauteil();
}

}

vector<Bauteil*> SternFind(vector<vector<Bauteil*>> AdjazenzMatrix, string Pins) {
	vector<Bauteil*> SternBauteile;

	int SternCounter = 0;
	int rows = Pins.size();
	int cols = Pins.size();

	for (int outer = 0; outer < rows; outer++) {
		for (int i = 0; i < rows; i++) {
			if (AdjazenzMatrix[i][outer] != NULL) {
				SternCounter++;
				SternBauteile.push_back(AdjazenzMatrix[i][outer]);
			}
			if (AdjazenzMatrix[outer][i] != NULL) {
				SternCounter++;
				SternBauteile.push_back(AdjazenzMatrix[outer][i]);
			}
		}

		if (SternCounter == 3) {
			if ((Pins.substr(outer, 1) != Netzwerk.CMN) &&
				(Pins.substr(outer, 1) != Netzwerk.OUTPUT) &&
				(Pins.substr(outer, 1) != Netzwerk.INPUT)) return SternBauteile;
		}
		SternCounter = 0;
		SternBauteile.clear();
	}
}

void SternAdjazenz() {
	vector<Bauteil*> SternBauteile;

	string Pins = "";
	for (int i = 0; i < Bauteile.size(); i++) {

		if (Pins.find(Bauteile.at(i)->Pin1) == Pins.npos) {
			Pins = Pins + Bauteile.at(i)->Pin1;
		}
		if (Pins.find(Bauteile.at(i)->Pin2) == Pins.npos) {
			Pins = Pins + Bauteile.at(i)->Pin2;
		}
	}
	int rows = Pins.size();
	int cols = Pins.size();

	vector<vector<Bauteil*>> AdjazenzMatrix;
	AdjazenzMatrix.resize(rows);
	for (int i = 0; i < rows; i++) {
		AdjazenzMatrix[i].resize(cols);
	}

	for (int i = 0; i < Bauteile.size(); i++) {

		AdjazenzMatrix[Pins.find(Bauteile.at(i)->Pin1)][Pins.find(Bauteile.at(i)->Pin2)] = Bauteile.at(i);
	}

	SternBauteile = SternFind(AdjazenzMatrix, Pins);

	string SternPin;

	if (SternBauteile.at(0)->Pin1 == SternBauteile.at(1)->Pin1)SternPin = SternBauteile.at(0)->Pin1;
	else if (SternBauteile.at(0)->Pin1 == SternBauteile.at(1)->Pin2)SternPin = SternBauteile.at(0)->Pin1;
	else SternPin = SternBauteile.at(0)->Pin2;

	Pins.erase(Pins.find(SternPin), 1);

	string* ValueBauteile = new string[Pins.size()];

	for (int i = 0; i < SternBauteile.size(); i++) {
		for (int ii = 0; ii < Pins.size(); ii++) {
			if (SternBauteile.at(i)->Pin1 == Pins.substr(ii, 1)) ValueBauteile[ii] = SternBauteile.at(i)->Name;
			else if (SternBauteile.at(i)->Pin2 == Pins.substr(ii, 1))ValueBauteile[ii] = SternBauteile.at(i)->Name;
		}
	}


	Bauteile.push_back(new Bauteil("ZD" + Pins.substr(0, 1) + Pins.substr(1, 1), "Z",
		Pins.substr(0, 1), Pins.substr(1, 1)));
	cout << "Zusammenfassung: " << "ZD" + Pins.substr(0, 1) + Pins.substr(1, 1) <<
		" = " << ValueBauteile[0] << " + " << ValueBauteile[1] << " + " <<
		"(" << ValueBauteile[0] << "*" << ValueBauteile[1] << ")" << "/" << ValueBauteile[2] << endl;

	Bauteile.push_back(new Bauteil("ZD" + Pins.substr(0, 1) + Pins.substr(2, 1), "Z",
		Pins.substr(0, 1), Pins.substr(2, 1)));
	cout << "Zusammenfassung: " << "ZD" + Pins.substr(0, 1) + Pins.substr(2, 1) <<
		" = " << ValueBauteile[0] << " + " << ValueBauteile[2] << " + " <<
		"(" << ValueBauteile[0] << "*" << ValueBauteile[2] << ")" << "/" << ValueBauteile[1] << endl;

	Bauteile.push_back(new Bauteil("ZD" + Pins.substr(1, 1) + Pins.substr(2, 1), "Z",
		Pins.substr(1, 1), Pins.substr(2, 1)));
	cout << "Zusammenfassung: " << "ZD" + Pins.substr(1, 1) + Pins.substr(2, 1) <<
		" = " << ValueBauteile[1] << " + " << ValueBauteile[2] << " + " <<
		"(" << ValueBauteile[1] << "*" << ValueBauteile[2] << ")" << "/" << ValueBauteile[0] << endl;

	int Size = Bauteile.size();
	for (int i = 0; i < Size; i++) {
		if (Bauteile.at(i)->Name == SternBauteile.at(0)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		else if (Bauteile.at(i)->Name == SternBauteile.at(1)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		else if (Bauteile.at(i)->Name == SternBauteile.at(2)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		Size = Bauteile.size();
	}
}
bool DreieckAdjazenz() {
	vector<Bauteil*> DreieckBauteile;

	string Pins = "";
	for (int i = 0; i < Bauteile.size(); i++) {

		if (Pins.find(Bauteile.at(i)->Pin1) == Pins.npos) {
			Pins = Pins + Bauteile.at(i)->Pin1;
		}
		if (Pins.find(Bauteile.at(i)->Pin2) == Pins.npos) {
			Pins = Pins + Bauteile.at(i)->Pin2;
		}
	}

	if (Pins.size() != 3)return false;

	DreieckBauteile = Bauteile;

	vector<vector<string>> ValueBauteile;
	ValueBauteile.resize(Pins.size());

	for (int i = 0; i < DreieckBauteile.size(); i++) {
		for (int ii = 0; ii < Pins.size(); ii++) {
			if (DreieckBauteile.at(i)->Pin1 == Pins.substr(ii, 1)) ValueBauteile[ii].push_back(DreieckBauteile.at(i)->Name);
			else if (DreieckBauteile.at(i)->Pin2 == Pins.substr(ii, 1))ValueBauteile[ii].push_back(DreieckBauteile.at(i)->Name);
		}
	}

			Bauteile.push_back(new Bauteil("Z*" + Pins.substr(0, 1), "Z",
											Pins.substr(0, 1), "*"));
			cout << "Zusammenfassung: " << "Z*" + Pins.substr(0, 1) <<
											" = " << "(" << ValueBauteile[0][0] << "*" << ValueBauteile[0][1] << ")" << "/" <<
											"(" << DreieckBauteile.at(0)->Name << " + " << DreieckBauteile.at(1)->Name << 
											" + " << DreieckBauteile.at(2)->Name << ")" << endl;

			Bauteile.push_back(new Bauteil("Z*" + Pins.substr(1, 1), "Z",
											Pins.substr(1, 1), "*"));
			cout << "Zusammenfassung: " << "Z*" + Pins.substr(1, 1) <<
											" = " << "(" << ValueBauteile[1][0] << "*" << ValueBauteile[1][1] << ")" << "/" <<
											"(" << DreieckBauteile.at(0)->Name << " + " << DreieckBauteile.at(1)->Name << 
											" + " << DreieckBauteile.at(2)->Name << ")" << endl;

			Bauteile.push_back(new Bauteil("Z*" + Pins.substr(2, 1), "Z",
											Pins.substr(2, 1), "*"));
			cout << "Zusammenfassung: " << "Z*" + Pins.substr(2, 1) <<
											" = " << "(" << ValueBauteile[2][0] << "*" << ValueBauteile[2][1] << ")" << "/" <<
											"(" << DreieckBauteile.at(0)->Name << " + " << DreieckBauteile.at(1)->Name << 
											" + " << DreieckBauteile.at(2)->Name << ")" << endl;

	int Size = Bauteile.size();
	for (int i = 0; i < Size; i++) {
		if (Bauteile.at(i)->Name == DreieckBauteile.at(0)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		else if (Bauteile.at(i)->Name == DreieckBauteile.at(1)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		else if (Bauteile.at(i)->Name == DreieckBauteile.at(2)->Name) {
			Bauteile.erase(Bauteile.begin() + i);
			i = -1;
		}
		Size = Bauteile.size();
	}
	return true;
}



void THE_ALGORITHM() {
	int serialLimit = Bauteile.size();
	int parallelLimit = Bauteile.size();

	for (int i = 0; i < 5; i++) {

		for (unsigned int i = 0; i < serialLimit; i++) {
			Is_serial();
			serialLimit = Bauteile.size();
		}

		for (unsigned int i = 0; i < parallelLimit; i++) {
			Is_parallel();
			parallelLimit = Bauteile.size();
		}

		if (Bauteile.size() == 3) {
			if (DreieckAdjazenz())return;
		}
		else if (Bauteile.size() < 3)return;

		SternAdjazenz();
	}
}


//USER FUNCTIONS
string NetworkInput() {
	string InputBuffer;
	string SpaceLess = "";
	string separator = ";\n";
	string Token;

	int endpos = 0;
	int startpos, spacepos = 0;
	int wordCount = 1;

	//cout << "Enter String in Format: a:IN; b:Out; c: CMN; d,e: Internal;\n" << "Nets:\t";		//Disabled for Testing
	//getline(cin, InputBuffer);
	//cout << "Registered String:\t" << InputBuffer << endl;

	InputBuffer = "a:IN; b:Out; c: CMN; d,e: Internal;";			// For Testing only

	for (spacepos; spacepos < InputBuffer.size(); spacepos++) {
		if (isspace(InputBuffer[spacepos])) {
			spacepos++;
			if (spacepos == InputBuffer.size()) break;
		}
		SpaceLess = SpaceLess + InputBuffer[spacepos];
	}
	std::cout << SpaceLess << "\n";

	//while (1){
	//	startpos = SpaceLess.find_first_not_of(separator, endpos);
	//	if (startpos == SpaceLess.npos)break;
	//	endpos = SpaceLess.find_first_of(separator, startpos);
	//	if (endpos == SpaceLess.npos)break;
	//	token = SpaceLess.substr(startpos, endpos - startpos);

	//	cout  << token << endl;
	//}

	return SpaceLess;
}
string BauteilInput() {
	string InputBuffer;
	string SpaceLess = "";
	string separator = ";\n";
	string Token;

	int endpos = 0;
	int startpos, spacepos = 0;
	int wordCount = 1;

	//cout << "Enter String in Format: R1:R(a, d); C2:C(d, b); L4:L(b, c)";			// Disabled for Testing
	//getline(cin, InputBuffer);
	//cout << "Registered String:\t" << InputBuffer << endl;

	InputBuffer = "R1:R(a, e); R2:R(e, d); R3:R(d, b); R4:R(e, b); L1:L(a, d); L2:L(e, c); C1:C(a,c); C2:C(e,b); ";			// For Testing only 
	for (spacepos; spacepos < InputBuffer.size(); spacepos++) {
		if (isspace(InputBuffer[spacepos])) {
			spacepos++;
			if (spacepos == InputBuffer.size()) break;
		}
		SpaceLess = SpaceLess + InputBuffer[spacepos];
	}
	cout << SpaceLess << "\n";

	//while (1){
	//	startpos = SpaceLess.find_first_not_of(separator, endpos);
	//	if (startpos == SpaceLess.npos)break;
	//	endpos = SpaceLess.find_first_of(separator, startpos);
	//	if (endpos == SpaceLess.npos)break;
	//	token = SpaceLess.substr(startpos, endpos - startpos);

	//	cout  << token << endl;
	//}

	return SpaceLess;
}
int TokenNetwork(string Token) {
	int startPos = 0, endPos = 0;
	int CommaPosCheck;
	string separator = ";";
	string subToken[3];
	vector<string> Internals;

	if (Token.find("IN") == Token.npos)return -1;
	startPos = Token.find_first_not_of(separator, startPos);
	endPos = Token.find_first_of(':', startPos);
	subToken[0] = Token.substr(startPos, endPos - startPos);

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

		CommaPosCheck = Token.find_first_of(',', startPos);
		if (CommaPosCheck == Token.npos) CommaPosCheck = Token.find_first_of(':', startPos);
		Internals.push_back(Token.substr(startPos, CommaPosCheck - startPos));
		startPos = Token.find_first_not_of(',', CommaPosCheck);
	}

	Netzwerk = Network(subToken[0], subToken[1], subToken[2], Internals);
	return 0;
}
void TokenBauteil(string Token) {
	int startPos = 0, endPos = 0;
	char separator;
	string subToken[4];

	while (startPos != Token.npos) {

		separator = ':';
		endPos = Token.find_first_of(separator, startPos);
		subToken[0] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_not_of(separator, endPos);

		separator = '(';
		endPos = Token.find_first_of(separator, startPos);
		subToken[1] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_not_of(separator, endPos);

		separator = ',';
		endPos = Token.find_first_of(separator, startPos);
		subToken[2] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_not_of(separator, endPos);

		separator = ')';
		endPos = Token.find_first_of(separator, startPos);
		subToken[3] = Token.substr(startPos, endPos - startPos);
		startPos = Token.find_first_of(';', endPos);
		startPos = Token.find_first_not_of(';', startPos);

		Bauteile.push_back(new Bauteil(subToken[0], subToken[1], subToken[2], subToken[3]));
	}
}

void print_network() {
	for (unsigned int i = 0; i < Bauteile.size(); i++) {
		std::cout << Bauteile.at(i)->Name << ":" << Bauteile.at(i)->Art << "(" << Bauteile.at(i)->Pin1 << ":" << Bauteile.at(i)->Pin2 << ")" << "\t";
	}
}


void user_main()
{
	int ww, hh;
	set_windowpos(0, 0, 600, 400);

	SetConsoleWindowTop();
	Sleep(1000);

	//while (1) {								// Endlosschleife
	get_windowsize(&ww, &hh);
	set_drawarea(ww, hh);				// Setzen des Zeichenbereiches     
	clrscr();

	NetworkToken = NetworkInput();			//Getting rid of spaces and cutting in blocks separated by ;
	BauteilToken = BauteilInput();

	//R1:R(a, b);
	TokenNetwork(NetworkToken);
	TokenBauteil(BauteilToken);

	THE_ALGORITHM();

	print_network();




	// Den "Restart"-Button malen und auf eine Aktivierung warten.
	//if(StopProcess())break;

		//}
}