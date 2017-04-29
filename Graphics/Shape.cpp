// Shape.cpp: implementation of the CShape class and drived classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shape.h"
#include <malloc.h>
#include <CString>

#pragma warning( disable : 4996 ) // disable sprintf warning


// Dieses Makro changeflag holt aus der Variable "var" 
// das flag "extract" heraus und fuegt dafuer das flag "insert" ein.
#define __changeflag(var, extract, insert)	if (var & extract) {	\
												var ^= extract;		\
												var |= insert;		\
											};

//////////////////////////////////////////////////////////////////////
// CShape Class

CShape::CShape(short x, short y, COLORREF crColor)
{
	// Initialisieren der Member-Variablen
	m_x = x;
	m_y = y;
	m_crColor = crColor;
}


//////////////////////////////////////////////////////////////////////
// CFill Class

CFill::CFill(short x, short y, COLORREF crFillColor, COLORREF crBorderColor) : 
		CShape(x, y, crBorderColor)
{
	m_crFillColor = crFillColor;
};

void CFill::Draw(CDC* pDC)
{ 
	CBrush brush(m_crFillColor);		// Erstellen...
	pDC->SelectObject(&brush);			// ...und auswaehlen ein GDI brushes
	pDC->FloodFill(m_x, m_y, m_crColor);// Fuellen...
	brush.DeleteObject();				// und Brush loeschen.
};


//////////////////////////////////////////////////////////////////////
// CLineTo Class

void CLineTo::Draw(CDC* pDC)
{
	
    CPen pen;  // Konstruieren, dann Initialisieren
    if( pen.CreatePen( PS_SOLID, 1, m_crColor ) )
    {
        // Auswahl des Objekts im Gerätekontext
        // Gleichzeitiges Speichern des alten Stifts
        CPen* pOldPen = pDC->SelectObject( &pen );

        // Zeichnen mit dem Stift
		pDC->LineTo(m_x, m_y);					// Ziehen einer Linie...
        // Wiederherstellen des alten Stifts im Gerätekontext
        pDC->SelectObject( pOldPen );
    }
    else
    {
        // Den Benutzer alarmieren, daß die Ressourcen knapp werden
    }

/*	CPen pen(PS_SOLID, 1, m_crColor);		// Generieren eines Zeichenstiftes...
	pDC->SelectObject(&pen);				// ...und auswaehlen dieses Stiftes.
	pDC->LineTo(m_x, m_y);					// Ziehen einer Linie...
*/
//	pen.DeleteObject();						// ...und anschliessendes Loeschen des ausgewaehlten Siftes.
}


//////////////////////////////////////////////////////////////////////
// CLine Class

CLine::CLine(short x1, short y1, short x2, short y2, UINT crColor)  : CLineTo(x1, y1, crColor)
{
	m_point = CPoint(x2, y2);
}

void CLine::Draw(CDC* pDC)
{
	POINT currentpos;
	currentpos = pDC->GetCurrentPosition(); // Ermitteln der aktuellen Cursorposition.

	pDC->MoveTo(m_point);					// Bewegen des Cursors zum Zielpunkt, ...
	CLineTo::Draw(pDC);						// ...zeichnen der Linie und...
	pDC->MoveTo(currentpos);				// ...rueckkehren zum Ausgangspunkt.
}


//////////////////////////////////////////////////////////////////////
// CRectangle Class

CRectangle::CRectangle(RECT rect, COLORREF crFrame, COLORREF crFill) : CShape(NULL, NULL, crFrame)
{
	m_rect   = rect;
	m_crFill = crFill;
}

void CRectangle::Draw(CDC* pDC)
{
	if (m_crFill == -1) {					// Ein durchsichtiges Rechteck zeichnen.
		CBrush brush(m_crColor);			// Erstellen eines graphics device interface (GDI) brushes...
		pDC->SelectObject(&brush);			// ...und auswaehlen des GDI brushes.
		pDC->FrameRect(&m_rect, &brush);	// Zeichnen...
		brush.DeleteObject();				// ..und brush loeschen.
	}
	else {									// Ein ausgefuelltes Rechteck zeichnen.
		CPen pen(PS_SOLID, 1, m_crColor);	// Zeichenstift fuer den Rand.
		pDC->SelectObject(&pen);

		CBrush brush(m_crFill);				// Brush fuer die ausgefuellte Flaeche.
		pDC->SelectObject(&brush);

		pDC->Rectangle(&m_rect);

		brush.DeleteObject();
		pen.DeleteObject();
	};
}


//////////////////////////////////////////////////////////////////////
// CEllipse Class

CEllipse::CEllipse(RECT rect, COLORREF crFrame, COLORREF crFill) : CShape(NULL, NULL, crFrame)
{
	m_rect		= rect;
	m_zoomrect	= rect;
	m_crFill	= crFill;
}

