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


struct _BAUM {							// Die fuenf Eingangsparameter des Baumes.
	int		Tiefe;
	int		Neigung_links;
	float	Wachstum_links;
	int		Neigung_rechts;
	float	Wachstum_rechts;
	void WerteKorrigieren()
	{
		if (Wachstum_links > 1) Wachstum_links = 1 / Wachstum_links;
		if (Wachstum_rechts > 1) Wachstum_rechts = 1 / Wachstum_rechts;
		Neigung_rechts = -Neigung_rechts;
	};
} baum;


void Zeichne_Ast(int x, int y, float n, float Tiefe, float Laenge)
{
	int x_rel;
	int y_rel;
	if(StopProcess())return;
	if (Tiefe > 1) {										// Stopbedingung fuer die Rekursion.
		// Zeichnen des linken Astes.
		x_rel = (int)(Laenge * _sinus(n + baum.Neigung_links));	// Berechnen der x-Koordinate.
		y_rel = (int)(Laenge * _cosinus(n + baum.Neigung_links));	// Berechnen der y-Koordinate.
		COLORREF cref=Colref[Colind++]; if(Colind>5)Colind=0;
		lineto(x-x_rel, y-y_rel, cref);					// Zeichnen
		// Rekursion!! Zeichnen des nachfolgenden Astes.
		//Sleep(5);updatescr();
		Zeichne_Ast(x-x_rel, y-y_rel, n+baum.Neigung_links, Tiefe-1, Laenge*baum.Wachstum_links);
		moveto(x,y);										// Cursor zur Anfangsposition zuruecksetzen.

		// Zeichnen des rechten Astes.
		x_rel = (int)(Laenge * _sinus(n + baum.Neigung_rechts));
		y_rel = (int)(Laenge * _cosinus(n + baum.Neigung_rechts));
		cref=Colref[Colind++];if(Colind>5)Colind=0;
		lineto(x-x_rel, y-y_rel, cref);
		//Sleep(5);updatescr();
		Zeichne_Ast(x-x_rel, y-y_rel, n+baum.Neigung_rechts, Tiefe-1, Laenge*baum.Wachstum_rechts);
	}

}

void Zeichne_Baum()
{
	int Weite, Hoehe, x, y, StammLaenge;

	// Berechnen des Gesamtwachstums.
	float GesamtWachstum = 1;
	float Wachstum = (baum.Wachstum_links > baum.Wachstum_rechts) ? baum.Wachstum_links : baum.Wachstum_rechts;
	float WT = 1;
	for (int i=1; i<baum.Tiefe; i++) {
		WT *= Wachstum;
		GesamtWachstum += WT;
	}

	// Berechnung des Fusspunktes des Baumes
	get_windowsize(&Weite, &Hoehe);
	x = Weite / 2;
	y = Hoehe*3/4;
	text(x, y, 18, RED, "Wurzel [x,y]=[%d,%d]", x,y);

	StammLaenge = (int)(y / GesamtWachstum);			// Laenge des Stammes.

	moveto(x, y);									// Marker auf die Wurzel setzen.
	lineto(x, y - StammLaenge, BLACK);				// Stamm zeichnen
	Zeichne_Ast(x, y - StammLaenge, 0, (float)baum.Tiefe, (float)(StammLaenge*(baum.Wachstum_links + baum.Wachstum_rechts)/2));
}

void Restart()
{
	int b, h, x, y;

	get_drawarea(&b, &h);

	textbox(b - 120, h - 40, b - 5, h - 5, 18, BLUE, GREY, GREY, SINGLE_LINE | VCENTER_ALIGN | CENTER_ALIGN, ("Restart"));
	updatescr();

	while ( 
		!((mouseclick(&x,&y) == 1) &&
		  ((x > b-120) && (x < b-5)) &&
		  ((y > h-40)  &&	(y < h-5))
		 )) {
		printf(".");
		if(StopProcess())break;
	};

	printf("######################################\n\n");
	clrscr();
	printf("######################################\n\n");
}

//USER PROGRAMM
string BauteilToken, NetworkToken;

class Network {
	string INPUT;
	string OUTPUT;
	string CMN;
	vector<string> INTERNALS;
public:
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
	string Name;
	string Art;
	string Pin1;
	string Pin2;
public:
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
 
//USER FUNCTIONS
string NetworkInput() {
	string InputBuffer;
	string SpaceLess = "";
	string separator = ";\n";
	string Token;

	int endpos = 0;
	int startpos, spacepos = 0;
	int wordCount = 1;

	//cout << "Enter String in Format: a:IN; b:Out; c: CMN; d,e: Internal;\n" << "Nets:\t";
	//getline(cin, InputBuffer);
	//cout << "Registered String:\t" << InputBuffer << endl;
	InputBuffer = "a:IN; b:Out; c: CMN; d,e: Internal;";

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
string BauteilInput() {
	string InputBuffer;
	string SpaceLess = "";
	string separator = ";\n";
	string Token;

	int endpos = 0;
	int startpos, spacepos = 0;
	int wordCount = 1;

	//cout << "Enter String in Format: R1:R(a, d); C2:C(d, b); L4:L(b, c)";
	//getline(cin, InputBuffer);
	//cout << "Registered String:\t" << InputBuffer << endl;
	InputBuffer = "R1:R(a, d); C2:C(d, b); L4:L(b, c);";

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


void user_main()
{
	int ww,hh;
	set_windowpos(0, 0, 600, 400);

	SetConsoleWindowTop();
	Sleep(1000);

	while (1) {								// Endlosschleife
		get_windowsize(&ww, &hh);
		set_drawarea(ww, hh);				// Setzen des Zeichenbereiches     
		clrscr();

		NetworkToken = NetworkInput();			//Getting rid of spaces and cutting in blocks separated by ;
		//BauteilToken = BauteilInput();

	//R1:R(a, b);
		TokenNetwork(NetworkToken);
		//TokenBauteil(BauteilToken);

		Restart();							// Den "Restart"-Button malen und auf eine Aktivierung warten.
		if(StopProcess())break;

	}
}
