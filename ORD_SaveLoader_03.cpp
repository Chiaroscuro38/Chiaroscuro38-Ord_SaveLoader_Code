// ORD_SaveLoader_03.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "ORD_SaveLoader_03.h"
#include <fstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ������ ���� ���α׷� ��ü�Դϴ�.

CWinApp theApp;

using namespace std;

int Error();

bool SaveCheck();

bool Select(CString& str);

void Load(CString* FullPath);

void Save();

CString strBasePath; // ���α׷� ���� ���
CString strPath; // ���̺굥���� ���� ���
CString strName; // ���� + �÷��̾� �̸�
CString strID;	 // �÷��̾� �̸�
int		clearSaves;  // Ŭ���� Ƚ��

int main()
{
	// ���� ���α׷� ���
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);
	strBasePath = szFilePath;

	if (SaveCheck())
	{
		// ���� ����
		cout << endl << "���ʷ� ������ ���̺� ������ ã�� �ּ���" << endl << endl;
		
		// ���� ����
		CString fullPath;
		if (!Select(fullPath)) return Error(); // ���α׷� ������н� ����

		// ������ ���� ��ü ���
		wcout << "���� : " << (const TCHAR*)fullPath << endl;

		// ��� ���ڿ��� �����
		TCHAR szFullPath[MAX_PATH]{};
		TCHAR* pFP = fullPath.GetBuffer(fullPath.GetLength());
		_tcscpy_s(szFullPath, MAX_PATH, pFP);
		fullPath.ReleaseBuffer();

		// ������ �ִ� ���
		PathRemoveFileSpec(szFullPath);
		int pathSize = _tcslen(szFullPath);
		strPath = fullPath.Left(pathSize + 1);
		wcout << "��� : " << (const TCHAR*)strPath << endl;

		// ���� �̸�
		CString strRight = fullPath.Right(fullPath.GetLength() - pathSize - 1);
		strRight.TrimRight(L".txt");
		wcout << "���� : " << (const TCHAR*)strRight << endl << endl;

		// �̸� �и��ϱ�
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
		
		wcout << "�̸� : " << (const TCHAR*)strName << endl;
		wcout << "Ƚ�� : " << clearSaves << endl;
		
		Save();
	}
	else
	{
		// ����� ���ڿ��� �ҷ��ͼ� ���α׷� ����
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
	
	// ��� �˻�
	CFileFind finder;
	CString newPath = strPath;
	newPath += "*.txt";

	BOOL bExist = finder.FindFile(newPath);
	if (!bExist) return Error(); //��ΰ� ��ȿ���� ����

	//iSaves

	while (bExist) {
		bExist = finder.FindNextFile();

		// �̸� �и��ϱ�
		CString strTemp = finder.GetFileTitle();
		wcout << (const TCHAR*)strTemp << endl;

		//// ���� �̸�
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
	
	// ������ ����
	CString Path = strBasePath;
	Path += "\\��������.txt";
	HANDLE hFile = CreateFile(Path.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile){
		return Error();
	}
	
	// ��� �����	
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
		// MFC�� �ʱ�ȭ�մϴ�. �ʱ�ȭ���� ���� ��� ������ �μ��մϴ�.
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: ���� �ڵ带 �ʿ信 ���� �����մϴ�.
			wprintf(L"�ɰ��� ����: MFC�� �ʱ�ȭ���� ���߽��ϴ�.\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: ���� ���α׷��� ������ ���⿡�� �ڵ��մϴ�.
		}
	}
	else
	{
		// TODO: ���� �ڵ带 �ʿ信 ���� �����մϴ�.
		wprintf(L"�ɰ��� ����: GetModuleHandle ����\n");
		nRetCode = 1;
	}

	return nRetCode;
}

bool SaveCheck()
{
	// ��� �˻�
	CFileFind finder;
	CString newPath = strBasePath;
	newPath += "\\*.data";
	BOOL bExist = finder.FindFile(newPath);

	//��ΰ� ��ȿ���� ����
	if (!bExist) {
		cout << "���� ����\n";
		return true;
	}
	while (bExist) {
		cout << "���� ���� �ƴ�\n";
		return false;
	}

	return false;
}

bool Select(CString& str)
{
	CFileDialog dlg(TRUE, L"txt", L"ord_name_save", OFN_OVERWRITEPROMPT);

	// ��� �����
	TCHAR szFilePath[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, szFilePath);
	lstrcat(szFilePath, L"\\");

	// ���� ��� ���� �� ����
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
	
	// ��� �����
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

	// ��� ����
	WriteFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
	WriteFile(hFile, szPath, iSize, &dwbyte, nullptr);

	// �̸� ����
	WriteFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
	WriteFile(hFile, szFile, iSize, &dwbyte, nullptr);

	// ID ����
	WriteFile(hFile, &iSize, sizeof(int), &dwbyte, nullptr);
	WriteFile(hFile, szID, iSize, &dwbyte, nullptr);

	// Ƚ�� ����
	WriteFile(hFile, &clearSaves, sizeof(int), &dwbyte, nullptr);

	CloseHandle(hFile);

	return;
}

void Load(CString* FullPath)
{
	CString& fullpath = *FullPath;

	wcout << "��� : " << (const TCHAR*)fullpath << endl << endl;

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

	cout << "���̺� �ڵ带 �ҷ��Խ��ϴ�." << endl;
	cout << strText << endl;

	// Ŭ�� ���� ����
	::OpenClipboard(NULL);
	EmptyClipboard();

	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, 256);
	char *mem = (char*)GlobalLock(hglbCopy);
	strcpy_s(mem, 256, strText.c_str());
	GlobalUnlock(hglbCopy);
	::SetClipboardData(CF_TEXT, mem);
	CloseClipboard();
}