void CEllipse::Draw(CDC* pDC)
{
	CBrush brush;
	CPen   pen(PS_SOLID, 1, m_crColor);
	pDC->SelectObject(&pen);

	if (m_crFill == -1)						// Eine transparente Ellipse zeichnen.
		brush.CreateStockObject(NULL_BRUSH);
	else									// Eine ausgefuellte Ellipse zeichnen.
		brush.CreateSolidBrush(m_crFill);

	pDC->SelectObject(&brush);
	pDC->Ellipse(&m_zoomrect);

	pen.DeleteObject();
	brush.DeleteObject();
}


//////////////////////////////////////////////////////////////////////
// CTextA Class

CTextA::CTextA(short x1, short y1, WORD nSize, COLORREF crTextColor, COLORREF crBkground, short nAngle, 
			 UINT nFlags, char* format, va_list argptr) : CShape(x1, y1, crTextColor)
{
	// Die Member-Variablen der Klasse initialisieren.
	m_nSize		= nSize;
	m_crBkground= crBkground;
	m_nBkMode	= OPAQUE;
	m_nAngle	= nAngle;
	m_nFlags	= nFlags;

	// Da die Befehle TextOut und DrawText (in CTextBox benutzt)
	// teilweise verschiedene numerische Konstanten fuer die Ausrichtung benutzen,
	// jedoch global die einundselben verwendet werden sollen, muessen die flags, 
	// die sich unterscheiden veraendert werden.
	__changeflag(m_nFlags, DT_VCENTER, TA_BASELINE);
	__changeflag(m_nFlags, DT_CENTER,  TA_CENTER);

	m_string = (char*) malloc(_MAX_STRING);			  // Einen ausreichend grossen Speicherblock zuweisen.
	m_nCount = vsprintf(m_string, format, argptr);	  // Aus der Argumentenliste einen string generieren.
	m_string = (char*) realloc(m_string, m_nCount+1); // Den Speicherblock auf die benoetigte Groesse verkleinern.
}

void CTextA::Draw(CDC* pDC)
{
	// Setzen einer Schriftart mit speziellen Eignenschaften.

	CFont font;
	VERIFY(font.CreateFont(
		m_nSize,                        // nHeight
		0,                         // nWidth
		m_nAngle,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Calibri")));                 // lpszFacename
	pDC->SelectObject(&font);



	pDC->SetTextColor(m_crColor);				// Textfarbe setzen.
	if (m_nBkMode == OPAQUE)					// Wenn undurchsichtig, ...
		pDC->SetBkColor(m_crBkground);			// ...dann Hintergrundfarbe setzen.
	pDC->SetBkMode(m_nBkMode);					// Festlegen ob durchsichtig oder undurchsichtig.
	pDC->SetTextAlign(m_nFlags);				// Setzen der Textausrichtung.
	pDC->TextOut(m_x, m_y,CString( m_string), m_nCount);	// Ausgabe des Textes.
	font.DeleteObject();						// Loeschen der Ausgewaehlten Schriftcharakteristik
}


//////////////////////////////////////////////////////////////////////
// CTextBox Class

CTextBox::CTextBox(RECT rect, WORD nSize, COLORREF crText, COLORREF crFrame, COLORREF crFill,
				   UINT nFormat, char* format, va_list argptr) : CShape(NULL, NULL, crText)
{
	m_rect	  = rect;
	m_nSize   = nSize;
	m_nFormat = nFormat;
	m_crFrame = crFrame;
	m_crFill  = crFill;

	m_string = (char*) malloc(_MAX_STRING);			  // Einen ausreichend grossen Speicherblock zuweisen.
	m_nCount = vsprintf(m_string, format, argptr);	  // Aus der Argumentenliste einen string generieren.
	m_string = (char*) realloc(m_string, m_nCount+1); // Den Speicherblock auf die benoetigte Groesse verkleinern.
}

void CTextBox::Draw(CDC* pDC)
{
	// Setzen einer Schriftart mit speziellen Eignenschaften.

	CFont font;
	VERIFY(font.CreateFont(
		m_nSize,                   // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Calibri")));           // lpszFacename

	pDC->SelectObject(&font);

	pDC->SetTextColor(m_crColor);		// Textfarbe setzen.
	pDC->SetBkMode(TRANSPARENT);		// Festlegen auf durchsichtig.

	// Zeichnen eines fuellenden Rechteckes.
	CRectangle rectangle(m_rect, m_crFrame, m_crFill);
	rectangle.Draw(pDC);

	m_rect.DeflateRect(1, 1, 1, 1);		// Verkleinern des virtuellen Rechteckes
										// um die Linienbreite des Rahmens.
	pDC->SetTextAlign(NULL);			// Textausrichtung zuruecksetzen.
	pDC->DrawText(CString(m_string),  m_nCount, &m_rect, m_nFormat);	// Text ausgeben.

	font.DeleteObject();
}