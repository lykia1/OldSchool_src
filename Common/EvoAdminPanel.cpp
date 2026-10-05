#include "stdafx.h"
#include "EvoAdminPanel.h"

#include "AtumApplication.h"
#include "IEvoImGuiCallback.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include <d3d9.h>
#include <INFGameMain.h>
#include <INFGameMainChat.h>
#include "SceneData.h"
#include "Camera.h"
#include "EffectRender.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EvoAdminPanel::EvoAdminPanel()
{
	m_isOpen = false;
    m_blockInput = false;
    nValue1 = 0;
    nValue2 = 0;
    m_dwLastTime = 0;
    mem = new MEMORYSTATUS;
    nMulti = 1;
    nMulti2 = 1;

    bRenderObjectNumbers = false;
    bRenderSpriteNumbers = false;
    bDrawEvents = false;
    bRenderParticleID = false;
    bRenderObjAnimID = false;
    bRenderObjBodyCond = false;
    bRenderMonsterBodyCond = false;
    bRenderTrace = false;

    timer1 = 1.0f;
    timer2 = 1.0f;
    timer3 = -1.0;
    timer4 = 0.97;
    timer5 = 0.2;
    timer6 = 0.2;
    timer7 = 0;
}

EvoAdminPanel::~EvoAdminPanel()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
    SAFE_DELETE(mem);
}

void EvoAdminPanel::Toggle()
{
    m_isOpen = !m_isOpen;
}

void EvoAdminPanel::ShowMenu(bool open)
{
    m_isOpen = open;
}

bool EvoAdminPanel::IsOpen()
{
    return m_isOpen;
}

void EvoAdminPanel::SetBlockInput(bool block)
{
    m_blockInput = block;
}

void EvoAdminPanel::RegisterCallback(std::string name, IEvoImGuiCallback* callback)
{
    // only add once
    for (auto& cb : m_imgui_callbacks)
    {
        if (cb.name == name)
            return;
    }
    m_imgui_callbacks.push_back(EvoImGuiCallbackEntry{ name, callback });
}

DWORD EvoAdminPanel::GetInfluenceColor(BYTE influenceType)
{
    switch (influenceType)
    {
        case INFLUENCE_TYPE_ANI:
            return D3DCOLOR_RGBA(255, 169, 41, 255);
        case INFLUENCE_TYPE_VCN:
            return D3DCOLOR_RGBA(41, 180, 255, 255);
        case INFLUENCE_TYPE_NORMAL:
            return D3DCOLOR_RGBA(255, 255, 255, 255);
        default:
            return D3DCOLOR_RGBA(159, 0, 255, 255);
    }
    //if (IS_VCN_INFLUENCE_TYPE(influenceType) && IS_ANI_INFLUENCE_TYPE(influenceType))   // Staff
    //{
    //   return D3DCOLOR_RGBA(159, 0, 255, 255);
    //}    
    //if (IS_VCN_INFLUENCE_TYPE(influenceType))
    //{
    //    return D3DCOLOR_RGBA(255, 169, 41,255);
    //}
    //if (IS_ANI_INFLUENCE_TYPE(influenceType))
    //{
    //    return D3DCOLOR_RGBA(41, 180, 255, 255);
    //}
    //if(IS_NORMAL_INFLUENCE_TYPE(influenceType))
    //{
    //    return D3DCOLOR_RGBA(255, 255, 255, 255);
    //}    
}

DWORD EvoAdminPanel::GetChatColor(char szColor)
{ 
     return D3DCOLOR_RGBA(159, 0, 255, 255);
}

void EvoAdminPanel::ImGuiBeginFrame()
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void EvoAdminPanel::ImGuiEndFrame()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

HRESULT EvoAdminPanel::InitDeviceObjects()
{
    D3DDEVICE_CREATION_PARAMETERS device_params;
    auto device = g_pApp->GetDirect3DDevice();
    device->GetCreationParameters(&device_params);

    ImGui::CreateContext();
    if (!ImGui_ImplWin32_Init(device_params.hFocusWindow)) {
        return E_FAIL;
    }

    if (!ImGui_ImplDX9_Init(device)) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT EvoAdminPanel::RestoreDeviceObjects()
{
    ImGui_ImplDX9_CreateDeviceObjects();
    return S_OK;
}

HRESULT EvoAdminPanel::InvalidateDeviceObjects()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    return S_OK;
}

