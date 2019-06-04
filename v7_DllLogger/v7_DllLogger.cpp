// v7_DllLogger.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

//****************************************

enum MessageMarker
{
	mmNone = 0,
	mmBlueTriangle,
	mmExclamation,
	mmExclamation2,
	mmExclamation3,
	mmInformation,
	mmBlackErr,
	mmRedErr,
	mmMetaData,
	mmUnderlinedErr,
	mmUnd
};


template <typename T> class list
{
	int Count;
	T* Elements;

public :void Add(T element)
	{
		Count++;
		Elements = (T*) realloc(Elements, sizeof(T) * Count + 1);
		Elements[Count - 1] = element;
	}

public :bool Contains(T element)
	{
		for(int i = 0; i < Count; i++)
		{
			if (lstrcmp(Elements[i], element) == 0)
				return true;
		}
		return false;
	}

	list()
	{
		Count = 0;
		Elements = (T*) malloc(sizeof(T) * Count + 1); 
	}
};


class IMPORT_1C CBkEndUI
{
public:
	CBkEndUI(CBkEndUI const &);	//37
	CBkEndUI(void);	//38
	CBkEndUI &  operator=(CBkEndUI const &);	//508
	virtual int  DoMessageBox(unsigned int,unsigned int,unsigned int);	//1238
	virtual int  DoMessageBox(char const* ,unsigned int,unsigned int);	//1239
	virtual void  DoStatusLine(char const*);	//1241
	virtual void  DoMessageLine(char const*, enum MessageMarker);	//1240
};

IMPORT_1C class CBkEndUI * __cdecl GetBkEndUI(void);
CBkEndUI*		pBkEndUI		= NULL;
//****************************************

typedef bool (WINAPI h_dllmain)(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved);
bool IsWrapped = false;
HWND  DskhWnd = GetDesktopWindow();
NOTIFYICONDATAA niData; 
PCHAR pDBPath;
list<PCHAR> DLLS; 

void log(PCHAR szBuf)
{
	FILE * m_fHandle = fopen("dll_log.csv", "a");
	if(m_fHandle != NULL)

	if(szBuf != NULL)
	{
		char buffer[80];
		time_t current = time(NULL);
		tm* timeinfo = localtime(&current);
		char* format = "%Y-%m-%d %I:%M:%S\0";
		strftime(buffer, 80, format, timeinfo);
		fprintf(m_fHandle, "%s;%s\n", &buffer, szBuf);

		fclose(m_fHandle);
	}
}



LPSTR GetDLLInfo(const CHAR *fileName)
{
	PLONG infoBuffer;  // �����, ���� ����� ������ ����������
	DWORD infoSize;    // � ��� ������
	
	LPSTR res = (LPSTR)malloc(sizeof(char) * 1024 * 1024);
	ZeroMemory(res, sizeof(char) *1024 * 1024);
	sprintf(res, "%s", fileName);
	//return res;


	struct LANGANDCODEPAGE { // ��������� ��� ��������� ������� ������� �� ������ ���������� �������� �����
		WORD wLanguage;
		WORD wCodePage;
	} *pLangCodePage;

	// ����� ����������, �������� ������� ����� �����������
	const CHAR *paramNames[] = {
			"CompanyName",
			"FileDescription",
			"FileVersion",
			"InternalName",
			"LegalCopyright",
			"LegalTradeMarks",
			"OriginalFilename",
			"ProductName",
			"ProductVersion",
			"Comments",
			"Author"
	};
	
	LPSTR paramNameBuf = (LPSTR)malloc(sizeof(char)*10000);// (LPSTR)malloc(sizeof(char)*1000); // ����� ��������� ��� ���������
	LPSTR paramValue = (LPSTR)malloc(sizeof(char)*10000);; //(LPSTR)malloc(sizeof(char)*1000);   // ����� ����� ��������� �� �������� ���������, ������� ��� ������ ������� VerQueryValue 
	UINT paramSz;       // ����� ����� ����� �������� ���������, ������� ��� ������ ������� VerQueryValue 
	
	// �������� ������ ���������� � ������ �����
	infoSize = GetFileVersionInfoSizeA(fileName, NULL);
	if ( infoSize > 0 )
	{
		// �������� ������
		infoBuffer = (PLONG) malloc(infoSize);
		
		// �������� ����������
		if ( 0 != GetFileVersionInfoA(fileName, NULL, infoSize, infoBuffer) )
		{
			// ���������� ��������� ������� � ���� "\StringFileInfo\�������_��������\���_���������
			// �.�. �� �� ����� ������� ������� � ����� ����������� (�������_��������) �������� ������� � �����,
			// �� ����� ���������� �� ���
			
			UINT cpSz;
			// �������� ������ ������� �������
			if ( VerQueryValueA(infoBuffer,                      // ��� �����, ���������� ����������
			                   "\\VarFileInfo\\Translation",// ����������� ��������� ����������
			                   (LPVOID*) &pLangCodePage,        // ���� ������� ������ ��� ��������� �� ������ ������������ ��� ������ 
			                   &cpSz) )                         // � ���� - ������ ���� ������ 
			{
				// ���������� ��� ������� ��������
				for (int cpIdx = 0; cpIdx < (int)(cpSz/sizeof(struct LANGANDCODEPAGE)); cpIdx++ )
				{
					// ���������� ����� ����������
					for (int paramIdx = 0; paramIdx < sizeof(paramNames)/sizeof(char*); paramIdx++)
					{
						// ��������� ��� ��������� ( \StringFileInfo\�������_��������\���_��������� )
						sprintf(paramNameBuf, "\\StringFileInfo\\%04x%04x\\%s", 
										pLangCodePage[cpIdx].wLanguage,  // ��, ��� ����� ������� ������ ��� 
										pLangCodePage[cpIdx].wCodePage,  // �����-�� ��������� ������� ��������
										paramNames[paramIdx]);
						// �������� �������� ���������
						if ( VerQueryValueA(infoBuffer, (LPCSTR)paramNameBuf, (LPVOID*)&paramValue, &paramSz))
						{
							//res = (char*) realloc(res, sizeof(char) * (strlen(res) + strlen(paramValue) + 2));
							sprintf(res, "%s;%s", res, paramValue);
						}
						else
						{
							//res = (char*) realloc(res, sizeof(char) * (strlen(res) + 2));
							sprintf(res, "%s;", res);
						}
					}
				}
			}
			else
				sprintf(res, "%s;;;;;;;;;;;;;;;;;;;;;;", res);
		}

		free(infoBuffer);
		free(paramNameBuf);
	}
	else
		sprintf(res, "%s;;;;;;;;;;;;;;;;;;;;;;", res);

		return res;
}

