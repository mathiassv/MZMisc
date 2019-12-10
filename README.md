# MZMisc
Collection of misc C++ header only helper classes

## PathString.h 
Class for handling file paths

```cpp
PathString( <file path> )
SamePath(const PathString& otherpath)
RemoveTrailingSlashFromPath()
GetPathLength()
GetPath()
GetFileName()
GetFileExtension()
GetFileNamePart()
GetFullFileName()
GetDevice(..)
GetAbsolutePath(...)
static bool CreateUNCPath(..)
static bool PathIsDirectoryEmptEx2(..)
static bool PathIsQuouted(...)
static STLStirng StripQuotes(...)
static STLStirng RelativeToAbsolutePath(...) 
static STLStirng RemoveIllegalPathCharacters(...)
static STLStirng ReplaceIllegalPathCharacters(...)

// Example
auto ps = PathString(szFilePath);
auto filename = ps.GetFileNamePart();
auto absPath = ps.GetAbsolutePath(szRelativePath);

bool success = PathString::CreateUNCPath(out uncPath, true true);
bool isEmpty = PathString::PathIsDirectoryEmptyEx2(myPath);
STLString myPath = PathString::RemoveIllegalPathCharacters(MyOtherPath);
```

## AutoHandle.h 
RAII Class for Windows HANDLE

```cpp
 {
  auto file = AutoHandle(::CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0));
  if (file.isValid())
  {
     ::WriteFile(file, byData, len, &bytesWritten, 0);
  }
}
 // file hanlder is auto closed when file goes out of scope
 
```
## BoostPrio.h 
Class for tempoary boost thread priority, and when it destructs it will return thread prio to previous state

```cpp
 {
  BoostThreadPrio boost;
    ... to work
 }
 // Prio is restored to previous state
 
 
```
