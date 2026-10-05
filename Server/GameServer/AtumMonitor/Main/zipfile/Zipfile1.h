// Zipfile1.h: interface for the Zipfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPFILE1_H__C524021C_828B_4FC4_A47E_2C2B6C65ADD1__INCLUDED_)
#define AFX_ZIPFILE1_H__C524021C_828B_4FC4_A47E_2C2B6C65ADD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataHeader.h"
#include "TotalHeader.h"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>      /* Needed only for _O_RDWR definition */
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>


class Zipfile  
{
public:
	//CString* pData;
	char* pData[10000];
	TotalHeader *pTotal_header;
	DataHeader *pData_header;
	CString m_ZipFilePath;
    CString m_EncodeStrFilePath;
	CString m_EncodeString;

	Zipfile();
	virtual ~Zipfile();
	void make_header(CString openfile[], int FileCount); //inetpub added 'void' type for 'make_header'
	void make_zip_file(CString OpenFile[],CProgressCtrl&, BOOL bEncode); //inetpub added 'void' type for 'make_zip_file'
	void make_encode_file(CProgressCtrl&); //inetpub added 'void' type for 'make_encode_file'
};

#endif // !defined(AFX_ZIPFILE1_H__C524021C_828B_4FC4_A47E_2C2B6C65ADD1__INCLUDED_)


