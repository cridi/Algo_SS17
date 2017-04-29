// GDE_3View.cpp : Implementierung der Klasse CGDE_3View
//

#include "stdafx.h"
#include "GDE_3.h"
#include "mainfrm.h"
#include "GDE_3Doc.h"
#include "GDE_3View.h"
#include ".\gde_3view.h"
#include "user.h"
#include "Wincon.h"
#include "Windows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern WORD MAX_X;
extern WORD MAX_Y;


// CGDE_3View

IMPLEMENT_DYNCREATE(CGDE_3View, CScrollView)

BEGIN_MESSAGE_MAP(CGDE_3View, CScrollView)
	// Standarddruckbefehle
	ON_COMMAND(ID_STARTBUTTON, OnStartButton)
	ON_COMMAND(ID_STOPBUTTON, OnStopButton)
	ON_COMMAND(ID_BUTTONZOOMOUT, OnButtonzoomout)
	ON_COMMAND(ID_BUTTONZOOMIN, OnButtonzoomin)
	ON_COMMAND(ID_BUTTONZOOMFIT, OnButtonzoomfit)
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
//	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()

// CGDE_3View Erstellung/Zerstörung

CGDE_3View::CGDE_3View()
{
	// TODO: Hier Code zum Erstellen einfügen
	m_fXScale   = 1.0f; // zooming scale
	m_fYScale   = 1.0f;
	m_fDelta	= 1.2f;
	m_nMapMode	= MM_TEXT;
	uthread		= NULL;

}

CGDE_3View::~CGDE_3View()
{
}

BOOL CGDE_3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Ändern Sie hier die Fensterklasse oder die Darstellung, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CScrollView::PreCreateWindow(cs);
}

// CGDE_3View-Zeichnung

//GSSem
void CGDE_3View::OnDraw(CDC* pDC)
{
	if (theApp.GDE_3Sem == 0) {
		theApp.GDE_3Sem++;
		CGDE_3Doc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;

		// TODO: Code zum Zeichnen der systemeigenen Daten hinzufügen
		pDC->StretchBlt(0, 0, (int)(MAX_X*m_fXScale), (int)(MAX_Y*m_fYScale), &pDoc->buffer, 0, 0, MAX_X, MAX_Y, SRCCOPY);
		theApp.GDE_3Sem--;
	}
	else {
		//gsSem printf("OnDraw GDE_3Sem:%d\n",theApp.GDE_3Sem);
	}

}

void CGDE_3View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	// TODO: Gesamte Größe dieser Ansicht berechnen
	theApp.vw = this;	// Pointer auf das aktuelle Window-Fenster speichern.

	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	SetScrollSizes(MM_TEXT,CSize(MAX_X, MAX_Y));

	CGDE_3Doc *pDoc = GetDocument();
	CDC *pdc=GetDC();
	pDoc->buffer.DeleteDC();
	if(!pDoc->buffer.CreateCompatibleDC(pdc)){
		AfxMessageBox(L"Cannot create buffer DC",MB_OK);
	}
	if(!(pDoc->memoryBmp.CreateCompatibleBitmap(pdc,MAX_X, MAX_Y))){
		AfxMessageBox(L"Cannot create Bitmap",MB_OK);
	}
	pDoc->buffer.SelectObject( &pDoc->memoryBmp );				// Das Speicher-Bitmap als Geraetekontext auswaehlen.
	pDoc->nShapePos =0;
	ReleaseDC(pdc);


}


// CGDE_3View drucken

BOOL CGDE_3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CGDE_3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Zusätzliche Initialisierung vor dem Drucken hier einfügen
}

void CGDE_3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: Bereinigung nach dem Drucken einfügen
}


// CGDE_3View Diagnose

#ifdef _DEBUG
void CGDE_3View::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGDE_3View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CGDE_3Doc* CGDE_3View::GetDocument() const // Nicht-Debugversion ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGDE_3Doc)));
	return (CGDE_3Doc*)m_pDocument;
}
#endif //_DEBUG