DWORD find_LdrpCallInitRoutine(HMODULE ImageBase)
{
	DWORD XXX = NULL;
	DWORD CodeBase;
	DWORD CodeSize;
	DWORD NtHeader;

	DWORD m_dwProtection;
	BOOL Found = FALSE;
	
	// ������� ������ ������ ���� � �� ������ �� PE-���������

	NtHeader = *(DWORD*)((BYTE*)ImageBase + 0x3C) + (DWORD)ImageBase;
	CodeBase = *(DWORD*)(NtHeader + 0x2C) + (DWORD)ImageBase;
	CodeSize = *(DWORD*)(NtHeader + 0x1C);

	VirtualProtect((BYTE*)CodeBase, CodeSize, PAGE_READONLY, &m_dwProtection);

	char* message = (char*)malloc(sizeof(char) * 255);
	wsprintfA(message, "v7_UserDefWorks.dll: NtHeader - 0x%X, CodeBase - 0x%X, CodeSize - 0x%X\r\n", NtHeader, CodeBase, CodeSize);
	OutputDebugStringA(message);

	OutputDebugStringA("v7_UserDefWorks.dll: �������� ����� ������ LdrpCallInitRoutine � ntdll.dll\r\n");
	// � ���������� �� ���� ������ � ������� ������ LdrpCallInitRoutine.
	//http://kitrap08.blogspot.ru/2011/04/blog-post.html
 
	for (BYTE* m_pFunc = (BYTE*)CodeBase; (int)m_pFunc < (CodeBase + CodeSize); m_pFunc++) 
	{
		if((*(DWORD*)m_pFunc) == 0xFFF48B53) // ������ LdrpCallInitRoutine � ntdll.dll
		{
			XXX = (DWORD)(m_pFunc - 5);
			Found=TRUE;
			break;
		}
	}
	VirtualProtect((BYTE*)CodeBase, CodeSize, m_dwProtection, &m_dwProtection);
	
	return XXX;
}

HICON GetIcon()
{
	HICON hIcon1;       // ���������� ������
	HINSTANCE hExe;     // ���������� ������������ .EXE �����
	HRSRC hResource;    // ���������� FindResource
	HRSRC hMem;         // ����������  LoadResource
	LPVOID lpResource;   // ��������� �� ������ �������
	int nID;            // ������������� (ID) �������
	hExe = GetModuleHandle(NULL);
	hResource = FindResource(hExe,  MAKEINTRESOURCE(1),  MAKEINTRESOURCE(RT_ICON));
	hMem =  (HRSRC)LoadResource(hExe, hResource);
	lpResource = LockResource(hMem);
	hIcon1 = CreateIconFromResourceEx((PBYTE) lpResource, SizeofResource(hExe, hResource), TRUE, 0x00030000, 32, 32, LR_DEFAULTCOLOR);
	return hIcon1; 
}

