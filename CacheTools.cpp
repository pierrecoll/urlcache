#include "stdafx.h"
#include "CacheTools.h"
#include "ClearCache.h"
#include "Cache.h"
#include "cacheinfo.h"
#include <stdio.h>
#include <sddl.h>


//////////////////////////////////////////////////////////////////////
// Classe : Main
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Methode : DisplayHelp
// Resume : Display Help
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
//1.21 Adding call to SearchEmieUSerList and SearchEmieSiteList in SearchAll 
//1.22 Adding search for Expire time
//1.23 Adding expiration information
//1.24 fix in options help
//1.25 160614 adding support for separator * in sourceurlname and localfilename
//1.26 160615 adding option for selective deletion
//1.27 Fixing -r:HeaderInfoSize. Adding output of number of elements for l: command.
//1.28 Adding support for DOMStore
//1.31 Adding support for iedownload
//1.32 major rewrite of functions/ no added features
//1.40 adding support for using container name / removing specific containers
//1.50 adding support for low integrity 
//1.51 using source URL Name as default criteria for search
//1.52 checking integrity level for -low option
//1.53 200113 adding support for separator * in sourceurlname and localfilename
//1.54 210114 adding support for separator + in sourceurlname and localfilename

DWORD ErrorPrint();
DWORD GetProcessIntegrityLevel();