// CGDE_3View Meldungshandler
void CGDE_3View::OnButtonzoomout() 
{
	CSize size = GetTotalSize();
	CRect rect;
	GetClientRect(rect);
	int width  = rect.Width();
	int height = rect.Height();
	if (size.cx <= rect.Width() && size.cy <= rect.Height())
		return;

	if (width < size.cx)
	{
		size.cx    = (int)(size.cx / m_fDelta); 
		m_fXScale /= m_fDelta;
	}
	if (height < size.cy)
	{
		size.cy    = (int)(size.cy / m_fDelta); 
		m_fYScale /= m_fDelta;
	}

	if (size.cx <= rect.Width())
	{
		size.cx   = rect.Width();
		m_fXScale = 1.0f;
	}
	if (size.cy <= rect.Height())
	{
		size.cy   = rect.Height();
		m_fYScale = 1.0f;
	}

	SetScrollSizes(m_nMapMode, size);
	Invalidate();
	
}

void CGDE_3View::OnButtonzoomin() 
{
//	if (m_fXScale >= MAX_ZOOM_FACTOR && m_fYScale >= MAX_ZOOM_FACTOR)
//		return;

	CSize size = GetTotalSize();
	size.cx    = (int)(size.cx * m_fDelta); 
	size.cy    = (int)(size.cy * m_fDelta); 
	m_fXScale *= m_fDelta;
	m_fYScale *= m_fDelta;

	SetScrollSizes(m_nMapMode, size);
	Invalidate();
	
}

void CGDE_3View::OnButtonzoomfit() 
{
	m_fXScale = 1.0;
	m_fYScale = 1.0;

	SetScrollSizes(MM_TEXT, CSize(MAX_X,MAX_Y)); 
	Invalidate();
	
}
void CGDE_3View::OnStartButton() 
{
	theApp.m_stopflag=FALSE;
	if(theApp.vw->uthread==NULL)
		theApp.vw->uthread = AfxBeginThread(StartGDE, &theApp, THREAD_PRIORITY_NORMAL);
	else AfxMessageBox(L"User_main is already running\n",MB_OK);
	
}
void CGDE_3View::OnStopButton() 
{
	theApp.m_stopflag=TRUE;
	int cc=0;
	while(theApp.vw->uthread!=NULL){
		Sleep(100);
		HWND hw=::GetConsoleWindow();
		::SetForegroundWindow(hw);
		//VOID keybd_event( BYTE bVk, BYTE bScan, DWORD dwFlags, DWORD dwExtraInfo );
		keybd_event( '0', 0, 0, 0 );
		keybd_event( 13, 0, 0, 0 );//CR
		cc++;
		if(cc==10)break;
	}
	//TRACE("cc=%d\n",cc);
	
	
}


void CGDE_3View::OnFileSave()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	CWaitCursor wait;

	CClientDC scrDC(this);
	CDC memDC;           // screen DC and memory DC
	CBitmap bitmap;
	CBitmap* pOldBitmap;  // handles to device-dependent bitmaps

	CRect rect;
	GetClientRect(&rect);

	memDC.CreateCompatibleDC(&scrDC);
	bitmap.CreateCompatibleBitmap(&scrDC, rect.Width(), rect.Height());

	pOldBitmap = memDC.SelectObject(&bitmap);// select new bitmap into memory DC

	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &scrDC, 0, 0, SRCCOPY);

	CGDE_3Doc* pDoc = GetDocument();
	CString filename = pDoc->GetPathName();

	CDib dib;
	try {
		dib.Create(&bitmap);
		dib.Save(filename);
	} catch(CImageException& e) {
		::AfxMessageBox(LPCTSTR(e.what()));
	}

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

	
}

void CGDE_3View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	MouseClick = nFlags;			// Tastenkombination und...
	//printf("1.x:%d,y:%d\n",point.x,point.y);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&point);
	//printf("2.x:%d,y:%d\n",point.x,point.y);
	CPoint shpoint=point;
	shpoint.x = (int)(point.x/m_fXScale);
	shpoint.y = (int)(point.y/m_fYScale);
	MousePoint = shpoint;				// ...Position speichern.
	//printf("3.x:%d,y:%d\n",shpoint.x,shpoint.y);
	

	CScrollView::OnLButtonDown(nFlags, point);
}

