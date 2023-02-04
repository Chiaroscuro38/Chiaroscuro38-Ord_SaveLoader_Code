// ORD_SaveLoader_03.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ORD_SaveLoader_03.h"
#include <fstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 프로그램 실행 초기
CString StartProgram();
bool IsSaveBeforeCheck();

// 유저 정보 가져오기
void InitUserData(CString& fullPath);
void Select_ORDSaveFile(CString& str);
bool Save_UserData();

bool Load_UserData();

// 유저 정보 갱신 및 코드 저장
bool GetNewestData();
bool Load_ORDSaveFile(CString& FullPath);
bool Save_UserInfo();



// 유일한 응용 프로그램 개체입니다.
CWinApp theApp;

using namespace std;

int Error();

// 프로그램이 존재하는 위치
CString path_ProgramExist; 
CString path_ORDSaveFile; 

// 저장될 정보들
CString Save_SaveFileName;
CString Save_UserID;	 
int		Save_ClearCount;  



int main()
{
	// 현재 프로그램 경로
	path_ProgramExist = StartProgram();
	bool IsSucces = true; // 오류체크

	// 유저의 정보를 가져옵니다.
	bool IsFirstStart = IsSaveBeforeCheck();
	if (IsFirstStart == true)
	{
		// 최초 실행시 ORD 세이브의 데이터 기반으로 유저데이터를 불러옵니다.

		cout << "연동할 세이브 데이터를 선택하세요. (Ord_SaveFile.txt)" << endl << endl;
		
		// 파일 선택
		CString fullPath;
		Select_ORDSaveFile(fullPath);
		if (fullPath.IsEmpty() == true) return Error();

		// 선택한 파일을 기반으로 최초 유저 정보를 생성습니다.
		InitUserData(fullPath);
		
		IsSucces = Save_UserData();
		if (IsSucces == false) return Error();

	}
	else if (IsFirstStart == false)
	{
		// 유저데이터를 불러옵니다.

		IsSucces = Load_UserData();
		if (IsSucces == false)  return Error();
	}
	


	// 유저정보를 기반으로 최신 ORD 세이브를 찾습니다.
	IsSucces = GetNewestData();
	if (IsSucces == false)  return Error();
	
	CString szFullPath = path_ORDSaveFile + Save_SaveFileName; 

	CString stdSaveCount;
	stdSaveCount.Format(_T("%d"), Save_ClearCount);
	szFullPath += stdSaveCount;
	szFullPath += ".txt";

	IsSucces = Load_ORDSaveFile(szFullPath);
	if (IsSucces == false)  return Error();


	// 사용자가 확인가능한 기록을 저장합니다
	IsSucces = Save_UserInfo();
	if (IsSucces == false)  return Error();
	
	return Error();
}

int Error()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
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

CString StartProgram()
{
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);
	CString path = szFilePath;

	return path;
}

bool IsSaveBeforeCheck()
{
	// 경로 검색
	CFileFind finder;
	CString newPath = path_ProgramExist;
	newPath += "\\*.data";

	BOOL bExist = finder.FindFile(newPath);

	//경로가 유효하지 않음
	if (!bExist) {
		cout << "최초로 실행했습니다."<< endl << endl;
		return true;
	}
	while (bExist) {
		cout << "이전 실행 데이터가 존재합니다." << endl << endl;
		return false;
	}

	return false;
}

void InitUserData(CString& fullPath)
{
	wcout << "선택 : " << (const TCHAR*)fullPath << endl;

	// 경로 문자열로 만들기
	TCHAR szFullPath[MAX_PATH]{};
	TCHAR* pFP = fullPath.GetBuffer(fullPath.GetLength());
	_tcscpy_s(szFullPath, MAX_PATH, pFP);
	fullPath.ReleaseBuffer();

	// 파일이 있는 경로
	PathRemoveFileSpec(szFullPath);
	int pathSize = _tcslen(szFullPath);
	path_ORDSaveFile = fullPath.Left(pathSize + 1);
	wcout << "경로 : " << (const TCHAR*)path_ORDSaveFile << endl;

	// 파일 이름
	CString strRight = fullPath.Right(fullPath.GetLength() - pathSize - 1);
	strRight.TrimRight(L".txt");
	wcout << "파일 : " << (const TCHAR*)strRight << endl << endl;

	// 이름 분리하기
	CString strTemp;
	int i = 0;
	while (FALSE != AfxExtractSubString(strTemp, strRight, i++, '_')) {
		if (1 == i) {
			Save_SaveFileName = strTemp;
		}
		else if (2 == i) {
			Save_UserID = strTemp;
			Save_SaveFileName = Save_SaveFileName + '_' + strTemp + '_';
		}
		else if (3 == i) {
			Save_ClearCount = _ttoi(strTemp);
		}
	}

	wcout << "이름 : " << (const TCHAR*)Save_SaveFileName << endl;
	wcout << "횟수 : " << Save_ClearCount << endl;
}

void Select_ORDSaveFile(CString& refStr)
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
		refStr = dlg.GetPathName();
	}
}

