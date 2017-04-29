// GDE_3.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "console\console.h"
#include "GDE_3.h"
#include "MainFrm.h"

#include "GDE_3Doc.h"
#include "GDE_3View.h"
#include ".\gde_3.h"
#include "Graphics\Graphicfunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern WORD MAX_X;
extern WORD MAX_Y;


// CGDE_3App

BEGIN_MESSAGE_MAP(CGDE_3App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Dateibasierte Standarddokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standarddruckbefehl "Seite einrichten"
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()


// CGDE_3App-Erstellung

CGDE_3App::CGDE_3App()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
	m_stopflag=FALSE;
	//GSSem
	GDE_3Sem = 0;

}


// Das einzige CGDE_3App-Objekt

CGDE_3App theApp;
/////////////////////////////////////////////////////////////////////////////
/*Console object*/
CConsole con(TRUE);

// Dieser thread startet die Funktion main(), in der Benutzer seinen Code hat.
UINT StartGDE(LPVOID lpv)
{
	extern void user_main();
	user_main();
	theApp.vw->uthread=NULL;
	return TRUE;
}

// CGDE_3App Initialisierung

BOOL CGDE_3App::InitInstance()
{
	// InitCommonControls() ist für Windows XP erforderlich, wenn ein Anwendungsmanifest
	// die Verwendung von ComCtl32.dll Version 6 oder höher zum Aktivieren
	// von visuellen Stilen angibt. Ansonsten treten beim Erstellen von Fenstern Fehler auf.

	InitCommonControls();



	CWinApp::InitInstance();

	// OLE-Bibliotheken initialisieren
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standardinitialisierung
	// Wenn Sie diese Features nicht verwenden und die Größe
	// der ausführbaren Datei verringern möchten, entfernen Sie
	// die nicht erforderlichen Initialisierungsroutinen.
	// Ändern Sie den Registrierungsschlüssel unter dem Ihre Einstellungen gespeichert sind.
	// TODO: Ändern Sie diese Zeichenfolge entsprechend,
	// z.B. zum Namen Ihrer Firma oder Organisation.



	SetRegistryKey(_T("GDE3"));
	LoadStdProfileSettings(16);  // Standard INI-Dateioptionen laden (einschließlich MRU)
	// Dokumentvorlagen der Anwendung registrieren. Dokumentvorlagen
	//  dienen als Verbindung zwischen Dokumenten, Rahmenfenstern und Ansichten.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGDE_3Doc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CGDE_3View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Befehlszeile parsen, um zu prüfen auf Standardumgebungsbefehle DDE, Datei offen
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Verteilung der in der Befehlszeile angegebenen Befehle. Es wird FALSE zurückgegeben, wenn
	// die Anwendung mit /RegServer, /Register, /Unregserver oder /Unregister gestartet wurde.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// Das einzige Fenster ist initialisiert und kann jetzt angezeigt und aktualisiert werden.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();


	SetWindowPos(m_pMainWnd->m_hWnd,NULL, 0, 0, MAX_X, MAX_Y, NULL);

	// Rufen Sie DragAcceptFiles nur auf, wenn eine Suffix vorhanden ist.
	//  In einer SDI-Anwendung ist dies nach ProcessShellCommand erforderlich

	/*Console start*/

	con.RedirectToConsole(0);
	con.SetNumberOfColumns(120);
	Sleep(10);
	con.SetNumberOfLines(2500);
	Sleep(10);

	SetGraphicWindowTop();
	/*Console end*/


	return TRUE;
}



// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CGDE_3App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CGDE_3App Meldungshandler


void CGDE_3App::OnAppExit()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.

	
}

void CGDE_3App::OnFileOpen()
{
	// TODO: Fügen Sie hier Ihren Befehlsbehandlungscode ein.
}