void Baloon(PCHAR Message, int init = NULL)
{
	//log(Message);

	if(init==1) 
	{   
		DskhWnd = GetDesktopWindow();
		RtlZeroMemory(&niData,sizeof(NOTIFYICONDATAA));
		niData.cbSize = sizeof(NOTIFYICONDATAA);
		niData.uID = 1;
		niData.uFlags = NIF_ICON;
		niData.hIcon =GetIcon(); 
		niData.hWnd = DskhWnd;
		Shell_NotifyIconA(NIM_DELETE,&niData);
		Shell_NotifyIconA(NIM_ADD,&niData);
	}

	if(init==-1) 
	{
		Baloon("1C ��������� ������.");
		Sleep(500);
		Shell_NotifyIconA(NIM_DELETE,&niData);
		ZeroMemory(&niData,sizeof(NOTIFYICONDATAA));
		return;
	}

//	if (PostMessages)pBkEndUI->DoStatusLine(Message);
	niData.uFlags = NIF_INFO;
	ZeroMemory(niData.szInfo,strlen(niData.szInfo));
	CopyMemory(niData.szInfo,Message,strlen(Message));
	Shell_NotifyIconA(NIM_MODIFY,&niData);
}

PCHAR Get1CBasePath()
{
	DWORD MyHandle = (DWORD)GetModuleHandle(NULL);
	PCHAR path = (PCHAR)(*(DWORD*)(MyHandle + 0x3EC44) + 0x190);
	return path;
}

PCHAR Get1CUserName()
{
//DWORD MyHandle = (DWORD)GetModuleHandleA("USERDEF.DLL");
	DWORD MyHandle = (DWORD)GetModuleHandle(NULL);
    PCHAR pUserName = (PCHAR) (*(DWORD*)(*(DWORD*)(MyHandle + 0x50C68) + 0x14)+0x140);
//	PCHAR pUserName = (PCHAR)(*(DWORD*)(MyHandle + 0x3E3F4));
//	PCHAR pUserName = (PCHAR)(*(DWORD*)(MyHandle + 0x3E3F4));
	return pUserName;
}

PCHAR Get1CUserFullName()
{
	DWORD MyHandle = (DWORD)GetModuleHandle(NULL);
	PCHAR path = (PCHAR)(*(DWORD*)(MyHandle + 0x3E3F4));

	return path;
}



bool _stdcall Hook_DllMain(h_dllmain InitRoutine, HINSTANCE DllBAse, DWORD fdwReason, LPVOID lpvReserved)
{
	bool Res;
	char* message = (char*)malloc(sizeof(char)*1000);
	LPSTR LibName = (char*)malloc(sizeof(char)*_MAX_PATH);
	PCHAR info = NULL;
	GetModuleFileNameA(DllBAse,LibName,sizeof(char)*_MAX_PATH);
	


	PathStripPathA(LibName);
	AnsiUpperBuff(LibName, strlen(LibName));
	switch( fdwReason ){
		case DLL_PROCESS_ATTACH:
			wsprintf(message,"��������� ���������� %s",LibName);
			break;
		case DLL_THREAD_ATTACH:         
			wsprintf(message,"����� ��������� ���������� %s",LibName);
			break;
		case DLL_THREAD_DETACH:
			wsprintf(message,"����� �������� ���������� %s",LibName);
			break;
		case DLL_PROCESS_DETACH:
			wsprintf(message,"��������� ���������� %s",LibName);
			break;
	} ;

	if(!DLLS.Contains(LibName))
	{
		DLLS.Add(LibName);
		log(GetDLLInfo(LibName));
	}

	Res = InitRoutine(DllBAse, fdwReason, lpvReserved); 

	Baloon(message);
	//if (pBkEndUI == NULL) 
	//{
	//	pBkEndUI = GetBkEndUI();
	//	pBkEndUI->DoStatusLine(message);
	//}
	//else
	//{
	//	try
	//	{

	//		OutputDebugStringA("v7_UserDefWorks.dll: ���������� ����� � ���� ���������");
	//		wsprintfA(message,"v7_UserDefWorks.dll: ������� ����: %s", Get1CBasePath());
	//		pBkEndUI->DoMessageLine(message,mmInformation);
	//		wsprintfA(message,"v7_UserDefWorks.dll: ������������: %s", Get1CUserName());
	//		pBkEndUI->DoMessageLine(message,mmInformation);
	//	}
	//	catch(const std::nullptr_t e)
	//	{
	//	}
	//}

	//if ((lstrcmpA(LibName,"USERDEF.DLL")==0)&&(fdwReason==DLL_PROCESS_ATTACH))
	//{
	//	OutputDebugStringA("v7_UserDefWorks.dll: ��������� USERDEF.DLL");
	//	OutputDebugStringA("v7_UserDefWorks.dll: �� ���� ����� ����� ������������� ��������� ��������");
	//	OutputDebugStringA("v7_UserDefWorks.dll: �������� DllMain USERDEF.DLL");
	//	Res = InitRoutine(DllBAse,fdwReason,lpvReserved); 
	//	//Res = true;
	//	
	//	pDBPath =  Get1CUserName();
	//	OutputDebugStringA("v7_UserDefWorks.dll: ����������� �� 20 ���.");

	//	//Sleep(20000);
	//	//Res = true;
	//	OutputDebugStringA("v7_UserDefWorks.dll: ������������������� USERDEF.DLL");
	//	return Res;
	//}

//	wsprintfA(message,"v7_UserDefWorks.dll: %s",message);
//	OutputDebugStringA(message);


	//if ((lstrcmpA(LibName,"TxtEdt.DLL")==0) && (fdwReason == DLL_PROCESS_ATTACH))
	//{
	//	Res = InitRoutine(DllBAse,fdwReason,lpvReserved); 
	//	pBkEndUI = GetBkEndUI();
	//	OutputDebugStringA("v7_UserDefWorks.dll: ���������� ����� � ���� ���������");
	//	wsprintfA(message,"v7_UserDefWorks.dll: ������� ����: %s",Get1CBasePath());
	//	//pBkEndUI->DoMessageLine(message,mmInformation);
	//	pBkEndUI->DoStatusLine(message);
	//	wsprintfA(message,"v7_UserDefWorks.dll: ������������: %s",Get1CUserName());
	//	//pBkEndUI->DoMessageLine(message,mmInformation);
	//	pBkEndUI->DoStatusLine(message);

	//	return Res;
	//}	
	
	
	return  Res;

}

