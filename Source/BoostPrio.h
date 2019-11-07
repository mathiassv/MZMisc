/*
  Boosts the thread prio until this class destructs. Then reset prio to previous state.

*/

#pragma once

class BoostThreadPrio
{
public:
    BoostThreadPrio(int nPriority = THREAD_PRIORITY_HIGHEST)
    {
      m_hThread = GetCurrentThread();
      if(m_hThread)
      {
        m_nOldThreadPrio = GetThreadPriority(m_hThread);
        SetThreadPriority(m_hThread, nPriority);
      }
      else
        m_nOldThreadPrio = THREAD_PRIORITY_NORMAL;
    }

    ~BoostThreadPrio()
    {
      if(m_hThread)
      {
        SetThreadPriority(m_hThread, m_nOldThreadPrio);
      }
    }

protected:
  HANDLE m_hThread;
  int m_nOldThreadPrio;
};

class BoostClassPrio
{
public:
  BoostClassPrio(int nPriority = HIGH_PRIORITY_CLASS)
  {
    m_hProcess = GetCurrentProcess();
    if(m_hProcess)
    {
      m_nOldPriClass = GetPriorityClass(m_hProcess);
      SetPriorityClass(m_hProcess, nPriority);
    }
    else
      m_nOldPriClass = NORMAL_PRIORITY_CLASS;
  }

  ~BoostClassPrio()
  {
    if(m_hProcess)
    {
      SetPriorityClass(m_hProcess, m_nOldPriClass);
    }
  }

protected:
  HANDLE m_hProcess;
  DWORD m_nOldPriClass;
};

