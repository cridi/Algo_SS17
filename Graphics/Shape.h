// Shape.h: interface for the CShape class and derived classes.
//
//////////////////////////////////////////////////////////////////////
#ifndef _SHAPE_H
#define _SHAPE_H
#define _MAX_STRING 255	// Festlegen der maximal moeglichen string-Laenge (CText, CTextBox)


/////////////////////////////////////////////////////////////////////////////
// Die Klasse CShape ist die Basisklasse aller Grafikklassen. 
// ALLE Grafikklassen haben diese als "Vorfahren".
//
// Allen Klassen ist gemeinsam, dass mit der Funktion
// Draw() gezeichnet und mit der Funktion Zoom() gezoomt werden kann.
// Diese beiden Funktionen sind aus diesem Grunde auch in der Basisklasse
// CShape mit dem Keywort "virtual" versehen.
class CShape
{
// Member-Variablen
protected:
	short	 m_x,
			 m_y;
	COLORREF m_crColor;

public:
	CShape(short x, short y, COLORREF crColor = 0);
	virtual ~CShape() {};	
	virtual void Draw(CDC* pDC) = 0;		// virtuelle Klasse zum zeichnen des Objektes

};


/////////////////////////////////////////////////////////////////////////////
// Diese Klasse dient dazu im Zeichenbereich einen einzelnen Bildpunkt
// mit einer gewuenschten Farbe zu setzen.
class CPixel : public CShape
{
public:
	CPixel(short x, short y, COLORREF crColor) : CShape(x, y, crColor) {};
	virtual ~CPixel() {};
	void Draw(CDC* pDC) { pDC->SetPixel(m_x, m_y, m_crColor); };

};


/////////////////////////////////////////////////////////////////////////////
// Diese Klasse dient zum Fuellen des Bereiches des Grafikfensters,
// der den Punkt (x, y) enthaelt und durch die Farbe crBorderColor begrenzt ist, 
// mit der Fullfarbe crFillClor1
class CFill : public CShape
{
protected:
	COLORREF m_crFillColor;

public:
	CFill(short x, short y, COLORREF crFillColor, COLORREF crBorderColor);
	virtual ~CFill() {};
	void Draw(CDC* pDC);
};


/////////////////////////////////////////////////////////////////////////////
// Klasse zum Positionieren des Cursors auf einem bestimmten Bildschirmpunkt.
// Diese Klasse muss im Zusammenhang mit der Klasse "LineTo" verwendet werden.
class CMoveTo : public CShape
{
public:
	CMoveTo(short x, short y) : CShape(x, y) {};
	virtual ~CMoveTo() {};
	void Draw(CDC* pDC)	{ pDC->MoveTo(m_x, m_y); };

};


/////////////////////////////////////////////////////////////////////////////
// Klasse zum ziehen einer Linie mit bestimmter Farbe zu einem Zielpunkt.
// Als Ausgangskoordinate dient der Cursor, der mit der Klasse "MoveTo" frei
// bewegt werden kann.
class CLineTo : public CShape
{
public:
	CLineTo(short x, short y, COLORREF crColor) : CShape(x, y, crColor) {};
	virtual ~CLineTo() {};
	void Draw(CDC* pDC);

};


/////////////////////////////////////////////////////////////////////////////
// Dient zum Zeichnen einer Linie vom Startpunkt (m_x, m_y) aus 
// zum Zielpunk (m_point) mit der gewuenschten Farbe crColor.
class CLine : public CLineTo
{
protected:
	POINT m_point;

public:
	CLine(short x1, short y1, short x2, short y2, UINT crColor);
	virtual ~CLine() {};
	void Draw(CDC* pDC);

};


/////////////////////////////////////////////////////////////////////////////
// Zeichnen eines Rechteckes oder nur eines Rechteckrahmens (crFill = -1)
// mit einer gewuenschten Rahmenfarbe bzw. Fullfarbe.
class CRectangle : public CShape
{
protected:
	RECT	 m_rect;
	COLORREF m_crFill;

public:
	CRectangle(RECT rect, COLORREF crFrame, COLORREF crFill = -1);
	virtual ~CRectangle() {};
	void Draw(CDC* pDC);

};


/////////////////////////////////////////////////////////////////////////////
// Zeichnen einer durchsichtigen oder undurchsichtigen (crFill = -1) Ellipse
// mit einer gewuenschten Rahmenfarbe bzw. Fullfarbe.
class CEllipse : public CShape
{
protected:
	RECT	 m_rect;
	RECT	 m_zoomrect;
	COLORREF m_crFill;

public:
	CEllipse(RECT rect, COLORREF crFrame, COLORREF crFill = -1);
	virtual ~CEllipse() {};
	void Draw(CDC* pDC);

};


/////////////////////////////////////////////////////////////////////////////
class CTextA : public CShape
{
protected:
	BYTE  m_nBkMode;
	WORD  m_nSize;
	short m_nAngle;
	UINT  m_nCount,
		  m_nFlags;
	char* m_string;
	COLORREF m_crBkground;

public:
	CTextA(short x1, short y1, WORD nSize, COLORREF crTextColor, COLORREF crBkground, short nAngle, UINT nFlags, char* format, va_list argptr);
	virtual ~CTextA() { free(m_string); };
	void Draw(CDC* pDC);

};


/////////////////////////////////////////////////////////////////////////////
class CText : public CTextA
{
public:
	CText(short x1, short y1, WORD nSize, COLORREF crTextColor, char* format, va_list argptr)
		: CTextA(x1, y1, nSize, crTextColor, NULL, NULL, NULL, format, argptr) {m_nBkMode = TRANSPARENT;};

	CText(short x1, short y1, WORD nSize, COLORREF crTextColor, COLORREF crBkground, char* format, va_list argptr)
		: CTextA(x1, y1, nSize, crTextColor, crBkground, NULL, NULL, format, argptr) {};

	CText(short x1, short y1, WORD nSize, COLORREF crTextColor, short nAngle, UINT nFlags, char* format, va_list argptr)
		: CTextA(x1, y1, nSize, crTextColor, NULL, nAngle, nFlags, format, argptr) {m_nBkMode = TRANSPARENT;};

	CText(short x1, short y1, WORD nSize, COLORREF crTextColor, COLORREF crBkground, short nAngle, UINT nFlags, char* format, va_list argptr)
		: CTextA(x1, y1, nSize, crTextColor, crBkground, nAngle, nFlags, format, argptr) {};

};


/////////////////////////////////////////////////////////////////////////////
class CTextBox : public CShape
{
protected:
	CRect m_rect;
	WORD  m_nSize;
	BYTE  m_nFormat;
	RECT  m_nRect;
	COLORREF m_crFrame,
			 m_crFill;
	WORD  m_nCount;
	char* m_string;

public:
	CTextBox(RECT rect, WORD nSize, COLORREF crText, COLORREF crFrame, 
			 COLORREF crFill, UINT nFormat, char* format, va_list argptr);
	virtual ~CTextBox() { free(m_string); };
	void Draw(CDC* pDC);

};
#endif