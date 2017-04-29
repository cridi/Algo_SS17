// Graphicfunctions.cpp : implementation of the GDE functions
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "shape.h"
#include "Winuser.h"
#include "pointerarray.h"
#include "dib.h"
#include "..\mainfrm.h"
#include "..\GDE_3.h"
#include "..\GDE_3Doc.h"
#include "..\GDE_3View.h"

#include "Graphicfunctions.h"

#pragma once

// Koordinaten und Fenstergroesse
WORD MAX_X = 600;			// maximale x-Koordinate
WORD MAX_Y = 400;			// maximale y-Koordinate


extern CGDE_3App theApp;
void DoEvents()
{
	MSG oMSG;
	while(::PeekMessage(&oMSG, NULL,0,0,PM_NOREMOVE))
	if(::GetMessage(&oMSG,NULL,0,0))
	{
		::TranslateMessage(&oMSG);
		::DispatchMessage(&oMSG);
	}
	else break;
}
// Makro Objekte von CShape abgeleiteten Klassen anzulegen und 
// einen Zeiger darauf im Array aShape zu speichern.
#define __add_to_aShape(classname) theApp.vw->GetDocument()->aShape.Add( new classname);

void drawtobuffer()
{
	//gsSem
	//gsSem printf("FFdrawtobuffer\n");
	while (theApp.GDE_3Sem == 1);//gsSemprintf("FFdrawtobuffer Waiting");
	if (theApp.GDE_3Sem == 0) {
		theApp.GDE_3Sem++;
		CGDE_3Doc *pDoc = theApp.vw->GetDocument();
		//pDoc=((CGDE_3View *)CWnd::FromHandle((theApp.hvw)))->GetDocument();
		//ASSERT_VALID(pDoc);
		int siz = (int)(pDoc->aShape.GetSize());
		pDoc->aShape[siz - 1]->Draw(&pDoc->buffer);
		theApp.GDE_3Sem--;
	}
	else {
		//gsSem printf("FFdrawtobuffer GDE_3Sem:%d\n",theApp.GDE_3Sem);
	}
}


void set_windowpos(int x, int y, int b, int h)
{
	extern CGDE_3App theApp;

	CRect client_rect, window_rect, status_rect, view_rect;

	theApp.m_pMainWnd->GetWindowRect(&window_rect);	// Die Windowgroesse ermitteln.
	int hhw=window_rect.Height();
	int www=window_rect.Width();

	theApp.vw->GetWindowRect(&view_rect);			//Die Viewgroesse ermitteln.
	int hhv=view_rect.Height();
	int wwv=view_rect.Width();


	// Zur Breite den Bereich ausserhalb des clients hinzurechnen.
	// Zusaetzlich muessen noch 3 addiert werden, da sonst die scrollbars gezeigt werden.
	b += www - wwv + 5;
	h += hhw - hhv + 5;

	theApp.m_pMainWnd->MoveWindow(x, y, b, h);
	theApp.m_pMainWnd->Invalidate();
}
void get_windowsize(int *b, int *h)
{
	RECT rect;
	theApp.vw->GetClientRect(&rect);				//Die Clientgroesse ermitteln
	*b = rect.right;
	*h = rect.bottom;
}
void set_drawarea(int b, int h)
{
	MAX_X = b;
	MAX_Y = h;

	while (theApp.vw == NULL) Sleep(100);// Warten bis CGDEView konstruiert ist.
	Sleep(100);
	theApp.vw->WRSetScrollSizes(MM_TEXT, CSize(MAX_X,MAX_Y)); // Anpassen des Bereiches der scrollbars.

	theApp.vw->GetDocument()->memoryBmp.DeleteObject();		// Das alte Bitmap im Speicher loeschen.
	CDC *pdc=theApp.vw->GetDC();
	if(!theApp.vw->GetDocument()->memoryBmp.CreateCompatibleBitmap(pdc,MAX_X, MAX_Y)){
		AfxMessageBox(L"Cannot create Bitmap",MB_OK);
	}
	theApp.vw->GetDocument()->buffer.SelectObject( &theApp.vw->GetDocument()->memoryBmp );				// Das Speicher-Bitmap als Geraetekontext auswaehlen.
	theApp.vw->GetDocument()->buffer.PatBlt(0, 0, MAX_X, MAX_Y, WHITENESS);	// Den ganzen Puffer weiss malen.
	theApp.vw->GetDocument()->nShapePos = 0;					// Zuruecksetzen der Position auf 0, um ein
									// Neuzeichnen aller Objekte zu gewaehrleisten.
	theApp.vw->Invalidate(TRUE);			// Zeichenbereich aktualisieren.
									// Der gesamte Zeichenbereich wird gelosescht.
}
void get_drawarea(int *b, int *h)
{
	*b = MAX_X;
	*h = MAX_Y;
}

// Bildpunkte und Bildschirmbereiche
void pixel(int x, int y, int color)
{
	__add_to_aShape( CPixel(x, y, color) );
	drawtobuffer();

}

void fill (int x, int y, int color1, int color2)
{
	__add_to_aShape( CFill(x, y, color1, color2) );
	drawtobuffer();

}


// Linien
void moveto(int x, int y)
{
	__add_to_aShape( CMoveTo(x, y) );
	drawtobuffer();

}

void lineto(int x, int y, int color)
{
	__add_to_aShape( CLineTo(x, y, color) );
	drawtobuffer();

}

void line(int x1, int y1, int x2, int y2, int color)
{
	__add_to_aShape( CLine(x1, y1, x2, y2, color) );
	drawtobuffer();

}


