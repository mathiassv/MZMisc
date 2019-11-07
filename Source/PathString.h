#pragma once


#ifndef STL_string
  #define STL_string std::string
  #ifndef _UNICODE
    typedef STL_string	 STLString;
  #endif
#endif

#ifndef STL_wstring
  #define STL_wstring std::wstring
  #ifdef _UNICODE
    typedef STL_wstring  STLString;
  #endif
#endif


class PathString
{
public:
  PathString()
  {

  }

	PathString(const STLString& s)
	{
		SplitPath(s.c_str());
	}

  PathString(const TCHAR* s)
	{
		SplitPath(s);
	}

  PathString(const TCHAR* path, const TCHAR* szFilename)
  {
    if (path)
    {
      m_path = path;
      AddTrailingSlash(m_path);
    }


    if (szFilename)
      m_filename = szFilename;
  }
  PathString& operator=(const TCHAR* s)
  {
    SplitPath(s);
    return *this;
  }

  void Init(const STLString& s)
  {
    SplitPath(s.c_str());
  }

  bool SamePath(const PathString& otherPath)
  {
    const TCHAR* szOtherPath = otherPath.GetPath();
    const TCHAR* szThisPath = GetPath();
    if (_tcsicmp(szOtherPath, szThisPath) == 0)
      return true;
    return false;
  }
  void RemoveTrailingSlashFromPath()
  {
    if (m_path.empty())
      return;

    if (m_path[m_path.length() - 1] == '\\')
      m_path.erase(m_path.length() - 1);
  }

  size_t GetPathLength() { return m_path.length(); }
	const TCHAR* GetPath() const { return m_path.c_str(); }
	const TCHAR* GetFileName() const { return m_filename.c_str(); }
	const TCHAR* GetFileExtension() // CAN BE NULL
	{
			return _tcsrchr(m_filename.c_str(), '.');
	}

  // get filename without the extension
  STLString GetFileNamePart()
  {
    STLString::size_type pos = m_filename.find_last_of('.');
    if(pos != STLString::npos)
    {
      return m_filename.substr(0, pos);
    }
    return m_filename;
  }

  STLString GetFullFilename()
  {
    STLString path = m_path;
    xPathAppend(path, m_filename.c_str());
    return path;

  }

  STLString GetDevice(const TCHAR* szRelativePath)
  {
    STLString path = szRelativePath;
    STLString::size_type pos = path.find_first_of(':');
    if(pos == STLString::npos)
      return _T("");

    path = path.substr(0, pos);
    // make sure there is not slash in device
    pos = path.find_first_of('\\');
    if(pos != STLString::npos)
      return _T("");

    pos = path.find_first_of('/');
    if(pos != STLString::npos)
      return _T("");

    return path;
  }

  STLString GetAbsolutePath(const TCHAR* szRelativePath)
  {
    TCHAR	szAbsolutePath[2048];

    // Check if szRelativePath has a device or UNC beginning. if it does then it is not a path
    // that is relative to GetPath, then it is relative to is self.
    bool bRelativeItself = false;
    if(szRelativePath[0] == '\\' && szRelativePath[1] == '\\')
      bRelativeItself = true;
    else if(szRelativePath[1] == ':' && szRelativePath[0] >= 'a' && szRelativePath[0] <= 'z')
      bRelativeItself = true;
    else if(szRelativePath[1] == ':' && szRelativePath[0] >= 'A' && szRelativePath[0] <= 'Z')
      bRelativeItself = true;
    else if(szRelativePath[1] == ':' && szRelativePath[0] >= '0' && szRelativePath[0] <= '9') // special num path supported by MC
      bRelativeItself = true;

    // check for virtual device.  like FTP: REG: and so on.
    if(bRelativeItself == false)
    {
      STLString device = GetDevice(szRelativePath);
      if(!device.empty())
      {
        bRelativeItself = true; 
      }
    }

    if(bRelativeItself == false)
    {
      STLString strPath = GetPath();
      strPath += szRelativePath;
      if (!PathCanonicalize(szAbsolutePath, strPath.c_str())) 
        return _T("");

      return AddTrailingSlash(szAbsolutePath);
    }

    // it is relative it self so ignore the "current path"
    if (!PathCanonicalize(szAbsolutePath, szRelativePath)) 
      return _T("");

    return AddTrailingSlash(szAbsolutePath);
  }

