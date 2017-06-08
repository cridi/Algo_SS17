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

COLORREF Colref[]={BLACK,RED,GREEN,BLUE,YELLOW,BROWN};
int Colind=0;

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
};
Bauteil::Bauteil(string a, string b, string c, string d){
	Name = a;
	Art = b;
	Pin1 = c;
	Pin2 = d;
}
Network Netzwerk;
vector<Bauteil*> Bauteile;

void Is_parallel(Bauteil A , Bauteil B, int Iterator_A, int Iterator_B) {

	if ((A.Pin2 == B.Pin1) && (A.Pin1 == B.Pin2) || (A.Pin1 == B.Pin1) && (B.Pin2 == A.Pin2)){			//Detection is parallel?

		Bauteile.push_back(new Bauteil(A.Name + "||" + B.Name , A.Art , A.Pin1, A.Pin2));				//New Element with outer Pins and new Name

		Bauteile.erase(Bauteile.begin() + Iterator_A);													//Deleting obsolete Elements
		Bauteile.erase(Bauteile.begin() + Iterator_B);
		Bauteile.shrink_to_fit();																		//Performance +
	}
}


bool Is_serial_Pin(string Pin_to_check) {
	int checksum;
	if ((Pin_to_check.compare("a")) || (Pin_to_check.compare("b")) || (Pin_to_check.compare("c"))) return false;		//Check if IN OUT or CMN

	for (unsigned int i = 0; i <= Bauteile.size(); i++) {
		if ((Bauteile.at(i)->Pin1).compare(Pin_to_check)) { checksum++; };												//Check if only two times connected
		if ((Bauteile.at(i)->Pin2).compare(Pin_to_check)) { checksum++; };												//Then it ist realy serial
	}

	if (checksum != 2) return false;
	if (checksum == 2) return true;
}


void Is_serial(Bauteil A, Bauteil B, int Iterator_A, int Iterator_B) {

	string Pins[] = { A.Pin1, B.Pin2, B.Pin2, A.Pin1 };

	string serial_Pin = 0;
	string outer_Pin[2];
	int c = 0;


	for (int i = 0; i < 4; i++ ){															//Zuordnung äußere Pins, gemeinsamer Pin;
		if (Is_serial_Pin(Pins[i])) {
			serial_Pin = Pins[i];
		}
		else  outer_Pin[c] = Pins[i]; c++;
	}

	if (!serial_Pin.empty()) { 


		Bauteile.push_back(new Bauteil(A.Name + " + " + B.Name, A.Art, outer_Pin[0], outer_Pin[1]));				//New Element with outer Pins and new Name

		Bauteile.erase(Bauteile.begin() + Iterator_A);													//Deleting obsolete Elements
		Bauteile.erase(Bauteile.begin() + Iterator_B);
		Bauteile.shrink_to_fit();																		//Performance +

	}
}

void THE_ALGORITHM() {
	for (unsigned int i = 0; i <= 3 * Bauteile.size(); i++) {
		Is_serial(*Bauteile.at(i), *Bauteile.at(i + 1), i, i + 1);
		Is_parallel(*Bauteile.at(i), *Bauteile.at(i + 1), i, i + 1);
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

	InputBuffer = "a:IN; b:Out; c: CMN; d,e: Internal;" ;			// For Testing only

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

	InputBuffer = "R1:R(a, d); R2:R(a, d); R3:R(d, e); R4:R(e, b)";			// For Testing only 
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

	if(Token.find("IN")==Token.npos)return -1;
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

	while(startPos != Token.npos) {

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
	int ww,hh;
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