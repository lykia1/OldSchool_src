//Copyright[2002] MasangSoft
#if !defined(AFX_GAMEDATALAST1_H__A520AAFF_7E8F_4A82_813D_B008287952A3__INCLUDED_1)
#define AFX_GAMEDATALAST1_H__A520AAFF_7E8F_4A82_813D_B008287952A3__INCLUDED_1

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define  MAXBUFF  65536

class TotalHeader
{
public:
	int m_EncodeNum;
	int m_Identification;
	int m_FileSize;
	int m_DataNumber;
	int m_Parity;

	TotalHeader();
	~TotalHeader();
};


class DataHeader
{
public:
	int m_EncodeNum;
	int m_DataSize;
	int m_Parity;
	char m_FileName[10];
	char* m_pData;

	DataHeader();
	~DataHeader();
};

class CGameData  
{
public:

	CGameData() :
		pTotal_header { nullptr }, m_ZipFilePath { }, m_EncodeStrFilePath { }, m_EncodeString { },
		m_bEncode { false }, m_mapDataHeader { }, m_it { m_mapDataHeader.begin() } { }

	virtual ~CGameData() { for (auto data : m_mapDataHeader) SAFE_DELETE(data.second); SAFE_DELETE(pTotal_header); }


	bool SetFile(char *filename, bool encode, char* encodeString, int encodeSize, bool bAllLoading = true);

	int GetTotalNumber() const { return pTotal_header ? pTotal_header->m_DataNumber : 0; }
	
	const char *CGameData::GetZipFilePath() { return m_ZipFilePath; }

	// todo : (CGameData) replace usages of GetStartPosition and GetNext with AsEnumerable

	// Deprecated, these methods are ineffective, use CGameData::AsEnumerable() instead.
	[[deprecated("Use CGameData::AsEnumerable() instead.")]]
	DataHeader* GetStartPosition() { return (m_it = m_mapDataHeader.begin())->second; }

	// Deprecated, these methods are ineffective, use CGameData::AsEnumerable() instead.
	[[deprecated("Use CGameData::AsEnumerable() instead.")]]
	DataHeader* GetNext() { return (++m_it == m_mapDataHeader.end()) ? nullptr : m_it->second; }

	DataHeader* Find(const char* strName) { auto it = m_mapDataHeader.find(strName); return (it == m_mapDataHeader.end()) ? nullptr : it->second; }
	
	void SetEncode(bool encode) { m_bEncode = encode; }
	void SetEncodeString(char* szEncode, int size) { memcpy( m_EncodeString, szEncode, size); }
	
	DataHeader* FindFromFile(char* strName);

	static BOOL GetCheckSum(BYTE o_byObjCheckSum[32], int *o_pnFileSize, char* pFilePath);

	const map<string, DataHeader*>& AsEnumerable() const { return m_mapDataHeader; }

protected:
	TotalHeader* pTotal_header;
	char m_ZipFilePath[256];
	char m_EncodeStrFilePath[256];
	char m_EncodeString[256];
	bool m_bEncode;

	map<string, DataHeader*> m_mapDataHeader;
	map<string, DataHeader*>::iterator m_it;
	
	virtual bool make_parse_file_ext();
};


class CGameDataOpt : public CGameData
{
public:
	DataHeader* find_by_number(unsigned number);

protected:
	map<unsigned, DataHeader*> m_mapDataHeaderOpt;

	bool make_parse_file_ext() override;
};

#endif