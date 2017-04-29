// GDE_3.h : Hauptheaderdatei für die GDE_3-Anwendung
//
#ifndef _GDE_3_H
#define _GDE_3_H
#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"       // Hauptsymbole


// CGDE_3App:
// Siehe GDE_3.cpp für die Implementierung dieser Klasse
//
class CGDE_3View;

class CGDE_3App : public CWinApp
{
public:
	CGDE_3App();
	CGDE_3View *vw;		//actual view
	BOOL	m_stopflag;

	//GSSem
	int GDE_3Sem;




// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppExit();
	afx_msg void OnFileOpen();
};

extern CGDE_3App theApp;
#endif