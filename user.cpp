// User.cpp : header file
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "user.h"
#include "graphics\graphicfunctions.h"

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
	text(x, y, 18, RED, "Wurzel [x,y]=[%d,%d", x,y);

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

struct _System {
	
} System;


void user_main()
{
	int ww,hh;
	set_windowpos(0,0,600,400);
	while (1) {								// Endlosschleife
		get_windowsize(&ww, &hh);
		set_drawarea(ww, hh);				// Setzen des Zeichenbereiches
		clrscr();
		if (0) {//Eingeben der Generierungsparameter des Baumes
			cout << "Startvorschlag Tiefe:10\nNeigung links:15,Wachstum links:0.7\nNeigung rechts: 18,Wachstum rechts:0.8\n";
			cout << "Tiefe ( >1 ): ";
			cin >> baum.Tiefe;		// Die Tiefe einlesen.

			cout << "Neigung nach links in Grad: ";	// Die Neigung links einlesen.
			cin >> baum.Neigung_links;

			cout << "Wachstum nach links ( <1 ): ";// Das Wachstum links einlesen...
			cin >> baum.Wachstum_links;

			cout << "Neigung nach rechts in Grad: ";
			cin >> baum.Neigung_rechts;

			cout << "Wachstum nach rechts ( <1 ): ";
			cin >> baum.Wachstum_rechts;
			baum.WerteKorrigieren();		// Die Werte fuer das Wachstum ueberpruefen
											// und wennn notwendig korrigieren.
			Zeichne_Baum();					// Den Baum zeichnen.
			cout << "Baum gezeichnet\n";

		}
		else {//Vermeidet das Eingeben der Daten. Das ist sinnvoll beim Testen

			SetConsoleWindowTop();


			//Testausgaben für Texte
			text(100, 100, 20, BLACK, "jwC");
			
			line(90, 130, 210, 130, 1);

			text(100, 140, 20, BLACK, "R + jwC");


			SetGraphicWindowTop();

		}


		Restart();						// Den "Restart"-Button malen und auf eine Aktivierung warten.
		if(StopProcess())break;
		
	}
}

/*void user_main()
{
int ww,hh;
set_windowpos(0,0,600,400);
while (1) {								// Endlosschleife
get_windowsize(&ww, &hh);
set_drawarea(ww, hh);				// Setzen des Zeichenbereiches
clrscr();
if (1) {//Eingeben der Generierungsparameter des Baumes
cout << "Startvorschlag Tiefe:10\nNeigung links:15,Wachstum links:0.7\nNeigung rechts: 18,Wachstum rechts:0.8\n";
cout << "Tiefe ( >1 ): ";
cin >> baum.Tiefe;		// Die Tiefe einlesen.

cout << "Neigung nach links in Grad: ";	// Die Neigung links einlesen.
cin >> baum.Neigung_links;

cout << "Wachstum nach links ( <1 ): ";// Das Wachstum links einlesen...
cin >> baum.Wachstum_links;

cout << "Neigung nach rechts in Grad: ";
cin >> baum.Neigung_rechts;

cout << "Wachstum nach rechts ( <1 ): ";
cin >> baum.Wachstum_rechts;
baum.WerteKorrigieren();		// Die Werte fuer das Wachstum ueberpruefen
// und wennn notwendig korrigieren.
Zeichne_Baum();					// Den Baum zeichnen.
cout << "Baum gezeichnet\n";

}
else {//Vermeidet das Eingeben der Daten. Das ist sinnvoll beim Testen

SetConsoleWindowTop();
baum.Tiefe = 10; baum.Neigung_links = 15; baum.Wachstum_links = 0.7f; baum.Neigung_rechts = 18; baum.Wachstum_rechts = 0.8f;
baum.WerteKorrigieren();		// Die Werte fuer das Wachstum ueberpruefen
// und wennn notwendig korrigieren.
Zeichne_Baum();					// Den Baum zeichnen.
cout << "Baum gezeichnet\n";
//Test des Wechsels des Front Fensters
Sleep(1000);
SetGraphicWindowTop();

Sleep(1000);
SetConsoleWindowTop();

Sleep(1000);
SetGraphicWindowTop();

//Testausgaben für Texte
text(30, 30, 10, BLUE, "test1");
text(40, 70, 20, BLUE, RED, "test2");
text(50, 110, 30, BLUE, "test3");
text(70, 150, 40, BLUE, RED, 45 * 10, 0, "test4"); //Winkel in Vielfachen von 0.1 Grad
text(80, 190, 30, BLUE, RED, 120 * 10, 0, "test5");
text(90, 230, 20, BLUE, "test6 %d Test", 6);

SetGraphicWindowTop();

}


Restart();						// Den "Restart"-Button malen und auf eine Aktivierung warten.
if(StopProcess())break;

}
}*/