void DisplayHelp()
{
	printf("UrlCache : Tool to clear, display, search or delete Cookies, History , Temporary Internet or any other existing WinINet Url Cache Container entries\n\n");
	printf("pierrelc@microsoft.com (Original idea Francois Bernis)\r\n");
	printf("Version 1.54 January 2021\r\n");
	printf("Uses WinINet Url cache APIs https://msdn.microsoft.com/en-us/library/windows/desktop/aa383928(v=vs.85).aspx \r\n");
	printf("Starting with Windows10 version 1709 aka RS3, cookies are stored in a database:\r\n");
	printf("you will have to use internetgetcookie.exe from https://aka.ms/browsertools to display cookie content\r\n");
	
	printf("Help : -h or -?\n\n");

	printf("-low To search low integrity containers (protected mode Internet Explorer)\r\n");
	printf("\t Cannot be used when running as High Integrity (Administrative command prompt)\r\n");
	printf("\tSee https://msdn.microsoft.com/en-us/library/bb250462(VS.85).aspx(d=robot) for more info on Low integrity store\r\n\n");

	printf("General syntax : -Action:Container for the action\n\n");

	printf("Actions :\n");
	printf(" \t-c clears all cache entries for the given cache container\n");
	printf(" \t-l lists  all cache entries for the given cache container\n");
	printf(" \t-s search cache entries in the given cache container using given criteria\n");
	printf(" \t-d delete cache entries in the given cache container using given criteria\n\n");

	printf("Caution: no warning for detete action!\n\n");

	printf("Containers:\n");
	printf(" \t:h History container\n");
	printf(" \t:c Cookie container\n");
	printf(" \t:t Temporary Internet Files container\n");
	printf(" \t:name of an existing container for example iecompat\n\n");

	printf("Search criteria:\n");
	printf("\t[SourceUrlName | LocalFileName | HeaderInfo | ExpireTime] text\n");
	printf("\tSourceUrlName and LocalFileName can mention different strings separated by * or + meaning OR or AND\n\n");

	printf("\tFor example, to search all entries in TIF containing wpad in the SourceUrlName field : \n");
	printf("\t   urlcache  -s:t SourceUrlName wpad\n\n");
	printf("\tTo search all entries in TIF containing autoconfig in the HeaderInfo field : \n");
	printf("\t   urlcache  -s:t HeaderInfo application/x-ns-proxy-autoconfig\n\n");
	printf("\tTo delete all entries in TIF containing wpad OR pac in the SourceUrlName field : \n");
	printf("\t   urlcache  -d:t SourceUrlName wpad*pac\n\n");
	printf("\tTo delete all entries in TIF containing wpad AND pac in the LocalFileName field : \n");
	printf("\t   urlcache  -d:t LocalFileName wpad+pac\n\n");

	printf("Warning: Search is case sensitive!\n\n");

	printf("To prevent displaying one or more cache entries info use the switches below\n\n");
	printf("      -r:StructSize\t for StructSize\n");
	printf("      -r:SourceUrlName\t for SourceUrlName\n");
	printf("      -r:LocalFileName\t for LocalFileName\n");
	printf("      -r:CacheEntryType\t for CacheEntryType\n");
	printf("      -r:UseCount\t for UseCount\n");
	printf("      -r:HitRate\t for HitRate\n");
	printf("      -r:SizeLow\t for SizeLow\n");
	printf("      -r:SizeHigh\t for SizeHigh\n");
	printf("      -r:LastModifiedTime\t for LastModifiedTime\n");
	printf("      -r:ExpireTime\t for ExpireTime\n");
	printf("      -r:LastAccessTime\t for LastAccessTime\n");
	printf("      -r:LastSyncTime\t for LastSyncTime\n");
	printf("      -r:HeaderInfo\t for HeaderInfo\n");
	printf("      -r:HeaderInfoSize\t for HeaderInfoSize\n");
	printf("      -r:FileExtension\t for FileExtension\n");
	printf("      -r:Expiration\t for Expiration estimation\n");
	printf("      -r:a\t every field except SourceUrlName\n\n");
	printf("   For example : \n\n");
	printf("     urlcache -r:StructSize -r:CacheEntryType will remove StructSize and CacheEntryType\n\n");
	//add where StructSize/SourceurlName... is/contains/greater syntax

}
//////////////////////////////////////////////////////////////////////
// Methode : LoopStringUpper
// Resume : Like strstr but non case sensitive 
//			Return a pointer to the first occurrence of a search string in a string.
// In : *arg1 = string 
//		*arg2 = search string
// Out : pointer to the first occurrence
// Extract from http://msdn2.microsoft.com/en-us/library/z9da80kz(VS.80).aspx
//////////////////////////////////////////////////////////////////////
char * LoopStringUpper(const char *arg1, const char *arg2)
{
	char buff1[MAX_PATH];
	char buff2[MAX_PATH];

	strcpy_s(buff1,arg1);
	strcpy_s(buff2,arg2);

	_strupr_s(buff1);
	_strupr_s(buff2);

	return strstr(buff1,buff2);

}
//////////////////////////////////////////////////////////////////////
// Methode : main
// Resume : Main Function
// In : None
// Out : None
//////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	int nRetCode = 0;
	int i,inf;
	char arg[MAX_PATH];
	CClearCache ClearCache;
	CCache Cache;
	DWORD dwProcessIntegrityLevel;
	LPTSTR Ext;
	Ext = 0;

    inf = 0;

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
		exit(-1L);
	}

	if (argc == 1)
	{
		DisplayHelp();
		exit(0L);
	}
	dwProcessIntegrityLevel= GetProcessIntegrityLevel();
	//first pass for low argument
	//first pass just for -r argument
	for (i = 1; i<argc; i++)
	{
		strcpy_s(arg, argv[i]);

		if (LoopStringUpper(arg, "-low") != 0)
		{
			if (dwProcessIntegrityLevel == SECURITY_MANDATORY_HIGH_RID)
			{
				printf("-low option cannot be run from an administrative command prompt (High Integrity Level)\r\n");
				exit(-1L);
			}
			else if (dwProcessIntegrityLevel == SECURITY_MANDATORY_LOW_RID)
			{
				//�process already Low -> shift arguments
				
				for (;i < argc; i++)
				{
					argv[i]= argv[i + 1];
				}
				argc--;
			}
			else if (dwProcessIntegrityLevel == SECURITY_MANDATORY_MEDIUM_RID)
			{
				CreateLowProcess();
				exit(0L);
			}
			else
			{
				printf("Unexpected integity level for -low option\r\n");
			}
		}
	}

	//first pass just for -r argument
	for (i=1;i<argc;i++)
	{
		strcpy_s(arg,argv[i]);

		if (LoopStringUpper(arg,"-r") != 0)
		{
			Cache.m_b_Remove = 1;
			if (LoopStringUpper(arg,":StructSize"))
			{
				Cache.m_bDisp_dwStructSize		= 0;
			}
			else if (LoopStringUpper(arg,":SourceUrlName"))	
			{
				Cache.m_bDisp_lpszSourceUrlName	= 0;
			}
			else if (LoopStringUpper(arg,":LocalFileName"))	
			{
				Cache.m_bDisp_lpszLocalFileName	= 0;
			}
			else if (LoopStringUpper(arg,":CacheEntryType"))	
			{
				Cache.m_bDisp_CacheEntryType		= 0;
			}
			else if (LoopStringUpper(arg,":UseCount"))		
			{
				Cache.m_bDisp_dwUseCount			= 0;
			}
			else if (LoopStringUpper(arg,":HitRate"))		
			{
				Cache.m_bDisp_dwHitRate			= 0;
			}
			else if (LoopStringUpper(arg,":SizeLow"))		
			{
				Cache.m_bDisp_dwSizeLow			= 0;
			}
			else if (LoopStringUpper(arg,":SizeHigh"))		
			{
				Cache.m_bDisp_dwSizeHigh			= 0;
			}
			else if (LoopStringUpper(arg,":LastModifiedTime"))
			{
				Cache.m_bDisp_LastModifiedTime	= 0;
			}
			else if (LoopStringUpper(arg,":ExpireTime"))		
			{
				Cache.m_bDisp_ExpireTime			= 0;
			}
			else if (LoopStringUpper(arg,":LastAccessTime"))	
			{
				Cache.m_bDisp_LastAccessTime		= 0;	 
			}
			else if (LoopStringUpper(arg,":LastSyncTime"))	
			{
				Cache.m_bDisp_LastSyncTime		= 0;
			}
			//HeaderInfoSize needs to be before HeaderInfo because of the API we are using 
			else if (LoopStringUpper(arg, ":HeaderInfoSize"))
			{
				Cache.m_bDisp_dwHeaderInfoSize = 0;
			}
			else if (LoopStringUpper(arg,":HeaderInfo"))		
			{
				Cache.m_bDisp_lpHeaderInfo		= 0;
			}
			else if (LoopStringUpper(arg,":FileExtension"))	
			{
				Cache.m_bDisp_lpszFileExtension	= 0;
			}
			else if (LoopStringUpper(arg, ":Expiration"))
			{
				Cache.m_bDisp_Expiration = 0;
			}
			else if (LoopStringUpper(arg,":a"))	
			{
				//Cache.m_bDisp_lpszSourceUrlName = 0;
				Cache.m_bDisp_dwStructSize		= 0;
				Cache.m_bDisp_lpszFileExtension	= 0;
				Cache.m_bDisp_lpszLocalFileName	= 0;
				Cache.m_bDisp_CacheEntryType	= 0;
				Cache.m_bDisp_dwUseCount		= 0;
				Cache.m_bDisp_dwHitRate			= 0;
				Cache.m_bDisp_dwSizeLow			= 0;
				Cache.m_bDisp_dwSizeHigh		= 0;
				Cache.m_bDisp_LastModifiedTime	= 0;
				Cache.m_bDisp_ExpireTime		= 0;
				Cache.m_bDisp_LastAccessTime	= 0;	 
				Cache.m_bDisp_LastSyncTime		= 0;
				Cache.m_bDisp_lpHeaderInfo		= 0;
				Cache.m_bDisp_dwHeaderInfoSize	= 0;
				Cache.m_bDisp_lpszFileExtension	= 0;
				Cache.m_bDisp_Expiration		= 0;

			}
			else
			{
				printf("\nSyntax error please read help using -h \n\n");
				exit(0);
			}
		}		
	}


	for (i=1;i<argc;i++)
	{
		strcpy_s(arg,argv[i]);
		//_strupr_s(arg);
		char *Parameter = strstr(arg, ":");
			
		//Help
		if ((!strcmp(arg, "-h")) || (!strcmp(arg, "-?")) || (!Parameter))
		{
			DisplayHelp();
			exit(0L);
		}
		Parameter++;
		char CachePrefix[256] = "";
		lstrcpyn(CachePrefix, Parameter, 250);
		lstrcat(CachePrefix, ":");
		char Action[5]="";
		lstrcpyn(Action, arg,sizeof(Action));
		Action[3] = '\0';

		//List
		if (strcmp(Action,"-l:") == NULL) 
		{
			if (argc >= i+2) 
			{
				printf("Too many parameters. Parameter not needed with -l action\r\n");
				printf("\nPlease read help using -h \n\n");
				exit(0L);
			}				
			if      (strcmp(Parameter,"h") == 0)  Cache.Display(HISTORY_CACHE_PREFIX);
			else if (strcmp(Parameter, "c") == 0) Cache.Display(COOKIE_CACHE_PREFIX);
			else if (strcmp(Parameter, "t") == 0) Cache.Display(TEMPORARY_CACHE_PREFIX);
			else if (strcmp(Parameter, "a") == 0) Cache.DisplayAll();
			else Cache.Display(CachePrefix);
			exit(0L);				
		}

		//Search
		if (strcmp(Action, "-s:") == NULL)
		{
			if ((argv[i + 1]) == NULL)
			{
				printf("Search parameter not found\r\n");
				printf("\nSyntax error please read help using -h \n\n");
				return nRetCode;
			}
			if (!_strcmpi(argv[i+1],"SourceUrlName"))	
			{
				Cache.m_bSearch_lpszSourceUrlName	= 1;
			}
			else if (!_strcmpi(argv[i+1],"LocalFileName"))
			{
				Cache.m_bSearch_lpszLocalFileName	= 1;
			}
			else if (!_strcmpi(argv[i+1],"HeaderInfo"))
			{
				Cache.m_bSearch_lpHeaderInfo	= 1;
			}
			else if (!_strcmpi(argv[i+1], "ExpireTime"))
			{
				Cache.m_bSearch_ExpireTime = 1;
			}			
			else
			{
				printf("\nNo criteria given for search: searching Source URL Name by default\r\n");
				Cache.m_bSearch_lpszSourceUrlName = 1;
				printf("Searching for entries where Source URL Name contains : %s\r\n", argv[i + 1]);
				//FALSE as last  parameter means search only, no delete
				if (strcmp(Parameter, "h") == 0) Cache.Search(HISTORY_CACHE_PREFIX, argv[i + 1], FALSE);
				else if (strcmp(Parameter, "c") == 0) Cache.Search(COOKIE_CACHE_PREFIX, argv[i + 1], FALSE);
				else if (strcmp(Parameter, "t") == 0) Cache.Search(TEMPORARY_CACHE_PREFIX, argv[i + 1], FALSE);
				else if (strcmp(Parameter, "a") == 0) Cache.SearchAll(argv[i + 1], FALSE);
				else                                   Cache.Search(CachePrefix, argv[i + 1], FALSE);
				exit(0L);
			}
			printf("Searching for entries where %s contains : %s\r\n",argv[i+1], argv[i+2]);
			//FALSE as last  parameter means search only, no delete
			if      (strcmp(Parameter, "h") == 0) Cache.Search(HISTORY_CACHE_PREFIX,argv[i+2],FALSE);
			else if (strcmp(Parameter, "c") == 0) Cache.Search(COOKIE_CACHE_PREFIX,argv[i+2],FALSE);
			else if (strcmp(Parameter, "t") == 0) Cache.Search(TEMPORARY_CACHE_PREFIX,argv[i+2],FALSE);
			else if (strcmp(Parameter, "a") == 0) Cache.SearchAll(argv[i+2],FALSE);
			else                                   Cache.Search(CachePrefix, argv[i + 2], FALSE);
			exit(0L);
		}

		//Delete
		if (strcmp(Action, "-d:") == NULL)
		{
			if (!_strcmpi(argv[i+1],"SourceUrlName"))	
			{
				Cache.m_bSearch_lpszSourceUrlName	= 1;
			}
			else if (!_strcmpi(argv[i+1],"LocalFileName"))
			{
				Cache.m_bSearch_lpszLocalFileName	= 1;
			}
			else if (!_strcmpi(argv[i+1],"HeaderInfo"))
			{
				Cache.m_bSearch_lpHeaderInfo	= 1;
			}
			else if (!_strcmpi(argv[i + 1], "ExpireTime"))
			{
				Cache.m_bSearch_ExpireTime = 1;
			}
			else
			{
				printf("\nNo criteria given for delete: seaching Source URL Name\n\n");
				Cache.m_bSearch_lpszSourceUrlName = 1;
				printf("Deleting entries where Source URL Name contains : %s\r\n",  argv[i + 1]);
				if (strcmp(Parameter, "h") == 0) Cache.Search(HISTORY_CACHE_PREFIX, argv[i + 1], TRUE);
				else if (strcmp(Parameter, "c") == 0) Cache.Search(COOKIE_CACHE_PREFIX, argv[i + 1], TRUE);
				else if (strcmp(Parameter, "t") == 0) Cache.Search(TEMPORARY_CACHE_PREFIX, argv[i + 1], TRUE);
				else if (strcmp(Parameter, "a") == 0) Cache.SearchAll(argv[i + 1], TRUE);
				else                                   Cache.Search(CachePrefix, argv[i + 1], TRUE);
				exit(0);
			}
			printf("Deleting entries where %s contains : %s\r\n",argv[i+1], argv[i+2]);
			//FALSE as last  parameter means search only, no delete
			if      (strcmp(Parameter, "h") == 0) Cache.Search(HISTORY_CACHE_PREFIX, argv[i + 2], TRUE);
			else if (strcmp(Parameter, "c") == 0) Cache.Search(COOKIE_CACHE_PREFIX, argv[i + 2], TRUE);
			else if (strcmp(Parameter, "t") == 0) Cache.Search(TEMPORARY_CACHE_PREFIX, argv[i + 2], TRUE);
			else if (strcmp(Parameter, "a") == 0) Cache.SearchAll(argv[i + 2], TRUE);
			else                                   Cache.Search(CachePrefix, argv[i + 2], TRUE);
			exit(0);
		}

		//Clear
		if (LoopStringUpper(arg,"-c") != 0)
		{
			if (argc >= i + 2)
			{
				printf("Too many parameters. Paramater not needed with -c action\r\n");
				printf("\nPlease read help using -h\r\n");
				exit(0L);
			}
				
			if (LoopStringUpper(arg,":h") != 0)      ClearCache.Clear(HISTORY_CACHE_PREFIX);
			else if (LoopStringUpper(arg,"c") != 0) ClearCache.Clear(COOKIE_CACHE_PREFIX);
			else if (LoopStringUpper(arg,"t") != 0) ClearCache.Clear(TEMPORARY_CACHE_PREFIX);
			else if (LoopStringUpper(arg,"a") != 0) ClearCache.ClearAll();
			else                                     ClearCache.Clear(CachePrefix);

			printf("Number of entries found: %d. Number of entries deleted: %d\r\n", ClearCache.nEntries, ClearCache.nEntriesDeleted);
			exit(0);
		}
	}

	return nRetCode;
}

