UrlCache : Tool to clear, display, search or delete Cookies, History and Temporary Internet WinINet url cache container entries
pierrelc@microsoft.com (Original idea Francois Bernis)
Version 1.27 April 2017
Options :  
Help : -h or -?

-c : To clear all cache entries of an url cache container , use the options listed below    
  -c:h        Clears previous Day History But Not Today (By Design)    
  -c:c        Clears Cookies    
  -c:t        Clears Temporary Internet Files    
  -c:a        Clears all  above  

  [SourceUrlName | LocalFileName | HeaderInfo | ExpireTime] text    
SourceUrlName and LocalFileName can mention different strings separated by *    
For example, to delete all entries in TIF containing wpad in the SourceUrlName field :       
  urlcache  -c:t SourceUrlName wpad    
To delete all cookies containing msdn or technet in the LocalFileName field :       
  urlcache  -c:c LocalFileName msdn*technet  

-l : To list  all cache entries of an url cache container, use the options listed below    
  -l:h        Lists all entries in History    
  -l:c        Lists all entries in Cookies    
  -l:t        Lists all entries in Temporary Internet Files    
  -l:a        Lists all above    
  -l:u        Lists all entries in EMIE User List    
  -l:s        Lists all entries in EMIE Site List    

 

-s : To search cache entries, use the options listed below    
  -s:h        Searches History    
  -s:c        Searches Cookies    
  -s:t        Searches Temporary Internet Files    
  -s:a        Searches all above    
  -s:u        Searches EMIE User List    
  -s:s        Searches EMIE Site List    

  [SourceUrlName | LocalFileName | HeaderInfo | ExpireTime] text    
  SourceUrlName and LocalFileName can mention different strings separated by *    
  For example, to search all entries in TIF containing wpad in the SourceUrlName field :       
    urlcache  -s:t SourceUrlName wpad    
  To search all entries in TIF containing wpad or pac in the SourceUrlName field :       
    urlcache  -s:t SourceUrlName wpad*pac    
  For example, to search all entries in TIF containing autoconfig in the HeaderInfo field :       
    urlcache  -s:t HeaderInfo application/x-ns-proxy-autoconfig  
 
   For -l and -s option, to prevent displaying one or more cache entries info use the switches below:
   -r:StructSize      for StructSize        
   -r:SourceUrlName   for SourceUrlName        
   -r:LocalFileName   for LocalFileName        
   -r:CacheEntryType  for CacheEntryType        
   -r:UseCount        for UseCount        
   -r:HitRate         for HitRate        
   -r:SizeLow         for SizeLow        
   -r:SizeHigh        for SizeHigh        
   -r:LastModifiedTime        for LastModifiedTime        
   -r:ExpireTime      for ExpireTime        
   -r:LastAccessTime  for LastAccessTime       
   -r:LastSyncTime    for LastSyncTime        
   -r:HeaderInfo      for HeaderInfo        
   -r:HeaderInfoSize  for HeaderInfoSize        
   -r:FileExtension   for FileExtension        
   -r:Expiration      for Expiration estimation        
   -r:a       every field except SourceUrlName   
   For example :       urlcache -r:StructSize -r:CacheEntryType will remove StructSize and CacheEntryType  
 
-d : To delete cache entries in an url cache container , use the options listed below    
  -d:h        Deletes History entries    
  -d:c        Deletes Cookie entries    
  -d:t        Deletes Temporary Internet Files entries    
  -d:a        Deletes all entries    
  -d:u        Deletes EMIE User List entries    
  -d:s        Deletes EMIE Site List entries    
  [SourceUrlName | LocalFileName | HeaderInfo | ExpireTime] text    
  Caution: no warning    
  SourceUrlName and LocalFileName can mention different strings separated by *   
  For example :    
  urlcache  -d:t SourceUrlName wpad    
  urlcache  -d:t HeaderInfo application/x-ns-proxy-autoconfig
