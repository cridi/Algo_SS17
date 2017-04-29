// PointerArray.h: interface for the CPointerArray class.
//
// Die von CArrray abgeleitete Klasse CPointerArray,
// stellt ein eindimensionales Feld dar, indem Zeiger
// beliebigen Types gespeichert werden koennen.
// Die Besonderheit liegt darin, dass, beim Loeschen eines Elementes 
// (gespeicherten Zeigers), der Speicherbereich, den das Objekt,
// auf den der Zeiger zeigt, belegt, wieder freigegeben wird.
// Kleines Bsp.:
//   Zeiger A zeigt auf das Objekt B, das im Speicher 200 Byte belegt.
//   Zeiger A wird in diesem Array gespeichert.
//   Beim Loeschen des Zeigers A wird auch das Objekt B geloescht.
// Dieses vorgehen ist eine einfache Form von "garbage collection"
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <afxtempl.h>			// includes CArray

template <class TYPE, class ARG_TYPE> class CPointerArray : public CArray <TYPE, ARG_TYPE>
{
public:
	void RemoveAll() {								// Leeren der gesamten Arrays.
		for (int i=0; i<this->GetSize(); i++)		// Das Feld ganz durchlaufen...
			delete(this->GetAt(i));					// ...und alle Speicherbloecke wieder freigeben.
		CArray <TYPE, ARG_TYPE>::RemoveAll();		// Alle gespeicherten Zeiger aus dem Feld loeschen.
	}

	void RemoveAt(int nIndex, int nCount = 1) {		// Loeschen von nCount Elementen aus dem Array.
		for (int i=nIndex; i<nIndex+nCount; i++)	// Die zu loeschenden Elemente durchlaufen...
			delete(this->GetAt(i));					// ...und die Speicherbloecke wieder freigeben.
		CArray <TYPE, ARG_TYPE>::RemoveAt(nIndex, nCount);	//	
	}
};