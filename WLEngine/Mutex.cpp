#include "Mutex.h"
namespace WL
{
	//////////////////////////////////////////////////////////////////////////
	CMutex::CAutoLock::CAutoLock(CMutex& mutex)
	: m_Mutex(mutex)
	{
		m_Mutex.lock();
	}

	CMutex::CAutoLock::~CAutoLock()
	{
		m_Mutex.unlock();
	}

	//////////////////////////////////////////////////////////////////////////
	CMutex::CAutoAllowWorkerThreadLock::CAutoAllowWorkerThreadLock(bool allow /*= true*/)
	{

	}


	CMutex::CAutoAllowWorkerThreadLock::~CAutoAllowWorkerThreadLock()
	{

	}

	CMutex::CMutex()
	{

	}

	CMutex::~CMutex()
	{

	}

	void CMutex::lock()
	{
		m_Mutex.lock();
	}

	void CMutex::unlock()
	{
		m_Mutex.unlock();

	}

	bool CMutex::tryLock()
	{
		if (!m_Mutex.tryLock())
		{
			return true;
		}
		m_Mutex.unlock();

		return false;
	}

	bool CMutex::isLocked()
	{
		if (!m_Mutex.tryLock())
		{
			return true;
		}
		m_Mutex.unlock();
		return false;
	}

	void CMutex::blockUntilUnlocked()
	{
		lock();
		unlock();
	}




}