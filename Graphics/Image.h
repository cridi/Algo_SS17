// Image.h: interface for the CImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGE_H__D3142778_9555_4128_887F_104A5F9478E1__INCLUDED_)
#define AFX_IMAGE_H__D3142778_9555_4128_887F_104A5F9478E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseException.h"

/** ImageFormat constants */
enum IMAGE_FORMAT { IF_BMP, IF_JPG };


/** Exception class for CImage and its derivative classes.
*
*	ex) You can throw CImageException object like this.
*
*	1. throw CImageException();	-> Reason would be "Exception".
*
*	2. throw CImageException(_T("Disaster")); -> Reason would be "Disaster".
*
*	3. throw CImageException(_T("Disaster"), LOCATION); ->Reason would be "Disaster SomeFile line no 11".
*/
class CImageException : public BaseException
{
public:
	/** Default Constructor
	*	@param		None
	*	@exceptions	None
	*/
	CImageException() throw() : BaseException(string("ImageException")) {}
	/** Constructor with string description
	*	@param		message - This should be a useful message for error tracking.
	*	@exceptions	None
	*/
	CImageException(string message) throw() : BaseException(message) {}
	/** Constructor with string description and error location 
	*	@param		message - This should be a useful message for error tracking.
	*	@param		location - Location where Exception has occrurred. You can use LOCATION macro here.
	*	@exceptions	None
	*/
	CImageException(string message, string location) throw() : BaseException(message, location) {}
};

/** Base class of all Image Class.
*
*	Normally, you use this class for all bmp, jpg objects.
*
*	ex) CImage* pImage = new CDib;
*
*		pImage->Draw(pDC);
*/
class CImage  
{
public:
	/** Virtual Destructor.
	*	Does nothing
	*/
	virtual ~CImage(){}
	/** Load image from specified filename. Overriding Function
	*	@param		fileName - Name of the File
	*	@return		None
	*	@exceptions	CImageException
	*/
	virtual void Load(CString fileName) throw(CImageException) = 0;
	/** Load image from CFile pointer. Overriding Function
	*	@param		pFile - CFile pointer
	*	@return		None
	*	@exceptions	CImageException
	*/
	virtual void Load(CFile* pFile) throw(CImageException) = 0;
	/** Save image with a specified filename. Overriding Function
	*	@param		fileName - Name of the File to be saved
	*	@return		None
	*	@exceptions	CImageException
	*/
	virtual void Save(CString fileName) throw(CImageException) = 0;
	/** Save image with a specified CFile pointer. Overriding Function
	*	@param		pFile - CFile pointer
	*	@return		None
	*	@exceptions	CImageException
	*/
	virtual void Save(CFile* pFile) throw(CImageException) = 0;
	/** Draw image to Screen with coordinate and scale ratio. Overriding Function
	*	@param		pdc - Device Context
	*	@param		x - x coordinate of destination image
	*	@param		y - y coordinate of destination image
	*	@param		scale - ratio of dest/source rectangle
	*	@param		dwROPCode - ROP code
	*	@return		None
	*	@exceptions	CImageException
	*/
	virtual void Draw(CDC* pdc, int x=0, int y=0, double scale = 1.0, DWORD dwROPCode = SRCCOPY) const throw(CImageException) = 0;
	/** Draw image to Screen with coordinate and fixed width/height. Overriding Function
	*	@param		pdc - Device Context
	*	@param		x - x coordinate of destination image
	*	@param		y - y coordinate of destination image
	*	@param		width - fixed width of destination image
	*	@param		height - fixed height of destination image
	*	@param		dwROPCode - ROP code
	*	@return		None
	*	@exceptions	CImageException
	*/
	virtual void Draw(CDC* pdc, int x, int y, int width, int height, DWORD dwROPCode = SRCCOPY) const throw(CImageException) = 0;
	/** Create image from data bits. Overriding Function
	*	@param		width - width of original image
	*	@param		height - height of original Image
	*	@param		pData - data bits. Contains rgb data(Pallette mode not supported)
	*	@return		None
	*	@exceptions	CImageException
	*/
	virtual void Create(int width, int height, LPBYTE pData, int bitCount=24) throw(CImageException) = 0;
	/** Returns width of Image. Overriding Function
	*	@param		None
	*	@return		width of Image
	*	@exceptions	None
	*/
	virtual UINT GetWidth() const throw() = 0;
	/** Returns height of Image. Overriding Function
	*	@param		None
	*	@return		height of Image
	*	@exceptions	None
	*/
	virtual UINT GetHeight() const throw() = 0;
	/** Returns BitPerPixel value of current Image
	*	@param		None
	*	@return		BitPerPixel value
	*	@exceptions	None
	*/
	virtual UINT GetBPP() const throw() = 0;
	/** Get COLORREF value of given pixel
	*	@param		x - x coordinate
	*	@param		y - y coordinate
	*	@return		COLORREF value of given pixel
	*	@exceptions None
	*/
	virtual COLORREF GetPixel(int x, int y) const throw() = 0;
	/** Get DibBits directly
	*	@param		None
	*	@return		LPBYTE	BYTE pointer of given Dib
	*	@exceptions None
	*/
	virtual LPBYTE GetDibBits() const throw() = 0;
};

#endif // !defined(AFX_IMAGE_H__D3142778_9555_4128_887F_104A5F9478E1__INCLUDED_)