  // if bFodler == true then it will make sure that the path ends with "\\"
  static bool CreateUNCPath(OUT STLString& uncPath, const TCHAR* szNormalPath, bool bFolder = true)
  {
    if (szNormalPath == nullptr || szNormalPath[0] == '\0')
      return false;

    if (szNormalPath[0] == '\\' && szNormalPath[1] == '\\')
    {
      uncPath = _T("\\\\?\\UNC\\");
      uncPath += szNormalPath + 2;
    }
    else
    {
      uncPath = _T("\\\\?\\");
      uncPath += szNormalPath;
    }

    if (bFolder)
      uncPath += _T("\\");

    return true;
  }

  static bool PathIsDirectoryEmptyEx2(STLString& strPath)
  {
    WIN32_FIND_DATA findData;

    xPathAppend(strPath, _T("*.*"));
    
    bool retVal = false;
    HANDLE hFind = ::FindFirstFile(strPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
      return false;

    // if anything else then "." or ".." is found. then do not return true.
    if ((findData.cFileName[0] == '.' && findData.cFileName[1] == '\0') || (findData.cFileName[0] == '.' && findData.cFileName[1] == '.'&& findData.cFileName[2] == '\0'))
    {
      if (!::FindNextFile(hFind, &findData))
        retVal = true;

      if ((findData.cFileName[0] == '.' && findData.cFileName[1] == '\0') || (findData.cFileName[0] == '.' && findData.cFileName[1] == '.'&& findData.cFileName[2] == '\0'))
      {
        if (!::FindNextFile(hFind, &findData))
          retVal = true;
      }
    }

    FindClose(hFind);
    return retVal;
  }

  static bool PathIsQuoted(const STLString& path)
  {
    if (path.size() < 2)
      return false;

    if (path[0] == '"')
    {
      if (path[path.size() - 1] == '"')
      {
        return true;
      }
    }
    return false;
  }

  static STLString StipQuotes(const STLString& path)
  {
    if (PathIsQuoted(path))
    {
      return path.substr(1, path.size() - 2);
    }
    return path;
  }

  static STLString QuotePath(const STLString& s)
  {
    return _T("\"") + s + _T("\"");
  }

  // PathString::RelativeToAbsolutePath( "{$mcpath} , "c:\\MC\\Root\\" );
  static STLString RelativeToAbsolutePath(const STLString& refPath, const TCHAR* szAppPath)
  {
    if (PathIsQuoted(refPath))
    {
      STLString unquotedPath = StipQuotes(refPath);
      unquotedPath = RelativeToAbsolutePath(unquotedPath, szAppPath);
      return QuotePath(unquotedPath);
    }

    STLString absolutePath;

    if (szAppPath == nullptr)
    {
      absolutePath = refPath;
      return absolutePath;
    }

    if (_tcsncmp(refPath.c_str(), _T("..\\"), 3) == 0)
    {
      // Create absolute from relative path
      PathString path(szAppPath, _T(""));
      PathString relativePath(refPath);
      absolutePath = path.GetAbsolutePath(relativePath.GetPath());
      if (!absolutePath.empty())
      {
        absolutePath += relativePath.GetFileName();
        return absolutePath;
      }
    }
    else if (_tcsncmp(refPath.c_str(), _T("${mcpath}\\"), 10) == 0)
    {
      // Create absolute from relative path
      PathString path(szAppPath, _T(""));
      PathString relativePath(refPath.c_str() + 10);
      absolutePath = path.GetAbsolutePath(relativePath.GetPath());
      if (!absolutePath.empty())
      {
        absolutePath += relativePath.GetFileName();
        return absolutePath;
      }
    }

    return absolutePath;
  }
  static STLString RemoveIllegalPathCharacters(STLString& str)
  {
    str.erase(std::remove(str.begin(), str.end(), ':'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '/'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '*'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '<'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '>'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '|'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '?'), str.end());
    return str;
  }

  static STLString ReplaceIllegalPathCharacters(STLString& str, const char ch)
  {
    std::replace(str.begin(), str.end(), ':', ch);
    std::replace(str.begin(), str.end(), '/', ch);
    std::replace(str.begin(), str.end(), '\\', ch);
    std::replace(str.begin(), str.end(), '*', ch);
    std::replace(str.begin(), str.end(), '<', ch);
    std::replace(str.begin(), str.end(), '>', ch);
    std::replace(str.begin(), str.end(), '|', ch);
    std::replace(str.begin(), str.end(), '\"', ch);
    std::replace(str.begin(), str.end(), '?', ch);
    return str;
  }

protected:
  static STLString AddTrailingSlash(const TCHAR* sz)
  {
//    ASSERT(sz);
    STLString str;
    str = sz;
    if( str.length() > 0 && str[ str.length() -1 ] != '\\' )
      str.append( _T("\\") );

    return str;
  }

  static STLString& AddTrailingSlash(STLString& str)
  {
    if( str.length() > 0 && str[ str.length() -1 ] != '\\' )
      str.append( _T("\\") );

    return str;
  }
  
  static STLString xPathAppend(STLString& path, const TCHAR* szNewPart)
  {
    AddTrailingSlash(path).append(szNewPart);
    return path;
  }

	void SplitPath(const TCHAR* s)
	{
		if(!s)
			return;

		TCHAR tmp[1024] = {0};
		const TCHAR* pos = _tcsrchr(s,'\\');
		if(pos)
			pos++; // We want the \ char in the path string
		else
			pos = s;

		_tcsncpy(tmp,s,min(pos-s,1024));
		m_path = tmp;
		m_filename = pos;
	}

  // Issue   C:\MyPath\..MYSubPathWithDotDot\ <<-- This path is translated.. NOT good.
 // WinAPI version of PathCanonicalize  do not support paths longer then MAX_PATH 
  BOOL WINAPI _PathCanonicalize(LPWSTR lpszBuf, LPCWSTR lpszPath)
  {
    LPWSTR lpszDst = lpszBuf;
    LPCWSTR lpszSrc = lpszPath;

    if (lpszBuf)
      *lpszDst = '\0';

    if (!lpszBuf || !lpszPath)
    {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    if (!*lpszPath)
    {
      *lpszBuf++ = '\\';
      *lpszBuf = '\0';
      return TRUE;
    }

    /* Copy path root */
    if (*lpszSrc == '\\')
    {
      *lpszDst++ = *lpszSrc++;
    }
    else if (*lpszSrc && lpszSrc[1] == ':')
    {
      /* X:\ */
      *lpszDst++ = *lpszSrc++;
      *lpszDst++ = *lpszSrc++;
      if (*lpszSrc == '\\')
        *lpszDst++ = *lpszSrc++;
    }

    /* Canonicalize the rest of the path */
    while (*lpszSrc)
    {
      if (*lpszSrc == '.')
      {
        if (lpszSrc[1] == '\\' && (lpszSrc == lpszPath || lpszSrc[-1] == '\\' || lpszSrc[-1] == ':'))
        {
          lpszSrc += 2; /* Skip .\ */
        }
        else if (lpszSrc[1] == '.' && (lpszSrc[2] != '\0' && lpszSrc[2] != '\\')) // include path that begins with "..mypath", do not translate that
        {
          // Copy path until we reach end or find next part
          while (*lpszSrc != '\0' && *lpszSrc != '\\')
          {
            *lpszDst++ = *lpszSrc++;
          }
        }
        else if (lpszSrc[1] == '.' && (lpszDst == lpszBuf || lpszDst[-1] == '\\'))
        {
          /* \.. backs up a directory, over the root if it has no \ following X:.
          * .. is ignored if it would remove a UNC server name or initial \\
          */
          if (lpszDst != lpszBuf)
          {
            *lpszDst = '\0'; /* Allow PathIsUNCServerShareA test on lpszBuf */
            if (lpszDst > lpszBuf + 1 && lpszDst[-1] == '\\' &&
              (lpszDst[-2] != '\\' || lpszDst > lpszBuf + 2))
            {
              if (lpszDst[-2] == ':' && (lpszDst > lpszBuf + 3 || lpszDst[-3] == ':'))
              {
                lpszDst -= 2;
                while (lpszDst > lpszBuf && *lpszDst != '\\')
                  lpszDst--;
                if (*lpszDst == '\\')
                  lpszDst++; /* Reset to last '\' */
                else
                  lpszDst = lpszBuf; /* Start path again from new root */
              }
              else if (lpszDst[-2] != ':' && !PathIsUNCServerShareW(lpszBuf))
                lpszDst -= 2;
            }
            while (lpszDst > lpszBuf && *lpszDst != '\\')
              lpszDst--;
            if (lpszDst == lpszBuf)
            {
              *lpszDst++ = '\\';
              lpszSrc++;
            }
          }
          lpszSrc += 2; /* Skip .. in src path */
        }
        else
          *lpszDst++ = *lpszSrc++;
      }
      else
        *lpszDst++ = *lpszSrc++;
    }
    /* Append \ to naked drive specs */
    if (lpszDst - lpszBuf == 2 && lpszDst[-1] == ':')
      *lpszDst++ = '\\';
    *lpszDst++ = '\0';
    return TRUE;
  }

	STLString m_path;
	STLString m_filename;
};