HRESULT EvoAdminPanel::DeleteDeviceObjects()
{
    return S_OK;
}

void EvoAdminPanel::Render()
{
    if (IsOpen())
    {
        ImGuiBeginFrame();
        RenderImGui();
        ImGuiEndFrame();
    }
}

LRESULT EvoAdminPanel::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYUP:
        switch (wParam)
        {
        case VK_INSERT:
            Toggle();
            return 1;
        }

    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_CHAR:
    case WM_SETCURSOR:
    case WM_DEVICECHANGE:
        if (IsOpen())
        {
            LRESULT res = ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
            if (m_blockInput || ImGui::GetIO().WantCaptureMouse)
            {
                return 1;
            }
            else
            {
                return res;
            }

        }
    default:
        return 0;
    }

}

void EvoAdminPanel::RenderImGui()
{
    ImGui::SetNextWindowSize(ImVec2(640, 480));

    ImGui::Begin("Evo Admin Panel", &m_isOpen, ImGuiWindowFlags_NoResize);
    {
        ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_::ImGuiTabBarFlags_None);
        {
           
            if (ImGui::BeginTabItem("Info"))
            {
               /* if (ImGui::Button("Add 10 px"))
                {
                    g_pD3dApp->nObjectRenderTest[0] += 10;
                }
                ImGui::NewLine();
                char szTemp[1024]; *szTemp = '\0';

                ImGui::NewLine();
                if (ImGui::Button("Remove 10 px"))
                {
                    g_pD3dApp->nObjectRenderTest[0] -= 10;
                }
                ImGui::NewLine();
                sprintf(szTemp, "Current setting for 0: %d", g_pD3dApp->nObjectRenderTest[0]);
                ImGui::Text(szTemp);*/

                ImGui::NewLine();
                ImGui::Separator();
                ImGui::Text("Menu Settings");
                ImGui::Separator();

                ImGui::Checkbox("Block input when menu is open", &m_blockInput);
              
                ImGui::NewLine();
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::Text("STAFF Informations");
                ImGui::Separator();
                if(g_pD3dApp)
                {
                    char szTemp[1024]; *szTemp = '\0';
                    //FPS
                    sprintf(szTemp, "Current FPS: %.2f", g_pD3dApp->m_fFPS);
                    ImGui::Text(szTemp);
                   // ImGui::Text("Elapsed Time: %.9f",timer1);
                    //PLAYERS
                    if (strlen(g_pD3dApp->m_strSeverUserNum)) {
                        sprintf(szTemp, "Total player count: %s", g_pD3dApp->m_strSeverUserNum);
                        ImGui::Text(szTemp);
                    }
                    //Current Map players
                    if (strlen(g_pD3dApp->m_strMapUserNum)) {
                        sprintf(szTemp, "Map player count: %s", g_pD3dApp->m_strMapUserNum);
                        ImGui::Text(szTemp);
                    }
                   memset(szTemp,0x00,sizeof(szTemp));
                }

               /* ImGui::NewLine();
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::Text("DEBUG info - CHAT linking");
                ImGui::Separator();
                if (g_pGameMain->m_pChat)
                {
                    auto* pCurChatTab = g_pGameMain->m_pChat->GetChatTabMode();
                    int nRenderIndex = pCurChatTab->m_nRenderStartIndex;
                    int nMaxRenderLineCounts = (g_pGameMain->m_pChat->m_nChatBoxHeight - CHATBOX_IMAGE_GAB_HEITHT_TOP)
                        / CHAT_FONT_LINE_HEIGHT; 
                    int nMaxRenderSquareSize = (g_pGameMain->m_pChat->m_nChatBoxHeight - CHATBOX_IMAGE_GAB_HEITHT_TOP);

                    char szTemp[1024]; *szTemp = '\0';
                    sprintf(szTemp, "RenderStartIdx: %d", nRenderIndex);
                    ImGui::Text(szTemp);
                    sprintf(szTemp, "MaxRenderLines: %d", nMaxRenderLineCounts);
                    ImGui::Text(szTemp);
                    sprintf(szTemp, "MaxRenderY size: %d", nMaxRenderSquareSize);
                    ImGui::Text(szTemp);
                    for (auto& it : g_pGameMain->m_pChat->vectStoredUID)
                    {
                        sprintf(szTemp, "LinkedItem: %d", it.first);
                        ImGui::Text(szTemp);
                        sprintf(szTemp, "LinkedItem pos: X:%d Y:%d", g_pGameMain->m_pChat->ptPosition[it.first].x,g_pGameMain->m_pChat->ptPosition[it.first].y);
                        ImGui::Text(szTemp);
                    }
                }*/


  
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Tools"))
            {
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::Text("Tools");
                ImGui::Separator();
              /*  ImGui::SliderFloat("clamp y min",&timer1,-1.0f,1.0f);
                ImGui::SliderFloat("clamp y max",&timer2,-1.0f,1.0f);
                ImGui::SliderFloat("cam clamp y max",&timer3,-1.0f,1.0f);
               /* ImGui::SliderFloat("viewWaterDepth",&timer4,-10.0f,50.0f);
                ImGui::SliderFloat("shoreRange",&timer5,-10.0f,10.0f);
                ImGui::SliderFloat("main_opacity",&timer6,-10.0f,10.0f);*/
                ImGui::SliderFloat("Adjust day/night time",&timer7,-24.0f,24.0f);
                if (g_pD3dApp) {
                    ImGui::Checkbox("Toggle wireframe mode", &g_pD3dApp->m_bWireframe);
                    ImGui::Checkbox("Toggle wallhack (no collision with objects and map) mode", &g_pD3dApp->m_bNoClipping);
                }
    //alts and dga caps
                if (ImGui::Button("Kick all alts"))
                {
                    TrySendCommand("/kickallalts");
                }
                ImGui::SameLine();
                if (ImGui::Button("Give 1x DGA Capsule"))
                {
                    TrySendCommand("/itemalluser 255 7025752 1");
                }
                ImGui::NewLine();
                ImGui::Separator();
    //bananas
                if (ImGui::Button("Give 1x Banana"))
                {
                    TrySendCommand("/itemalluser 255 7022580 1");
                }
                ImGui::SameLine();
                if (ImGui::Button("Give 10x Banana"))
                {
                    TrySendCommand("/itemalluser 255 7022580 10");
                }
                ImGui::SameLine();
                if (ImGui::Button("Give 100x Banana"))
                {
                    TrySendCommand("/itemalluser 255 7022580 100");
                }
                ImGui::SameLine();
                if (ImGui::Button("Give 666x Banana"))
                {
                    TrySendCommand("/itemalluser 255 7022580 666");
                }
                ImGui::NewLine();
                ImGui::Separator();
//sp spawns
                if (ImGui::Button("Block SP spawns"))
                {
                    TrySendCommand("/BSP 1");
                }
                ImGui::SameLine();
                if (ImGui::Button("Enable SP spawns"))
                {
                    TrySendCommand("/BSP 2");
                }
                ImGui::SameLine();
                if (ImGui::Button("Check if SP spawns are enabled"))
                {
                    TrySendCommand("/BSP");
                }
                ImGui::NewLine();
                ImGui::Separator();
//reload features file
                if (ImGui::Button("Reload features.cfg file (server side)"))
                {
                    TrySendCommand("/RF");
                }

                ImGui::EndTabItem();
            }
            
            for (auto& tab : m_imgui_callbacks)
            {
                if (ImGui::BeginTabItem(tab.name.c_str()))
                {
                    tab.cb->RenderImGui();
                    ImGui::EndTabItem();
                }
            }
         /*   if (ImGui::BeginTabItem("Dev Tools"))
            {
                ImGui::Separator();
                ImGui::Text("Dbg Strings");
                ImGui::Separator();
                ImGui::Text("float1 : %.3f",timer1);
                ImGui::Text("float2 : %.3f",timer2);
                ImGui::Text("float3 : %.3f",timer3);
                ImGui::NewLine();
                ImGui::Text("Size1 : %d", nSizeV1);
                ImGui::Text("Size2 : %d", nSizeV2);
                ImGui::Text("Size3 : %d", nSizeV3);

                ImGui::Separator();
                ImGui::Text("Configs");
                ImGui::Separator();
                if (g_pD3dApp) {
                    ImGui::Checkbox("Toggle wireframe mode", &g_pD3dApp->m_bWireframe);
                    ImGui::Checkbox("Toggle wallhack (no collision with objects and map) mode", &g_pD3dApp->m_bNoClipping);

                    ImGui::Checkbox("Draw Events", &bDrawEvents);
                    ImGui::Checkbox("Draw Object ID", &bRenderObjectNumbers);
                    ImGui::Checkbox("Draw Object Animation ID", &bRenderObjAnimID);
                    ImGui::Checkbox("Draw Sprite Animation ID", &bRenderSpriteNumbers);
                    ImGui::Checkbox("Draw Particle Animation ID", &bRenderParticleID);
                    ImGui::Checkbox("Draw Object Body Conditions", &bRenderObjBodyCond);
                    ImGui::Checkbox("Draw Monsters Body Conditions", &bRenderMonsterBodyCond);
                    ImGui::Checkbox("Draw Trace ID", &bRenderTrace);
                }
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::InputInt("Multipler", &nMulti);
                ImGui::NewLine();
                if (ImGui::Button("Val1 +"))
                {
                    nValue1 += 1* nMulti;
                }
                ImGui::SameLine();
                if (ImGui::Button("Val1 -"))
                {
                    nValue1 -= 1 * nMulti;
                }
                ImGui::SameLine();
                ImGui::Text("Value: %d", nValue1);
                ImGui::Separator();

                ImGui::InputInt("Multipler2", &nMulti2);
                ImGui::NewLine();
                if (ImGui::Button("Val2 +"))
                {
                    nValue2 += 1 * nMulti2;
                }
                ImGui::SameLine();
                if (ImGui::Button("Val2 -"))
                {
                    nValue2 -= 1 * nMulti2;
                }
                ImGui::SameLine();
                ImGui::Text("Value2: %d", nValue2);
                ImGui::Separator();
                ImGui::Separator();
                ImGui::NewLine();
            
                    ZeroMemory(mem, sizeof(MEMORYSTATUS));
                    mem->dwLength = sizeof(MEMORYSTATUS);
                    GlobalMemoryStatus(mem);
                
                DWORD dwNowTime = timeGetTime();
                DWORD dwNowUsedTime = dwNowTime - m_dwLastTime;
                if (0 != m_dwLastTime)
                {
                    m_stTotalTime.dwTotalUsedTime += dwNowUsedTime;
                    m_stTotalTime.nCnt += 1;
                }
                m_dwLastTime = dwNowTime;


                char buf[128] = { 0, };


                ImGui::Text("DeviceStat: %s", g_pD3dApp->GetDeviceStats());
                ImGui::Text("FrameStat: %s", g_pD3dApp->GetFrameStats());
                ImGui::Text("ElapsedTime: %.2f", g_pD3dApp->GetElapsedTime());
                

                ImGui::Text("Current FPS: %.2f", g_pD3dApp->m_fFPS);
                ImGui::Text("Current Ping: %.2f", g_pD3dApp->m_fPingLat/2.0f);
                ImGui::Separator();
                float fCounter = ((50 - min(g_pD3dApp->m_fFPS, 50)) / 100.0f) * 3;           
#ifdef _M_X64 //x64 size 
                ImGui::Text("Objects : [ %I64d ]", g_pScene->m_vectorCulledObjectPtrList.size());
                ImGui::Text("Monster Render Count : %I64d", g_pScene->m_vecMonsterRenderList.size());
                ImGui::Text("Monster Data Count : %I64d", g_pScene->m_mapMonsterList.size());
                ImGui::Separator();
                ImGui::Text("Enemy Render Count : %I64d", g_pScene->m_vecEnemyRenderList.size());
                ImGui::Text("Enemy Data Count : %I64d", g_pScene->m_mapEnemyList.size());
#else
                ImGui::Text("Objects : [ %d ]", g_pScene->m_vectorCulledObjectPtrList.size());
                ImGui::Text("Monster Render Count : %d", g_pScene->m_vecMonsterRenderList.size());
                ImGui::Text("Monster Data Count : %d", g_pScene->m_mapMonsterList.size());
                ImGui::Separator();
                ImGui::Text("Enemy Render Count : %d", g_pScene->m_vecEnemyRenderList.size());
                ImGui::Text("Enemy Data Count : %d", g_pScene->m_mapEnemyList.size());            
#endif
                ImGui::Text("\\cEnemyTimer diff : \\r[ %.2f ]", max(0.01f, 1.0f - fCounter));
                ImGui::Separator();
               ImGui::Text("Render Distance : [%d]", (int)(g_pScene->m_fFogEndValue + g_pD3dApp->m_pCamera->m_fRenderDistance));
               if (g_pD3dApp->m_pEffectRender)
                {

                   ImGui::Text("Particles : C[%d], O[%d], R[%d]",
                        g_pD3dApp->m_pEffectRender->m_nParticleEffectCount,
                        g_pD3dApp->m_pEffectRender->m_nObjectParticleEffectRender,
                        g_pD3dApp->m_pEffectRender->m_nParticleEffectRender);
     
                   ImGui::Text("Sprites : C[%d], R[%d]",
                       g_pD3dApp->m_pEffectRender->m_nSpriteEffectCount,
                       g_pD3dApp->m_pEffectRender->m_nSpriteEffectRender);
                 

                    ImGui::Text("Objects : C[%d], R[%d]",
                        g_pD3dApp->m_pEffectRender->m_nObjectEffectCount,
                        g_pD3dApp->m_pEffectRender->m_nObjectEffectRender);
                  

                    ImGui::Text("Traces : C[%d], R[%d]",
                        g_pD3dApp->m_pEffectRender->m_nTraceEffectCount,
                        g_pD3dApp->m_pEffectRender->m_nTraceEffectRender);
                 
                }

                //draw D3D.drawSubSet call counts
               ImGui::Text("DrawSubset Call Counts : %d", (int)g_pD3dApp->m_iDrawSubsetCallCount);
               ImGui::Separator();
               ImGui::Text("Memory");
               ImGui::Separator();

                wsprintf(buf, "%ld%%", mem->dwMemoryLoad);
                ImGui::Text("Memmory using persent: %s", buf);
      
                sprintf(buf, "%-10.2fMb", (mem->dwTotalPhys / 1024.0 / 1024.0));
                ImGui::Text("Memmory total phys: %s", buf);
      
                sprintf(buf, "%-10.2fMb", (mem->dwAvailPhys / 1024.0 / 1024.0));
                ImGui::Text("Memmory avail phys: %s", buf);
               
                sprintf(buf, "%-10.2fMb", (mem->dwTotalPageFile / 1024.0 / 1024.0));
                ImGui::Text("Memmory TotalPageFile: %s", buf);
               
                sprintf(buf, "%-10.2fMb", (mem->dwAvailPageFile / 1024.0 / 1024.0));
                ImGui::Text("Memmory AvailPageFile: %s", buf);
                
                sprintf(buf, "%-10.2fMb", (mem->dwTotalVirtual / 1024.0 / 1024.0));
                ImGui::Text("Memmory TotalVirtual: %s", buf);
              
                sprintf(buf, "%-10.2fMb", (mem->dwAvailVirtual / 1024.0 / 1024.0));
                ImGui::Text("Memmory AvailVirtual: %s", buf);
              
                ImGui::EndTabItem();
            }*/
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
#define REQUEST_TIME_EVOPANEL 300ms
bool EvoAdminPanel::TrySendCommand(std::string commandString)
{
    if (commandString.size() > SIZE_MAX_CHAT_MESSAGE)
        return false;

    auto deltaTime = std::chrono::system_clock::now() - m_lastCommand;
    if (deltaTime >= REQUEST_TIME_EVOPANEL)
    {
        g_pGameMain->m_pChat->ProcessChatCommand((char*)commandString.c_str());
        m_lastCommand = std::chrono::system_clock::now();
        return true;
    }
    return false;
}