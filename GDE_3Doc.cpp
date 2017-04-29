// GDE_3Doc.cpp : Implementierung der Klasse CGDE_3Doc
//

#include "stdafx.h"
#include "GDE_3.h"

#include "GDE_3Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern WORD MAX_X;
extern WORD MAX_Y;


// CGDE_3Doc

IMPLEMENT_DYNCREATE(CGDE_3Doc, CDocument)

BEGIN_MESSAGE_MAP(CGDE_3Doc, CDocument)
END_MESSAGE_MAP()


// CGDE_3Doc Erstellung/Zerstörung

CGDE_3Doc::CGDE_3Doc()
{
	// TODO: Hier Code für One-Time-Konstruktion einfügen

}

CGDE_3Doc::~CGDE_3Doc()
{
	aShape.RemoveAll();				// Das Array vollstaendig leeren.

}

BOOL CGDE_3Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)

	return TRUE;
}




// CGDE_3Doc Serialisierung

void CGDE_3Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: Hier Code zum Speichern einfügen
	}
	else
	{
		// TODO: Hier Code zum Laden einfügen
	}
}


// CGDE_3Doc Diagnose

#ifdef _DEBUG
void CGDE_3Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGDE_3Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGDE_3Doc-Befehle