UINT64 Originalbuf = NULL;
UINT64 XXX = NULL;


void Swap(UINT64* m_pFunc, UINT64 m_SwapBuf)
{
	if (!IsWrapped) 
	{
		DWORD m_dwProtection;
		UINT64 buf = *m_pFunc;
		VirtualProtect(m_pFunc, 8, PAGE_EXECUTE_READWRITE, &m_dwProtection);
		*m_pFunc = m_SwapBuf;
		m_SwapBuf = buf;
		VirtualProtect(m_pFunc, 8, m_dwProtection, &m_dwProtection);
		FlushInstructionCache(GetModuleHandleA(NULL), m_pFunc, 8);
		IsWrapped = !IsWrapped;
	}	
}

void DoWrap(DWORD dwrdMethodOffset, DWORD pNewFunc)
{
if (!IsWrapped)
	{
		UINT64* m_pFunc = reinterpret_cast<UINT64*>(dwrdMethodOffset);
		if (m_pFunc)
		{
			UINT64 m_SwapBuf = (static_cast<UINT64>(pNewFunc - reinterpret_cast<DWORD>(m_pFunc)-5))<<8 | 0xE9;
			Swap(m_pFunc,m_SwapBuf);
			Originalbuf = m_SwapBuf;
		}
	}
};

bool init()
{
	Baloon("", 1);
	log("Dll;CompanyName;FileDescription;FileVersion;InternalName;LegalCopyright;LegalTradeMarks;OriginalFilename;ProductName;ProductVersion;Comments;Author");

	DLLS = list<PCHAR>();

	HMODULE HNtdll = GetModuleHandleA("ntdll.dll");
	if (HNtdll == NULL)
	{
		OutputDebugStringA("v7_UserDefWorks.dll: �� ����� Handle ntdll.dll\r\n");
		return false;
	}

	OutputDebugStringA("v7_UserDefWorks.dll: ����� Handle ntdll.dll\r\n");
	XXX = find_LdrpCallInitRoutine(HNtdll);
	char* message = (char*)malloc(sizeof(char)*255);
	if (XXX != NULL) 
	{
		wsprintfA(message,"v7_UserDefWorks.dll: ����� ����� LdrpCallInitRoutine - 0x%X\r\n",XXX);
		OutputDebugStringA(message);
		DoWrap(XXX, (DWORD)&Hook_DllMain);
		OutputDebugStringA("v7_UserDefWorks.dll: ���������� �������� ������������� ���������\r\n");
		return true;
	}

	OutputDebugStringA("v7_UserDefWorks.dll: �� ����� ����� LdrpCallInitRoutine.\r\n");
	return false;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			return init();
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			
			Baloon("",-1);
			OutputDebugStringA("v7_UserDefWorks.dll: �������.");
			Swap((UINT64*)XXX,Originalbuf);
			break;
	}
	return TRUE;
}