void CGDE_3View::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.

	CScrollView::OnLButtonUp(nFlags, point);
}

void CGDE_3View::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	

	CScrollView::OnRButtonDown(nFlags, point);
}



void CGDE_3View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein, und/oder benutzen Sie den Standard.
	CClientDC dc(this);
	OnPrepareDC(&dc);
	//printf("1.x:%d,y:%d\n",point.x,point.y);
	dc.DPtoLP(&point);
	//printf("2.x:%d,y:%d\n",point.x,point.y);
	CPoint shpoint=point;
	shpoint.x = (int)(point.x/m_fXScale);
	shpoint.y = (int)(point.y/m_fYScale);
	//printf("3.x:%d,y:%d\n",shpoint.x,shpoint.y);
	CString strCurPos;
	strCurPos.Format(L"[x,y]=%d,%d ",shpoint.x, shpoint.y);
	CStatusBar *sb= &((CMainFrame *)theApp.m_pMainWnd)->m_wndStatusBar;
	sb->SetPaneText(sb->CommandToIndex(ID_INDICATOR_CURPOS),strCurPos);

	CScrollView::OnMouseMove(nFlags, point);
}

void CGDE_3View::OnDestroy()
{
	CScrollView::OnDestroy();

	// TODO: Fügen Sie hier Ihren Meldungsbehandlungscode ein.
	OnStopButton();

}

void CGDE_3View::OnFileOpen()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	TCHAR szFilters[] = _T ("bmp files (*.bmp)|*.bmp|All files (*.*)|*.*||");
	CFileDialog dlg(FALSE, _T ("bmp"), _T ("*.bmp"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,
		szFilters);
	
	CString filename;
	if(dlg.DoModal() == IDOK)
		filename = dlg.GetPathName();
	else
		return;
 
	CDib dib;
	try {
		dib.Load(filename);
	} catch(CImageException& e) {
		::AfxMessageBox(LPCTSTR(e.what()));
	}

	CGDE_3Doc* pDoc = GetDocument();
	pDoc->SetTitle(dlg.GetFileName());
	dib.Load(filename);
	int ww=dib.GetWidth();
	int hh=dib.GetHeight();
	dib.Draw(&(pDoc->buffer),0,0,ww,hh,SRCCOPY);
	pDoc->SetTitle(filename);
	pDoc->SetPathName(filename);

	Invalidate();

}

void CGDE_3View::OnFileSaveAs()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
		CWaitCursor wait;

	CClientDC scrDC(this);
	CDC memDC;           // screen DC and memory DC
	CBitmap bitmap;
	CBitmap* pOldBitmap;  // handles to device-dependent bitmaps

	CRect rect;
	GetClientRect(&rect);

	memDC.CreateCompatibleDC(&scrDC);
	bitmap.CreateCompatibleBitmap(&scrDC, rect.Width(), rect.Height());

	pOldBitmap = memDC.SelectObject(&bitmap);// select new bitmap into memory DC

	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &scrDC, 0, 0, SRCCOPY);

	TCHAR szFilters[] = _T ("bmp files (*.bmp)|*.bmp|All files (*.*)|*.*||");
	CFileDialog dlg(FALSE, _T ("bmp"), _T ("*.bmp"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilters);
	
	CString filename;
	if(dlg.DoModal() == IDOK)
		filename = dlg.GetPathName();
	else
		return;
 
	CDib dib;
	try {
		dib.Create(&bitmap);
		dib.Save(filename);
	} catch(CImageException& e) {
		::AfxMessageBox(LPCTSTR(e.what()));
	}

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

	CGDE_3Doc* pDoc = GetDocument();
	pDoc->SetTitle(filename);
	pDoc->SetPathName(filename);
	}

void CGDE_3View::OnFileNew()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
	AfxMessageBox(L"Not supported",MB_OK);
}