//From https://msdn.microsoft.com/en-us/library/bb250462(VS.85).aspx(d=robot)
void CreateLowProcess()
{
	BOOL bRet;
	HANDLE hToken;
	HANDLE hNewToken;

	// Notepad is used as an example
	WCHAR wszProcessName[MAX_PATH];
	GetModuleFileNameW(NULL, wszProcessName,MAX_PATH-1);
	WCHAR *lpwszCommandLine = GetCommandLineW();
	//remove -low from command line
	//lpwszCommandLine += 8+wcslen(wszProcessName);

	// Low integrity SID
	WCHAR wszIntegritySid[20] = L"S-1-16-4096";
	//WCHAR wszIntegritySid[129] = L"S-1-15-2-3624051433-2125758914-1423191267-1740899205-1073925389-3782572162-737981194-4256926629-1688279915-2739229046-3928706915";
	PSID pIntegritySid = NULL;

	TOKEN_MANDATORY_LABEL TIL = { 0 };
	PROCESS_INFORMATION ProcInfo = { 0 };
	STARTUPINFOW StartupInfo = { 0 };
	ULONG ExitCode = 0;

	if (OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED, &hToken))
	{
		if (DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
			SecurityImpersonation, TokenPrimary, &hNewToken))
		{
			if (ConvertStringSidToSidW(wszIntegritySid, &pIntegritySid))
			{
				TIL.Label.Attributes = SE_GROUP_INTEGRITY;
				TIL.Label.Sid = pIntegritySid;

				// Set the process integrity level
				if (SetTokenInformation(hNewToken, TokenIntegrityLevel, &TIL,
					sizeof(TOKEN_MANDATORY_LABEL) + GetLengthSid(pIntegritySid)))
				{
					// Create the new process at Low integrity
					bRet = CreateProcessAsUserW(hNewToken, wszProcessName,
						lpwszCommandLine, NULL, NULL, FALSE,
						0, NULL, NULL, &StartupInfo, &ProcInfo);
					if (!bRet)
					{
						printf("CreateProcessAsUserW failed\r\n");
						ErrorPrint();
					}
					else
					{
						printf("CreateProcessAsUser %ws with Low Integrity. Command line: %ws\r\n", wszProcessName, lpwszCommandLine);
					}
				}
				else
				{
					printf("SetTokenInformation failed\r\n");
					ErrorPrint();
				}
				LocalFree(pIntegritySid);
			}
			else
			{
				printf("ConvertStringSidToSidW failed\r\n");
				ErrorPrint();
			}
			CloseHandle(hNewToken);
		}
		else
		{
			printf("DuplicateTokenEx failed\r\n");
			ErrorPrint();
		}
		CloseHandle(hToken);
	}
	else
	{
		printf("OpenProcessToken failed\r\n");
		ErrorPrint();
	}
}

