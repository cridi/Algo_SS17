// User.cpp : header file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "user.h"
#include "graphics\graphicfunctions.h"

#include <string>
#include <iostream>
#include <fstream>

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

class _NET {
public:
	char INPUT;
	char OUTPUT;
	char CMN;
	char INTERNAL;
}net;

class _RESISTOR {
public:
	char name;
	int value;
	char a_pin;
	char b_pin;
};
class _INDUCTOR {
public:
	char name;
	int value;
	char a_pin;
	char b_pin;
};
class _CONDENSATOR {
public:
	char name;
	int value;
	char a_pin;
	char b_pin;
};

string NetworkBuffer;

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


void user_main()
{
	int ww,hh;
	set_windowpos(0, 0, 600, 400);

	//USER PROGRAMM
	cout << "Enter Network!\n";
	getline(cin, NetworkBuffer);
	cout << "Registered String\n" << NetworkBuffer << endl;
	
	string seperator = ": \t;,";
	string token;
	int endpos = 0;
	int startpos = 0;
	int wordCount = 1;

	while (1) {								// Endlosschleife
		get_windowsize(&ww, &hh);
		set_drawarea(ww, hh);				// Setzen des Zeichenbereiches     
		clrscr();

		startpos = NetworkBuffer.find_first_not_of(seperator, endpos);
		if (startpos == NetworkBuffer.npos)break;
		endpos = NetworkBuffer.find_first_of(seperator, startpos);
		if (endpos == NetworkBuffer.npos)break;
		token = NetworkBuffer.substr(startpos, endpos - startpos);

		if (wordCount == 1) {
			if (token.compare("Nets")&token.compare("nets")) {
				cout << "Syntax Error: Unexpected Input Format.";
				break;
			}
		}

		wordCount++;
		cout << token << endl;

		//Restart();						// Den "Restart"-Button malen und auf eine Aktivierung warten.
		if(StopProcess())break;
		
	}
}