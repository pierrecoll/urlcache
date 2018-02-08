#include "stdafx.h"
#include "CacheTools.h"
#include "ClearCache.h"
#include "Cache.h"
#include "cacheinfo.h"
#include <stdio.h>

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

void DisplayHelp()
{
	printf("UrlCache : Tool to clear, display, search or delete Cookies, History,DOMStore, EMIE and Temporary Internet WinINet url cache container entries\n\n");
	printf("pierrelc@microsoft.com (Original idea Francois Bernis)\r\n");
	printf("Version 1.33 February 2018\r\n");
	printf("Uses WinINet Url cache APIs https://msdn.microsoft.com/en-us/library/aa385473(v=vs.85).aspx\r\n");
	printf("To work with the cache files located in the Low integrity directory , you can copy urlcache.exe to a directory with Low integrity (like %%TEMP%%\\Low) and run urlcache.exe from this directory\r\n");
	printf("\tSee https://blogs.msdn.microsoft.com/ieinternals/2010/08/26/writing-files-from-low-integrity-processes/ for more info on Low integrity store\r\n");
	printf("\nOptions : \n\n");
	printf("  Help : -h or -?\n\n");
	printf("  -c : To clear all cache entries of an url cache container , use the options listed below\n\n");
	printf("    -c:h\tClears previous Day History But Not Today (By Design)\n");
	printf("    -c:c\tClears Cookies\n");
	printf("    -c:t\tClears Temporary Internet Files\n");
	printf("    -c:a\tClears all  above\n\n");
	printf("    [SourceUrlName | LocalFileName | HeaderInfo | ExpireTime] text\n");
	printf("    SourceUrlName and LocalFileName can mention different strings separated by *\n");
	printf("    For example, to delete all entries in TIF containing wpad in the SourceUrlName field : \n");
	printf("       urlcache  -c:t SourceUrlName wpad\n\n");
	printf("    To delete all cookies containing msdn or technet in the LocalFileName field : \n");
	printf("       urlcache  -c:c LocalFileName msdn*technet\n\n");

	printf("  -l : To list  all cache entries of an url cache container, use the options listed below\n\n");
	printf("    -l:h\tLists all entries in History\n");
	printf("    -l:c\tLists all entries in Cookies\n");
	printf("    -l:t\tLists all entries in Temporary Internet Files\n");
	printf("    -l:a\tLists all above\n\n");
	printf("    -l:u\tLists all entries in EMIE User List\n");
	printf("    -l:s\tLists all entries in EMIE Site List\n");
	printf("    -l:d\tLists all entries in DOMStore\n");
	printf("    -l:i\tLists all entries in iedownload\n");
	
	printf("    To prevent displaying one or more cache entries info use the switches below\n\n");
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
	printf("  -s : To search cache entries, use the options listed below\n\n");
	printf("    -s:h\tSearches History \n");
	printf("    -s:c\tSearches Cookies \n");
	printf("    -s:t\tSearches Temporary Internet Files\n");
	printf("    -s:a\tSearches all above\n");
	printf("    -s:u\tSearches EMIE User List\n");
	printf("    -s:s\tSearches EMIE Site List\n");	
	printf("    -s:d\tSearches DOMStore\n");
	printf("    -s:i\tSearches iedownload\n");

	printf("    [SourceUrlName | LocalFileName | HeaderInfo | ExpireTime] text\n");
	printf("    SourceUrlName and LocalFileName can mention different strings separated by *\n");
	printf("    For example, to search all entries in TIF containing wpad in the SourceUrlName field : \n");
	printf("       urlcache  -s:t SourceUrlName wpad\n\n");
	printf("    To search all entries in TIF containing wpad or pac in the SourceUrlName field : \n"); 
	printf("       urlcache  -s:t SourceUrlName wpad*pac\n\n");
	printf("    For example, to search all entries in TIF containing autoconfig in the HeaderInfo field : \n");
	printf("       urlcache  -s:t HeaderInfo application/x-ns-proxy-autoconfig\n\n");
	
	printf("  -d : To delete cache entries in an url cache container , use the options listed below\n\n");
	printf("    -d:h \tDeletes History entries\n");
	printf("    -d:c \tDeletes Cookie entries\n");
	printf("    -d:t \tDeletes Temporary Internet Files entries\n");
	printf("    -d:a \tDeletes all entries\n");
	printf("    -d:u \tDeletes EMIE User List entries\n");
	printf("    -d:s \tDeletes EMIE Site List entries\n");		
	printf("    -d:d \tDeletes DOMStore entries\n");
	printf("    -d:i \tDeletes iedownload entries\n");

	printf("    [SourceUrlName | LocalFileName | HeaderInfo | ExpireTime] text\n");
	printf("    Caution: no warning\n");
	printf("    SourceUrlName and LocalFileName can mention different strings separated by *\n");
	printf("   For example : \n\n");
	printf("    urlcache  -d:t SourceUrlName wpad\n");
	printf("    urlcache  -d:t HeaderInfo application/x-ns-proxy-autoconfig\n\n");


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
		if (argc==1) DisplayHelp(); 

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
			_strupr_s(arg);


			//Help
			if ((LoopStringUpper(arg,"-h") != NULL) || (LoopStringUpper(arg,"-?") != NULL))
			{
				DisplayHelp();
			};

			//List
			if (LoopStringUpper(arg,"-l") != NULL) 
			{
				if (argc >= i+2) 
				{
					printf("Too many parameters. Url not used with -l parameter\r\n");
				 	printf("\nPlease read help using -h \n\n");
					return nRetCode;
				}				
				if (LoopStringUpper(arg,":h") != NULL) Cache.Display(HISTORY_CACHE_PREFIX);
				else if (LoopStringUpper(arg,":c") != 0) Cache.Display(COOKIE_CACHE_PREFIX);
				else if (LoopStringUpper(arg,":t") != 0) Cache.Display(TEMPORARY_CACHE_PREFIX);
				else if (LoopStringUpper(arg,":a") != 0) Cache.DisplayAll();
				else if (LoopStringUpper(arg, ":u") != 0) Cache.Display(EMIE_USERLIST_CACHE_PREFIX);
				else if (LoopStringUpper(arg, ":s") != 0) Cache.Display(EMIE_SITELIST_CACHE_PREFIX);
				else if (LoopStringUpper(arg, ":d") != 0) Cache.Display(DOMSTORE_CACHE_PREFIX);
				else if (LoopStringUpper(arg, ":i") != 0) Cache.Display(IEDOWNLOAD_CACHE_PREFIX);
				else
				{
					printf("\nSyntax error please read help using -h \n\n");
					exit(0);
				}
			}

			//Search
			if (LoopStringUpper(arg,"-s") != NULL)
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
				if (LoopStringUpper(arg,":h") != NULL) Cache.Search(HISTORY_CACHE_PREFIX,argv[i+2],FALSE);
				else if (LoopStringUpper(arg,":c") != 0) Cache.Search(COOKIE_CACHE_PREFIX,argv[i+2],FALSE);
				else if (LoopStringUpper(arg,":t") != 0) Cache.Search(TEMPORARY_CACHE_PREFIX,argv[i+2],FALSE);
				else if (LoopStringUpper(arg,":a") != 0) Cache.SearchAll(argv[i+2],FALSE);
				else if (LoopStringUpper(arg, ":u") != 0) Cache.Search(EMIE_USERLIST_CACHE_PREFIX, argv[i + 2], FALSE);
				else if (LoopStringUpper(arg, ":s") != 0) Cache.Search(EMIE_SITELIST_CACHE_PREFIX, argv[i + 2], FALSE);
				else if (LoopStringUpper(arg, ":d") != 0) Cache.Search(DOMSTORE_CACHE_PREFIX,argv[i + 2], FALSE);
				else if (LoopStringUpper(arg, ":i") != 0) Cache.Search(IEDOWNLOAD_CACHE_PREFIX, argv[i + 2], FALSE);
				else
				{
					printf("\nSyntax error please read help using -h \n\n");
					exit(0);
				}
			}

			//Delete
			if (LoopStringUpper(arg,"-d") != NULL)
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
				if (LoopStringUpper(arg, ":h") != NULL) Cache.Search(HISTORY_CACHE_PREFIX, argv[i + 2], TRUE);
				else if (LoopStringUpper(arg, ":c") != 0) Cache.Search(COOKIE_CACHE_PREFIX, argv[i + 2], TRUE);
				else if (LoopStringUpper(arg, ":t") != 0) Cache.Search(TEMPORARY_CACHE_PREFIX, argv[i + 2], TRUE);
				else if (LoopStringUpper(arg, ":a") != 0) Cache.SearchAll(argv[i + 2], TRUE);
				else if (LoopStringUpper(arg, ":u") != 0) Cache.Search(EMIE_USERLIST_CACHE_PREFIX, argv[i + 2], TRUE);
				else if (LoopStringUpper(arg, ":s") != 0) Cache.Search(EMIE_SITELIST_CACHE_PREFIX, argv[i + 2], TRUE);
				else if (LoopStringUpper(arg, ":d") != 0) Cache.Search(DOMSTORE_CACHE_PREFIX, argv[i + 2], TRUE);
				else if (LoopStringUpper(arg, ":i") != 0) Cache.Search(IEDOWNLOAD_CACHE_PREFIX, argv[i + 2], TRUE);
				else
				{
					printf("\nSyntax error please read help using -h \n\n");
					exit(0);
				}

			}

			//Clear
			if (LoopStringUpper(arg,"-c") != 0)
			{
				//160615 adding option for selective deletion
				BOOL bSelectiveDelete = FALSE;
				if ((argv[i + 1]) != NULL)
				{
					bSelectiveDelete = TRUE;
					if (!_strcmpi(argv[i + 1], "SourceUrlName"))
					{
						ClearCache.m_bSearch_lpszSourceUrlName = 1;
					}
					else if (!_strcmpi(argv[i + 1], "LocalFileName"))
					{
						ClearCache.m_bSearch_lpszLocalFileName = 1;
					}
					else if (!_strcmpi(argv[i + 1], "HeaderInfo"))
					{
						ClearCache.m_bSearch_lpHeaderInfo = 1;
					}
					else if (!_strcmpi(argv[i + 1], "ExpireTime"))
					{
						ClearCache.m_bSearch_ExpireTime = 1;
					}
					else
					{
						printf("\nSyntax error using -c option.Please read help using -h\n\n");
						exit(-1L);
					}
				}
				
				if (LoopStringUpper(arg,":h") != 0) ClearCache.Clear(HISTORY_CACHE_PREFIX, bSelectiveDelete, argv[i + 2]);
				else if (LoopStringUpper(arg,":c") != 0) ClearCache.Clear(COOKIE_CACHE_PREFIX,  bSelectiveDelete,argv[i+2]);
				else if (LoopStringUpper(arg,":t") != 0) ClearCache.Clear(TEMPORARY_CACHE_PREFIX, bSelectiveDelete, argv[i + 2]);
				else if (LoopStringUpper(arg,":a") != 0) ClearCache.ClearAll();
				else
				{
					printf("\nSyntax error please read help using -h \n\n");
					exit(0);
				}		
				printf("Number of entries found: %d. Number of entries deleted: %d\r\n", ClearCache.nEntries, ClearCache.nEntriesDeleted);
			}
		}
	}
	return nRetCode;
}