DWORD GetProcessIntegrityLevel()
{
	HANDLE hToken;
	HANDLE hProcess;

	DWORD dwLengthNeeded;
	DWORD dwError = ERROR_SUCCESS;

	PTOKEN_MANDATORY_LABEL pTIL = NULL;
	DWORD dwIntegrityLevel;

	hProcess = GetCurrentProcess();
	if (OpenProcessToken(hProcess, TOKEN_QUERY |
		TOKEN_QUERY_SOURCE, &hToken))
	{
		// Get the Integrity level.
		if (!GetTokenInformation(hToken, TokenIntegrityLevel,
			NULL, 0, &dwLengthNeeded))
		{
			dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER)
			{
				pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0,
					dwLengthNeeded);
				if (pTIL != NULL)
				{
					if (GetTokenInformation(hToken, TokenIntegrityLevel,
						pTIL, dwLengthNeeded, &dwLengthNeeded))
					{
						dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid,
							(DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid) - 1));

						if (dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
						{
							// Low Integrity
							wprintf(L"Running as Low Integrity Process\r\n");
						}
						else if (dwIntegrityLevel >= SECURITY_MANDATORY_MEDIUM_RID &&
							dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
						{
							// Medium Integrity
							wprintf(L"Running as Medium Integrity Process\r\n");
						}
						else if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID)
						{
							// High Integrity
							wprintf(L"Running as High Integrity Process\r\n");
						}
						return dwIntegrityLevel;
					}
					else
					{
						printf("GetProcessIntegrityLevel: GetTokenInformation failed\r\n");
						ErrorPrint();
					}
					LocalFree(pTIL);
				}
			}
		}
		CloseHandle(hToken);
	}
	else
	{
		printf("GetProcessIntegrityLevel: OpenProcessToken failed\r\n");
		ErrorPrint();
	}
	return -1;
}