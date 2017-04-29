// GDE_3View.h : Schnittstelle der Klasse CGDE_3View
//
#ifndef _GDE_3VIEW_H
#define _GDE_3VIEW_H

#pragma once

extern UINT StartGDE(LPVOID lpv);

class CGDE_3View : public CScrollView
{
protected: // Nur aus Serialisierung erstellen
	CGDE_3View();
	DECLARE_DYNCREATE(CGDE_3View)

// Attribute
public:
	CGDE_3Doc* GetDocument() const;
	BYTE  MouseClick;						// Speichert Tastendruecke mit der Maus.
	POINT MousePoint;						// Position des Mauszeigers waehrend dem Klicken.

	// zooming
	float  m_fXScale;
	float  m_fYScale;
	float  m_fDelta;
	int	   m_nMapMode;

	CWinThread* uthread;

// Operationen
public:
void WRSetScrollSizes (int mode, CSize si)
{
	((CGDE_3View *)FromHandle((theApp.vw->m_hWnd)))->SetScrollSizes(mode, si);
	((CGDE_3View *)FromHandle((theApp.vw->m_hWnd)))->Invalidate();
}
// Überschreibungen
	public:
	virtual void OnDraw(CDC* pDC);  // Überladen, um diese Ansicht darzustellen
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementierung
public:
	virtual ~CGDE_3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
	afx_msg void OnStopButton();
	afx_msg void OnStartButton();
	afx_msg void OnButtonzoomout();
	afx_msg void OnButtonzoomin();
	afx_msg void OnButtonzoomfit();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileSave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
//	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileNew();
};

#ifndef _DEBUG  // Debugversion in GDE_3View.cpp
inline CGDE_3Doc* CGDE_3View::GetDocument() const
   { return reinterpret_cast<CGDE_3Doc*>(m_pDocument); }
#endif

#endif
