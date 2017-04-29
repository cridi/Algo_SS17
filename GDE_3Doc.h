// GDE_3Doc.h : Schnittstelle der Klasse CGDE_3Doc
//
#ifndef _GDE_3DOC_H
#define _GDE_3DOC_H

#include "graphics\shape.h"
#include "graphics\pointerarray.h"
#include "graphics\dib.h"

#pragma once

class CGDE_3Doc : public CDocument
{
protected: // Nur aus Serialisierung erstellen
	CGDE_3Doc();
	DECLARE_DYNCREATE(CGDE_3Doc)

// Attribute
public:
	//gg CDib		m_dib;
	// Double buffering
	CBitmap	memoryBmp;						// Bitmap im Speicher.
	CDC	buffer;								// Geraetekontext-Objekt
	int		nShapePos;						// Position des Grafikobjekts ab dem neu gezeichnet wird.
	CPointerArray <CShape*, CShape*> aShape;

// Operationen
public:

// Überschreibungen
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementierung
public:
	virtual ~CGDE_3Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	DECLARE_MESSAGE_MAP()
};
#endif

