// BaseException.h: interface for the CBaseException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEEXCEPTION_H__0902F453_576E_48BB_90ED_7FC4CA666A82__INCLUDED_)
#define AFX_BASEEXCEPTION_H__0902F453_576E_48BB_90ED_7FC4CA666A82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdexcept>
#include <string>
#include <iostream>
using namespace std;

#pragma warning( disable : 4290 ) // disable Exception Specification warning
#pragma warning( disable : 4996 ) // disable sprintf warning

#define LOCATION Location(__FILE__, __LINE__)

/** This function returns string object of Error filename and line number */
inline string Location(string file, int lineNo) {
	char buf[64];
	sprintf(buf, " line no. %d", lineNo);

	return file + buf;
}

/** Base class of All Exception Classes. 
*
*	BaseException and all derivative classes have 3 Constructs.
*
*	ex) You can throw BaseException object like this.
*
*	1. throw BaseException();	-> message would be "Exception"
*
*	2. throw BaseException(_T("Disaster")); -> message would be "Disaster"
*
*	3. throw BaseException(_T("Disaster"), LOCATION); ->message would be "Disaster SomeFile line no 11"
*/
class BaseException : public runtime_error
{
public:
	/** Default Constructor
	*	@param		None
	*	@exceptions	None
	*/
	BaseException() throw() : runtime_error("error") {}
	/** Constructor with string description
	*	@param		message - This should be a useful message for error tracking.
	*	@exceptions	None
	*/
	BaseException(string message) throw() : runtime_error(message) {}
	/** Constructor with string description and error location 
	*	@param		message - This should be a useful message for error tracking.
	*	@param		location - Location where Exception has occrurred. You can use LOCATION macro here.
	*	@exceptions	None
	*/
	BaseException(string message, string location) throw() : runtime_error(message+location) {}
	/** This function outputs the error message in some output stream
	*	@param		os - some output stream
	*	@return		None
	*	@exceptions	None
	*/
	void Log(ostream& os) throw() { os << what() << endl; }
};

#endif // !defined(AFX_BASEEXCEPTION_H__0902F453_576E_48BB_90ED_7FC4CA666A82__INCLUDED_)
