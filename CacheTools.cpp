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

DWORD ErrorPrint();

void DisplayHelp()
{
	printf("UrlCache : Tool to clear, display, search or delete Cookies, History , Temporary Internet or any other existing WinINet Url Cache Container entries\n\n");
	printf("pierrelc@microsoft.com (Original idea Francois Bernis)\r\n");
	printf("Version 1.50 February 2018\r\n");
	printf("Uses WinINet Url cache APIs https://msdn.microsoft.com/en-us/library/windows/desktop/aa383928(v=vs.85).aspx \r\n");
	
	printf("Help : -h or -?\n\n");

	printf("-low To search low integrity containers (protected mode Internet Explorer)");
	printf("\tSee https://msdn.microsoft.com/en-us/library/bb250462(VS.85).aspx(d=robot) for more info on Low integrity store\r\n");

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
	printf("\tSourceUrlName and LocalFileName can mention different strings separated by *\n\n");

	printf("\tFor example, to search all entries in TIF containing wpad in the SourceUrlName field : \n");
	printf("\t   urlcache  -s:t SourceUrlName wpad\n\n");
	printf("\tTo search all entries in TIF containing autoconfig in the HeaderInfo field : \n");
	printf("\t   urlcache  -s:t HeaderInfo application/x-ns-proxy-autoconfig\n\n");
	printf("\tTo delete all entries in TIF containing wpad or pac in the SourceUrlName field : \n");
	printf("\t   urlcache  -d:t SourceUrlName wpad*pac\n\n");
	
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

	LPTSTR Ext;
	Ext = 0;

    inf = 0;

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		if (argc == 1)
		{
			DisplayHelp();
			exit(0L);
		}
		//first pass for low argument
		//first pass just for -r argument
		for (i = 1; i<argc; i++)
		{
			strcpy_s(arg, argv[i]);

			if (LoopStringUpper(arg, "-low") != 0)
			{
				CreateLowProcess();
				exit(0L);
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
					printf("Too many parameters. Paramater not used with -l parameter\r\n");
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
				//-s requires a source URL name
				/*if (argc != i+2) 
				{
					printf("Source URL name not found\r\n");
				 	printf("\nSyntax error please read help using -h \n\n");
					return nRetCode;
				}*/
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
					printf("\nSyntax error using -s option.please read help using -h \n\n");
					exit(-1L);
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
				else if (!_strcmpi(argv[i + 1], "ExpireTime"))
				{
					Cache.m_bSearch_ExpireTime = 1;
				}
				else
				{
					printf("\nSyntax error using -s option.please read help using -h \n\n");
					exit(-1L);
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
					printf("Too many parameters. Paramater not needed with -c parameter\r\n");
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
	lpwszCommandLine += 8+wcslen(wszProcessName);

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