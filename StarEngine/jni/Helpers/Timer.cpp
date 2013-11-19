#include "Timer.h"
#include "..\Context.h"

namespace star
{
	Timer::Timer() //Default Constructor
		:m_TargetTime(1.0f)
		,m_CurrentTime(0)
		,m_CountingDown(false)
		,m_Looping(false)
		,m_Paused(false)
		,m_Infinite(false)
		,m_ExcecutingFunction(nullptr)
	{
	}

	Timer::Timer(	double targetTime, bool countingDown,
					bool loop, std::function<void ()> func,
					bool paused) 
		:m_TargetTime(targetTime)
		,m_CurrentTime(countingDown?targetTime:0)
		,m_CountingDown(countingDown)
		,m_Looping(loop)
		,m_Paused(paused)
		,m_Infinite(targetTime == 0 ? true : false)
		,m_ExcecutingFunction(func)
	{
	}

	Timer::~Timer() //Default Destructor
	{
	}

	Timer::Timer(const Timer& yRef)
		:m_TargetTime(yRef.m_TargetTime)
		,m_CurrentTime(yRef.m_CurrentTime)
		,m_CountingDown(yRef.m_CountingDown)
		,m_Looping(yRef.m_Looping)
		,m_Paused(yRef.m_Paused)
		,m_Infinite(yRef.m_Infinite)
		,m_ExcecutingFunction(yRef.m_ExcecutingFunction)
	{
	}

	Timer::Timer(Timer&& yRef)
		:m_TargetTime(yRef.m_TargetTime)
		,m_CurrentTime(yRef.m_CurrentTime)
		,m_CountingDown(yRef.m_CountingDown)
		,m_Looping(yRef.m_Looping)
		,m_Paused(yRef.m_Paused)
		,m_Infinite(yRef.m_Infinite)
		,m_ExcecutingFunction(yRef.m_ExcecutingFunction)
	{
	}

	Timer& Timer::operator=(const Timer& yRef)
	{
		m_TargetTime = yRef.m_TargetTime;
		m_CurrentTime = yRef.m_CurrentTime;
		m_CountingDown = yRef.m_CountingDown;
		m_Looping = yRef.m_Looping;
		m_Paused = yRef.m_Paused;
		m_Infinite = yRef.m_Infinite;
		m_ExcecutingFunction = yRef.m_ExcecutingFunction;
		return *this;
	}

	Timer& Timer::operator=(Timer&& yRef)
	{
		m_TargetTime = yRef.m_TargetTime;
		m_CurrentTime = yRef.m_CurrentTime;
		m_CountingDown = yRef.m_CountingDown;
		m_Looping = yRef.m_Looping;
		m_Paused = yRef.m_Paused;
		m_Infinite = yRef.m_Infinite;
		m_ExcecutingFunction = yRef.m_ExcecutingFunction;
		return *this;
	}

	bool Timer::Update(const Context& context)
	{
		if(m_Paused)
		{
			return false;
		}
		if(m_CountingDown && m_CurrentTime >= 0)
		{
			m_CurrentTime -= context.mTimeManager->GetSeconds();
			if(m_CurrentTime < 0)
			{
				m_ExcecutingFunction();
				if(m_Looping)
				{
					Reset(false);
					return false;
				}
				return true;
			}
			return false;
		}
		else if(!m_CountingDown && m_CurrentTime <= m_TargetTime)
		{
			m_CurrentTime += context.mTimeManager->GetSeconds();
			if(m_CurrentTime > m_TargetTime)
			{
				m_ExcecutingFunction();
				if(m_Looping)
				{
					Reset(false);
					return false;
				}
				return true;
			}
			return false;
		}
		return true;
	}

	void Timer::SetPaused(bool paused)
	{
		m_Paused = paused;
	}

	void Timer::SetCountingDown(bool countingDown)
	{
		m_CountingDown = countingDown;
	}

	void Timer::SetLoop(bool looping)
	{
		m_Looping = looping;
	}

	void Timer::Reset(bool paused)
	{
		m_CurrentTime = m_CountingDown ? m_TargetTime : 0;
		SetPaused(paused);
	}

	void Timer::SetTargetTime(double targetTime, const bool reset, const bool paused)
	{
		m_TargetTime = targetTime;
		if(reset)
		{
			Reset(paused);
		}
	}

	void Timer::SetFunction(std::function<void ()> func)
	{
		m_ExcecutingFunction = func;
	}

	int32 Timer::GetCurrentMinutes() const
	{
		int32 currentTime(0);
		currentTime = (int32)m_CurrentTime;
		return currentTime / 60;
	}

	int32 Timer::GetCurrentSeconds() const
	{
		int32 currentTime(0);
		currentTime = (int32)m_CurrentTime;
		return currentTime % 60;
	}

	int32 Timer::GetCurrentTotalSeconds() const
	{
		int32 currentTime(0);
		currentTime = (int32)m_CurrentTime;
		return currentTime;
	}
	
	double Timer::GetTargetTime() const
	{
		return m_TargetTime;
	}

	double Timer::GetCurrentAccurateTime() const
	{
		return m_CurrentTime;
	}
}
