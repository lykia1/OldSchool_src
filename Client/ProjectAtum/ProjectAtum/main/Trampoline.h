#pragma once
#include <Windows.h>
#include <cstring>
#include <functional>
#include <memory>	  

template <typename FuncType>
class TrampolineHook
{
public:
	TrampolineHook();
	TrampolineHook(byte* funcAddr, byte* destination, unsigned int trampLength);
	~TrampolineHook();	

	bool Init();

	FuncType GetOriginal();
	bool Hook();
	void Unhook();

private:
	byte* CreateTrampoline();

private:
	BYTE* m_source;
	BYTE* m_destination;
	unsigned int m_length;
	BYTE* m_gateway;
};

template <typename FuncType>
inline TrampolineHook<FuncType>::TrampolineHook()
{
	m_gateway = 0;
	m_destination = 0;
	m_source = 0;
}

template<typename FuncType>
TrampolineHook<FuncType>::TrampolineHook(byte* funcAddr, byte* destination, unsigned int trampLength)
{
	m_destination = destination;
	m_source = funcAddr;
	m_length = trampLength;
	m_gateway = 0;
	Init();
}

template<typename FuncType>
TrampolineHook<FuncType>::~TrampolineHook()
{
	Unhook();
	// free trampoline memory
	VirtualFree(m_gateway, 0, MEM_RELEASE);
}
	  
template<typename FuncType>
inline bool TrampolineHook<FuncType>::Init()
{
	m_gateway = (BYTE*)CreateTrampoline();
	return (m_gateway != nullptr);
}

template<typename FuncType>
inline FuncType TrampolineHook<FuncType>::GetOriginal()
{
	return reinterpret_cast<FuncType>(m_gateway);
}

template<typename FuncType>
bool TrampolineHook<FuncType>::Hook()
{
	if (!m_gateway || !m_source || m_length < 5)
		return false;

	DWORD oProc;
	if (VirtualProtect(m_source, m_length, PAGE_EXECUTE_READWRITE, &oProc))
	{
		memset(m_source, 0x90, m_length);
		uintptr_t relAddy = (uintptr_t)(m_destination - m_source - 5);
		*m_source = (char)0xE9;
		*(uintptr_t*)(m_source + 1) = (uintptr_t)relAddy;
		if (VirtualProtect(m_source, m_length, oProc, &oProc))
		{
			return true;
		}
	}
	return false;
}

template<typename FuncType>
void TrampolineHook<FuncType>::Unhook()
{
	if (!m_gateway || !m_source || m_length < 5)
		return;

	DWORD oProc;
	if (VirtualProtect(m_source, m_length, PAGE_EXECUTE_READWRITE, &oProc))
	{
		std::memcpy(m_source, m_gateway, m_length);
		VirtualProtect(m_source, m_length, oProc, &oProc);
	}
}

template<typename FuncType>
byte* TrampolineHook<FuncType>::CreateTrampoline()
{
	if (m_gateway != nullptr) return 0;
	if (m_length < 5) return 0;

	byte* gateway = (BYTE*)VirtualAlloc(0, m_length + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (gateway)
	{
		std::memcpy(gateway, m_source, m_length);
		uintptr_t jumpAddy = (uintptr_t)(m_source - gateway - 5);
		*(gateway + m_length) = (char)0xE9;
		*(uintptr_t*)(gateway + m_length + 1) = jumpAddy;
		return gateway;
	}
	return 0;
}