#pragma once

#include <wtypes.h>
#include <winbase.h>
#include <memory>

class CResGuard {
public:
    CResGuard()  { InitializeCriticalSection(&m_cs); }
    ~CResGuard() { DeleteCriticalSection(&m_cs); }
	BOOL IsLocked()
	{
		if( m_cs.OwningThread )
			return TRUE;
		else
			return FALSE;
	};
public:
    class CGuard {
    public:
        CGuard(CResGuard& rs) : m_rs(rs) { m_rs.Guard(); };
        ~CGuard() { m_rs.Unguard(); }
    private:
        CResGuard& m_rs;
    };
    friend class CResGuard::CGuard;
private:
   void Guard()   { EnterCriticalSection(&m_cs); }
   void Unguard() { LeaveCriticalSection(&m_cs); }
private:
   CRITICAL_SECTION m_cs;
};

template <class T>
class Singleton
{
public:
       static T& Instance()
	   {
		   if(NULL==s_instance.get())
		   {
			   CResGuard::CGuard gd(s_rs);
			   if(NULL==s_instance.get())
			   {
				   s_instance.reset(new T);
			   }
		   }
		   return *s_instance.get();
	   }
protected:
       Singleton(void){}
       ~Singleton(void){}
       Singleton(const Singleton&){}
       Singleton & operator= (const Singleton &){return *this;}

       static std::auto_ptr<T> s_instance;
       static CResGuard s_rs;
};

template <class T> std::auto_ptr<T> Singleton<T>::s_instance;
template <class T> CResGuard Singleton<T>::s_rs;

#define DECLARE_SINGLETON_CLASS( type ) \
       friend class std::auto_ptr< type >;\
       friend class Singleton< type >;
