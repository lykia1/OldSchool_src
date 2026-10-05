#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <chrono>	

class IEvoImGuiCallback;

struct EvoImGuiCallbackEntry
{
	std::string name;
	IEvoImGuiCallback* cb;
};
struct PROFILER_ITEM
{
	DWORD dwTotalUsedTime;
	int nCnt;
};
class EvoAdminPanel
{
public:
	EvoAdminPanel();
	virtual ~EvoAdminPanel();

	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Render();

	void Toggle();
	void ShowMenu(bool open);
	bool IsOpen();
	void SetBlockInput(bool block);
	
	void RegisterCallback(std::string name, IEvoImGuiCallback* callback);

	static DWORD GetInfluenceColor(BYTE influenceType);
	static DWORD GetChatColor(char szColor);
	bool GetRenderObjID() { return bRenderObjectNumbers; }
	bool GetRenderSpriteID() { return bRenderSpriteNumbers; }
	bool GetRenderParticleID() { return bRenderParticleID; }
	bool GetRenderObjAnimID() { return bRenderObjAnimID; }
	bool GetRenderObjBodyCond() { return bRenderObjBodyCond; }
	bool GetRenderMonsterBodyCond() { return bRenderMonsterBodyCond; }
	bool GetDrawEvents() { return bDrawEvents; }
	bool GetRenderTraceID() { return bRenderTrace; }
	bool SetRenderTraceID(bool bVal) { bRenderTrace = bVal; }
	void SetRenderObjID(bool bVal) { bRenderObjectNumbers  = bVal; }
	void SetRenderSpriteID(bool bVal) { bRenderSpriteNumbers = bVal; }
	void SetRenderParticleID(bool bVal) { bRenderParticleID = bVal; }
	void SetRenderObjAnimID(bool bVal) { bRenderObjAnimID = bVal; }
	void SetRenderObjBodyCond(bool bVal) { bRenderObjBodyCond = bVal; }
	void SetRenderMonsterBodyCond(bool bVal) { bRenderMonsterBodyCond = bVal; }
	void SetDrawEvents(bool bVal) { bDrawEvents = bVal; }

	float timer1;
	float timer2;
	float timer3;
	float timer4;
	float timer5;
	float timer6;
	float timer7;

	int nSizeV1;
	int nSizeV2;
	int nSizeV3;

protected:
	virtual void RenderImGui();
	void ImGuiBeginFrame();
	void ImGuiEndFrame();
	bool TrySendCommand(std::string commandString);
private:
	bool m_isOpen;
	bool m_blockInput;
	std::chrono::time_point<std::chrono::system_clock> m_lastCommand;
	std::vector<EvoImGuiCallbackEntry> m_imgui_callbacks;
	MEMORYSTATUS* mem;
	DWORD						m_dwLastTime;
	PROFILER_ITEM				m_stTotalTime;
	DWORD						m_dwStartTime;
	string						m_pItemName;
	map<string, DWORD>			m_mapUsedTime;
	map<string, PROFILER_ITEM>	m_mapTotalUsedTime;
	int nValue1;
	int nValue2;
	int nMulti;
	int nMulti2;

	bool bRenderObjectNumbers;
	bool bRenderSpriteNumbers;
	bool bDrawEvents;
	bool bRenderParticleID;
	bool bRenderObjAnimID;
	bool bRenderObjBodyCond;
	bool bRenderMonsterBodyCond;
	bool bRenderTrace;
};