// ORD_SaveLoader_03.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ORD_SaveLoader_03.h"
#include <fstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 유일한 응용 프로그램 개체입니다.

CWinApp theApp;

using namespace std;

int Error();

bool SaveCheck();

bool Select(CString& str);

void Load(CString* FullPath);

void Save();

CString strBasePath; // 프로그램 저장 경로
CString strPath; // 세이브데이터 저장 경로
CString strName; // 버전 + 플레이어 이름
CString strID;	 // 플레이어 이름
int		clearSaves;  // 클리어 횟수

int main()
{
	// 현재 프로그램 경로
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);
	strBasePath = szFilePath;

	if (SaveCheck())
	{
		// 최초 실행
		cout << endl << "최초로 선택할 세이브 파일을 찾아 주세요" << endl << endl;
		
		// 파일 선택
		CString fullPath;
		if (!Select(fullPath)) return Error(); // 프로그램 실행실패시 종료

		// 선택한 파일 전체 경로
		wcout << "선택 : " << (const TCHAR*)fullPath << endl;

		// 경로 문자열로 만들기
		TCHAR szFullPath[MAX_PATH]{};
		TCHAR* pFP = fullPath.GetBuffer(fullPath.GetLength());
		_tcscpy_s(szFullPath, MAX_PATH, pFP);
		fullPath.ReleaseBuffer();

		// 파일이 있는 경로
		PathRemoveFileSpec(szFullPath);
		int pathSize = _tcslen(szFullPath);
		strPath = fullPath.Left(pathSize + 1);
		wcout << "경로 : " << (const TCHAR*)strPath << endl;

		// 파일 이름
		CString strRight = fullPath.Right(fullPath.GetLength() - pathSize - 1);
		strRight.TrimRight(L".txt");
		wcout << "파일 : " << (const TCHAR*)strRight << endl << endl;

		// 이름 분리하기
		CString strTemp;
		int i = 0;
		while (FALSE != AfxExtractSubString(strTemp, strRight, i++, '_')) {
			if (1 == i) {
				strName = strTemp;
			}
			else if (2 == i) {
				strID = strTemp;
				strName = strName + '_' + strTemp + '_';
			}
			else if (3 == i) {
				clearSaves = _ttoi(strTemp);
			}
		}
		
		wcout << "이름 : " << (const TCHAR*)strName << endl;
		wcout << "횟수 : " << clearSaves << endl;
		
		Save();
	}
	else
	{
		// 저장된 문자열을 불러와서 프로그램 실행
		CString Path = strBasePath;
		Path += "\\save.data";
		HANDLE hFile = CreateFileW(Path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			return Error();
		}
		
		DWORD dwbyte = 0;

		int iSize;
		TCHAR szPath[MAX_PATH]{};
		TCHAR szFile[MAX_PATH]{};
		TCHAR szID[MAX_PATH]{};

		ReadFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
		ReadFile(hFile, szPath, iSize, &dwbyte, nullptr);
		ReadFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
		ReadFile(hFile, szFile, iSize, &dwbyte, nullptr);
		ReadFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
		ReadFile(hFile, szID, iSize, &dwbyte, nullptr);
		ReadFile(hFile, &clearSaves, sizeof(int), &dwbyte, nullptr);

		CloseHandle(hFile);

		strPath = szPath;
		strName = szFile;
		strID = szID;
	}
	
	// 경로 검색
	CFileFind finder;
	CString newPath = strPath;
	newPath += "*.txt";

	BOOL bExist = finder.FindFile(newPath);
	if (!bExist) return Error(); //경로가 유효하지 않음

	//iSaves

	while (bExist) {
		bExist = finder.FindNextFile();

		// 이름 분리하기
		CString strTemp = finder.GetFileTitle();
		wcout << (const TCHAR*)strTemp << endl;

		//// 파일 이름
		strTemp.TrimLeft(strName);
		strTemp.TrimRight(L".txt");
		int iCurSaves = _ttoi(strTemp);

		wcout << iCurSaves << endl;

		if (clearSaves <= iCurSaves)
			clearSaves = iCurSaves;
	}
	
	CString szFullPath = strPath + strName; 

	CString Temp;      
	Temp.Format(_T("%d"), clearSaves);

	szFullPath += Temp;
	szFullPath += ".txt";

	Load(&szFullPath);
	
	// 데이터 저장
	CString Path = strBasePath;
	Path += "\\유저정보.txt";
	HANDLE hFile = CreateFile(Path.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile){
		return Error();
	}
	
	// 경로 만들기	
	CString saves;
	saves.Format(_T("%d"), clearSaves);

	CString text = L"ID : ";
	text += strID;
	text += L" / Clears : ";
	text += saves;

	TCHAR szText[MAX_PATH]{};
	
	TCHAR* pP = text.GetBuffer(text.GetLength());
	_tcscpy_s(szText, MAX_PATH, pP);
	text.ReleaseBuffer();
	
	DWORD dwbyte = 0;
	int iSize = (MAX_PATH * sizeof(TCHAR));	
	WriteFile(hFile, szText, iSize, &dwbyte, nullptr);
	
	CloseHandle(hFile);

	return Error();
}