bool Save_UserData()
{
	CString filePath = path_ProgramExist;
	filePath += "\\save.data";

	HANDLE hFile = CreateFile(filePath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return false;
	}

	// 경로 만들기
	TCHAR szPath[MAX_PATH]{};
	TCHAR szFile[MAX_PATH]{};
	TCHAR szID[MAX_PATH]{};

	// szPath
	TCHAR* pP = path_ORDSaveFile.GetBuffer(path_ORDSaveFile.GetLength());
	_tcscpy_s(szPath, MAX_PATH, pP);
	path_ORDSaveFile.ReleaseBuffer();

	// szFile
	pP = Save_SaveFileName.GetBuffer(Save_SaveFileName.GetLength());
	_tcscpy_s(szFile, MAX_PATH, pP);
	Save_SaveFileName.ReleaseBuffer();

	// szID
	pP = Save_UserID.GetBuffer(Save_UserID.GetLength());
	_tcscpy_s(szID, MAX_PATH, pP);
	Save_UserID.ReleaseBuffer();

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
	WriteFile(hFile, &Save_ClearCount, sizeof(int), &dwbyte, nullptr);

	CloseHandle(hFile);

	return true;
}

bool Load_UserData()
{
	// 프로그램의 세이브 데이터로 유저정보를 찾아옴 
	CString Path = path_ProgramExist;
	Path += "\\save.data";
	HANDLE hFile = CreateFileW(Path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return false;
	}

	DWORD dwbyte = 0;

	// 길이 등 숫자 데이터
	int iTemp; 

	TCHAR szPath[MAX_PATH]{};
	TCHAR szFile[MAX_PATH]{};
	TCHAR szID[MAX_PATH]{};

	ReadFile(hFile, &iTemp, sizeof(int), &dwbyte, nullptr);
	ReadFile(hFile, szPath, iTemp, &dwbyte, nullptr);
	ReadFile(hFile, &iTemp, sizeof(int), &dwbyte, nullptr);
	ReadFile(hFile, szFile, iTemp, &dwbyte, nullptr);
	ReadFile(hFile, &iTemp, sizeof(int), &dwbyte, nullptr);
	ReadFile(hFile, szID, iTemp, &dwbyte, nullptr);
	ReadFile(hFile, &iTemp, sizeof(int), &dwbyte, nullptr);

	CloseHandle(hFile);

	path_ORDSaveFile = szPath;

	Save_SaveFileName = szFile;
	Save_UserID = szID;
	Save_ClearCount = iTemp;

	return true;
}

bool GetNewestData()
{
	CString newPath = path_ORDSaveFile;
	newPath += "*.txt";

	// 경로 검색
	CFileFind finder;
	BOOL bExist = finder.FindFile(newPath);
	// 탐색 결과 없음
	if (bExist == false) return false; 
	
	while (bExist) 
	{
		bExist = finder.FindNextFile();

		// 파일 이름
		CString strTemp = finder.GetFileTitle();
		wcout << (const TCHAR*)strTemp << endl;

		if (strTemp.Find(Save_SaveFileName) == -1)
		{
			continue;
		}
		else
		{
			// 세이브 카운트 검사
			strTemp.Replace(Save_SaveFileName, CString(" "));
			int iCurSaves = _ttoi(strTemp);
			cout << iCurSaves << " clear" << endl;

			if (Save_ClearCount > iCurSaves)
			{
				continue;
			}
			else
			{
				cout << "Newest! : " << iCurSaves << endl;
				Save_ClearCount = iCurSaves;
			}
		}
	}

	return true;
}

bool Load_ORDSaveFile(CString& FullPath)
{
	wcout << "경로 : " << (const TCHAR*)FullPath << endl << endl;

	string strText;
	string line;
	ifstream file(FullPath);

	string codeBegin("( \"-");
	string codeEnd("\" )");

	int i = 0;
	int j = 0;

	bool IsSucess = file.is_open();
	if (IsSucess == false) return false;
	while (getline(file, line)) {
		cout << line << endl;
		if (line.find("-") != string::npos)
		{
			int index_begin = line.find(codeBegin) + 3;
			int index_end = line.find(codeEnd);
			strText = line.substr(index_begin, index_end - index_begin);
		}
	}

	cout << "불러온 코드 : " << strText << endl;

	// 클립 보드 복사
	::OpenClipboard(NULL);
	EmptyClipboard();

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, 256);
	char *mem = (char*)GlobalLock(hglbCopy);
	strcpy_s(mem, 256, strText.c_str());
	GlobalUnlock(hglbCopy);
	::SetClipboardData(CF_TEXT, mem);
	CloseClipboard();

	return true;
}

bool Save_UserInfo()
{
	CString Path = path_ProgramExist;
	Path += "\\유저정보.txt";
	HANDLE hFile = CreateFile(Path.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile) return false;

	// 경로 만들기	
	CString saves;
	saves.Format(_T("%d"), Save_ClearCount);

	CString text = L"ID : ";
	text += Save_UserID;
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

	return true;
}
