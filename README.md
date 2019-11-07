# MZMisc
Collection of misc C++ header only helper classes

## PathString.h 
Class for handling file paths

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
                static CreateUNCPath(..)
                static PathIsDirectoryEmptEx2(..)
                static PathIsQuouted(...)
                static StripQuotes(...)	
                static RelativeToAbsolutePath
                static RemoveIllegalPathCharacters
                static ReplaceIllegalPathCharacters

# AutoHandle.h 
RAII Class for Windows HANDLE

# BoostPrio.h 
Class for tempoary boost thread priority, and when it destructs it will return thread prio to previous state