int Error()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: 오류 코드를 필요에 따라 수정합니다.
			wprintf(L"심각한 오류: MFC를 초기화하지 못했습니다.\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
		}
	}
	else
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		wprintf(L"심각한 오류: GetModuleHandle 실패\n");
		nRetCode = 1;
	}

	return nRetCode;
}

bool SaveCheck()
{
	// 경로 검색
	CFileFind finder;
	CString newPath = strBasePath;
	newPath += "\\*.data";
	BOOL bExist = finder.FindFile(newPath);

	//경로가 유효하지 않음
	if (!bExist) {
		cout << "최초 실행\n";
		return true;
	}
	while (bExist) {
		cout << "최초 실행 아님\n";
		return false;
	}

	return false;
}

bool Select(CString& str)
{
	CFileDialog dlg(TRUE, L"txt", L"ord_name_save", OFN_OVERWRITEPROMPT);

	// 경로 만들기
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);
	lstrcat(szFilePath, L"\\");

	// 파일 경로 설정 및 실행
	dlg.m_ofn.lpstrInitialDir = szFilePath;	
	if (IDOK == dlg.DoModal())
	{
		str = dlg.GetPathName();
		return true;
	}

	return false;
}

void Save()
{
	CString filePath = strBasePath;
	filePath += "\\save.data";
	
	HANDLE hFile = CreateFile(filePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return;
	}
	
	// 경로 만들기
	TCHAR szPath[MAX_PATH]{};
	TCHAR szFile[MAX_PATH]{};
	TCHAR szID[MAX_PATH]{};

	// szPath
	TCHAR* pP = strPath.GetBuffer(strPath.GetLength());
	_tcscpy_s(szPath, MAX_PATH, pP);
	strPath.ReleaseBuffer();

	// szFile
	pP		  = strName.GetBuffer(strName.GetLength());
	_tcscpy_s(szFile, MAX_PATH, pP);
	strName.ReleaseBuffer();

	// szID
	pP		  = strID.GetBuffer(strID.GetLength());
	_tcscpy_s(szID, MAX_PATH, pP);
	strID.ReleaseBuffer();

	DWORD dwbyte = 0;
	int iSize = (MAX_PATH * sizeof(TCHAR));

	// 경로 저장
	WriteFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
	WriteFile(hFile, szPath, iSize, &dwbyte, nullptr);

	// 이름 저장
	WriteFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
	WriteFile(hFile, szFile, iSize, &dwbyte, nullptr);

	// ID 저장
	WriteFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
	WriteFile(hFile, szID, iSize, &dwbyte, nullptr);

	// 횟수 저장
	WriteFile(hFile, &clearSaves, sizeof(int), &dwbyte, nullptr);

	CloseHandle(hFile);

	return;
}

void Load(CString* FullPath)
{
	CString& fullpath = *FullPath;

	wcout << "경로 : " << (const TCHAR*)fullpath << endl << endl;

	string strText;
	string line;
	ifstream file(fullpath); 
	if (file.is_open()) {
		while (getline(file, line)) {
			cout << line << endl;
			if (line.find("-") != string::npos)
			{
				int index_begin = line.find("-");
				strText = line.substr(index_begin, 17);
			}
		}
		file.close(); 
	}
	else {
		cout << "Unable to open file";
		return;
	}

	cout << "세이브 코드를 불러왔습니다." << endl;
	cout << strText << endl;

	// 클립 보드 복사
	::OpenClipboard(NULL);
	EmptyClipboard();

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, 256);
	char *mem = (char*)GlobalLock(hglbCopy);
	strcpy_s(mem, 256, strText.c_str());
	GlobalUnlock(hglbCopy);
	::SetClipboardData(CF_TEXT, mem);
	CloseClipboard();
}