// Rechtecke
void rectangle(int x1, int y1, int x2, int y2, int cframe, int cfill)
{
	__add_to_aShape( CRectangle( CRect(x1, y1, x2, y2), cframe, cfill) );
	drawtobuffer();

}


// Kreise und Ellipsen
void ellipse(int x1, int y1, int x2, int y2, int cframe, int cfill)
{
	__add_to_aShape( CEllipse( CRect(x1, y1, x2, y2), cframe, cfill) );
	drawtobuffer();

}


// Text
void text(int x, int y, int size, int color, char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	__add_to_aShape( CText(x, y, size, color, format, argptr) );
	drawtobuffer();

}
void text(int x, int y, int size, int color, int bkcolor, char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	__add_to_aShape( CText(x, y, size, color, bkcolor, format, argptr) );
	drawtobuffer();

}
void text(int x, int y, int size, int color, int angle, int align, char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	__add_to_aShape( CText(x, y, size, color, angle, align, format, argptr) );
	drawtobuffer();
}
void text(int x, int y, int size, int color, int bkcolor, int angle, int align, char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	__add_to_aShape( CText(x, y, size, color, bkcolor, angle, align, format, argptr) );
	drawtobuffer();
}

void textbox(int x1, int y1, int x2, int y2, int size, int ctext, 
			 int cframe, int cfill, int flags, char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	__add_to_aShape( CTextBox( CRect(x1, y1, x2, y2), size, ctext, cframe, cfill, flags, format, argptr) );
	drawtobuffer();
}


// Ablaufsteuerung
int mouseclick(int *x, int *y)	// Wartet auf einen Mausklick und gibt dann button & key zurueck.
{
	
	theApp.vw->Invalidate();			// Zeichenbereich aktualisieren (GDEView::OnPaint() wird aufgerufen)
	theApp.vw->MouseClick = 0;				// Ruecksetzen der Maustaste.
	while (theApp.vw->MouseClick == 0)		// Warten bis eine Maustaste gedrueckt wird.
		{ 
			DoEvents();
			if(StopProcess())break;
			Sleep(20);						// 20ms warten
		}			
	*x = theApp.vw->MousePoint.x;			// Zurueckgeben der x-Koordinate des Mauszeigers.
	*y = theApp.vw->MousePoint.y;			// Zurueckgeben der y-Koordinate des Mauszeigers.
	return theApp.vw->MouseClick;			// Zurueckgeben der Maustasten- & Tastenkombination.
}

int checkmouse()				// Prueft ob zwischenzeitlich die Maus betaetigt wurde.
{
	DoEvents();
	theApp.vw->Invalidate();			// Zeichenbereich aktualisieren (GDEView::OnPaint() wird aufgerufen)
	if (theApp.vw->MouseClick > 0) {		// wenn Maustaste gedrueckt ...
		theApp.vw->MouseClick = 0;			// ... ruecksetzen der Maustaste ...
		return theApp.vw->MouseClick;		// ... und zurueckgeben der Tastenkombination.
	}
	else
		return 0;				// ansonsten 0 zurueckgeben.
}

void wait(unsigned int msecs)	// Wartet die vorgegebende Zeit in Millisekunden
{
 	theApp.vw->Invalidate(FALSE);	// Update screen ( GDEView::OnPaint() wird aufgerufen)
	Sleep(msecs);
}


//GSSem
void clrscr()					// Loeschen des gesamten Zeichenbereiches.
{
	//gsSem printf("FFclrscr\n");
	while (theApp.GDE_3Sem == 1);//gsSem printf("FFclrscr Waiting");
	if (theApp.GDE_3Sem == 0) {
		theApp.GDE_3Sem++;
		theApp.vw->GetDocument()->aShape.RemoveAll();			// Das Array leeren...
		theApp.vw->GetDocument()->nShapePos = 0;				// ...und die Position ruecksetzen.
		theApp.vw->GetDocument()->buffer.PatBlt(0, 0, MAX_X, MAX_Y, WHITENESS);	// Den ganzen Puffer weiss malen.
		theApp.vw->Invalidate(FALSE);	// Zeichenbereich aktualisieren (GDEView::OnPaint() wird aufgerufen)
										//	theApp.vw->SendMessage(WM_PAINT);//Zeichenbereich aktualisieren (GDEView::OnPaint() wird aufgerufen)
		theApp.GDE_3Sem--;
	}
	else {
		//gsSem printf("FFclrscr GDE_3Sem:%d\n",theApp.GDE_3Sem);
	}

}

void updatescr()				// Neuzeichnen des gesamten Zeichenbereiches.
{
	//gsSem
	//gsSem printf("FFupdatescr\n");
	while (theApp.GDE_3Sem == 1);//gsSem printf("FFupdatescr Waiting");
	if (theApp.GDE_3Sem == 0) {
		theApp.GDE_3Sem++;
		theApp.vw->Invalidate(FALSE);	// Zeichenbereich aktualisieren (GDEView::OnPaint() wird aufgerufen)
		theApp.GDE_3Sem--;
	}
	else {
		//gsSem printf("FFupdatescr GDE_3Sem:%d\n",theApp.GDE_3Sem);
	}
}
BOOL StopProcess()
{
	return theApp.m_stopflag ;
}

// Grafikfenster als oberstes Fenster anzeigen
void SetGraphicWindowTop() {
	//gscon
	SetWindowPos(AfxGetApp()->GetMainWnd()->m_hWnd,NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}
// Consolefenster als oberstes Fenster anzeigen
void SetConsoleWindowTop() {
	//gscon
	SetWindowPos(AfxGetApp()->GetMainWnd()->m_hWnd,GetConsoleWindow(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

