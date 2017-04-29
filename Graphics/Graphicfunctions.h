// Graphicfunctions.h : interface of the GDE functions
//
// In diesem Header-File sind alle Grafikfunktionen, die der Benutzer
// zur Programmierung verwenden kann deklariert.
//
/////////////////////////////////////////////////////////////////////////////


// Koordinaten und Fenstergroesse
void SetGraphicWindowTop();						// Grafikfenster als oberstes Fenster anzeigen
void SetConsoleWindowTop();						// Consolenfenster als oberstes Fenster anzeigen
void set_windowpos(int x, int y, int b, int h); // Groesse und Position des Grafikfensters setzen.
void get_windowsize(int *b, int *h);			// Groesse des Grafikfensters ermitteln.
void set_drawarea(int b, int h);				// Setzen der Groesse des Zeichenbereiches.
void get_drawarea(int *b, int *h);				// Ermitteln der Groesse des Zeichenbereches.


// Farben
#define BLACK	0
#define WHITE	RGB(255,255,255)
#define RED		RGB(255,0,0)
#define GREEN	RGB(0,255,0)
#define BLUE	RGB(0,0,255)
#define YELLOW	RGB(255,255,0)
#define GREY	RGB(192,192,192)
#define BROWN	RGB(128,64,0)


// Bildpunkte und Bildschirmbereiche
void pixel(int x, int y, int color);				// Setzen eines einzenen Pixels.
void fill (int x, int y, int color1, int color2);	// Fuellt einen Bereich des Fensters mit einer Farbe.
	// color1: Fuellt den durch diese Farbe begrenzten Bereich.
	// color2: Fuefarbe

// Linien
void moveto(int x, int y);				// Setzt die Position des virtuellen Grafikcursors.
void lineto(int x, int y, int color);	// Zieht eine Linie von den Koordinaten des Grafikcursors aus.
void line(int x1, int y1, int x2, int y2, int color);	// Zeichnet eine Linie.

// Rechtecke
void rectangle(int x1, int y1, int x2, int y2, int cframe, int cfill = -1);
	// Zeichnet ein Rechteck (optional deckend).
	// x1, y1 : obere linke Ecke
	// x2, y2 : untere rechte Ecke
	// cframe : Rahmenfarbe
	// cfill  : Fuellfarbe

// Kreise und Ellipsen
void ellipse(int x1, int y1, int x2, int y2, int cframe, int cfill = -1);
		// Zeichnet eine Ellipse (optional deckend).
		// x1, y1 : obere linke Ecke
		// x2, y2 : untere rechte Ecke
		// cframe : Rahmenfarbe
		// cfill  : Fuellfarbe

// Text
#define LEFT_ALIGN		DT_LEFT
#define RIGHT_ALIGN		DT_RIGHT
#define CENTER_ALIGN	DT_CENTER
#define TOP_ALIGN		DT_TOP
#define BOTTOM_ALIGN	DT_BOTTOM
#define VCENTER_ALIGN	DT_VCENTER
#define BREAK_WORDS		DT_WORDBREAK
#define NO_CLIPPING		DT_NOCLIP
#define SINGLE_LINE		DT_SINGLELINE

void text(int x, int y, int size, int color, char *format, ...);
void text(int x, int y, int size, int color, int bkcolor, char *format, ...);
void text(int x, int y, int size, int color, int angle, int align, char *format, ...);
void text(int x, int y, int size, int color, int bkcolor, int angle, int align, char *format, ...);
	// Mehrzeilige (\n) Textausgabe (analog printf);
	// size  : Textgroesse
	// color : Textfarbe
	// bkcolor : Hintergrundfarbe wenn deckend, sonst transparent.
	// angle   : Winkel um dem der Text gedreht wird.
	// align   : Ausrichtung des Textes.
	// der hintere Teil funktioniert analog zu printf
void textbox(int x1, int y1, int x2, int y2, int size, int textcolor, 
			 int framecolor, int fillcolor, int flags, char *format, ...);
		// Einzeilige Textausgabe in einer Textbox.
		// size      : Textgroesse
		// textcolor : Textfarbe
		// framecolor: Randfarbe
		// fillcolor : Fuellfarbe des Rahmens
		// flags	 : Ausrichtung LEFT_ALIGN. RIGHT_ALIGN, CENTER_ALIGN, TOP_ALIGN, ...
		// der hintere Teil funktioniert analog zu printf

// Ablaufsteuerung (all Funktionen beinhalten ein neuzeichnen des Zeichenbereiches)
#define MBTN_LEFT			MK_LBUTTON
#define MBTN_MIDDLE			MK_MBUTTON
#define MBTN_RIGHT			MK_RBUTTON
#define MBTN_SHIFT			MK_SHIFT
#define MBTN_CONTROL		MK_CONTROL

int mouseclick(int *x, int *y);	// Wartet auf einen Mausklick und gibt dann button & key zurueck.
int checkmouse();				// Prueft ob zwischenzeitlich die Maus betaetigt wurde.
								// 0: nicht betaetigt, ...
void wait(unsigned int msecs);	// Wartet die vorgegebende Zeit in Millisekunden.

void clrscr();					// Loescht den gesamten Zeichenbereich.
void updatescr();				// Neuzeichnen des gesamten Zeichenbereiches.

BOOL StopProcess();				// Gibt an ob ein Stopflagg gesetzt wurde