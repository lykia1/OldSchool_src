#include "StdAfx.h"
#include "EvoAP_PlayerTab.h"

#include "EvoAdminPanel.h"
#include "imgui.h"
#include "imgui_additional.h"

#include "Atumapplication.h"
#include "IMSocketManager.h"
#include "ShuttleChild.h"

#include "INFGameMain.h"
#include "INFGameMainChat.h"

#define PLAYERINFO_REQUEST_TIME 3s
#define COMMAND_REQUEST_TIME 300ms 

EvoAP_PlayerTab::EvoAP_PlayerTab()
{
	m_playerInfo.reserve(75);
	m_lastPlayerInfoRequest = std::chrono::system_clock::now();
	g_pD3dApp->GetAdminPanel()->RegisterCallback("Players", this);
}

EvoAP_PlayerTab::~EvoAP_PlayerTab()
{
}

void EvoAP_PlayerTab::RenderImGui()
{
	m_openBlockInfoPopup = false;
	m_openMuteInfoPopup = false;

	auto deltaTime = std::chrono::system_clock::now() - m_lastPlayerInfoRequest;
	ImGui::BeginDisabled(deltaTime < PLAYERINFO_REQUEST_TIME);
	if (ImGui::Button("Refresh"))
	{
		SendPlayerInfoRequest();
		m_lastPlayerInfoRequest = std::chrono::system_clock::now();
	}
	ImGui::EndDisabled();
	
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Update player list, limited to one update every 3 seconds.");
	}
	ImGui::SameLine();
	ImGui::Checkbox("Show only players on same map as me", &m_onlyShowPlayersOnMyMap);

	ImGui::NewLine();
	ImGui::Text("Nation Balance:");
	int nCountBCUTotal = 0;
	int nCountANITotal = 0;
	int nCountANIMaps = 0;
	int nCountANICity = 0;
	int nCountBCUMaps = 0;
	int nCountBCUCity = 0;
	for (const auto& playerInfo : m_playerInfo)
	{
		if (playerInfo.Character.InfluenceType == INFLUENCE_TYPE_ANI)
		{
			nCountANITotal++;
			if(!IS_CITY_MAP_INDEX(playerInfo.Character.MapChannelIndex.MapIndex))
				nCountANIMaps++;
			else
				nCountANICity++;
		}
		if (playerInfo.Character.InfluenceType == INFLUENCE_TYPE_VCN)
		{
			nCountBCUTotal++;
			if (!IS_CITY_MAP_INDEX(playerInfo.Character.MapChannelIndex.MapIndex))
				nCountBCUMaps++;
			else
				nCountBCUCity++;
		}
	}
	ImGui::Text("     Total: %d BCU / %d ANI", nCountBCUTotal,nCountANITotal);
	ImGui::Text("     On maps: %d BCU / %d ANI", nCountBCUMaps, nCountANIMaps);
	ImGui::Text("     In Cities: %d BCU / %d ANI", nCountBCUCity, nCountANICity);
	ImGui::NewLine();
	if (ImGui::BeginTable("PlayerInfoTable", 3, ImGuiTableFlags_BordersH))
	{
		ImGui::TableSetupColumn("Character/Accountname");
		ImGui::TableSetupColumn("Current Map");
		ImGui::TableSetupColumn("Actions");
		ImGui::TableHeadersRow();
		ImGui::TableNextColumn();

		for (const auto& playerInfo : m_playerInfo)
		{
			if (m_onlyShowPlayersOnMyMap &&
				(playerInfo.Character.MapChannelIndex.MapIndex != g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex 
					|| (playerInfo.Character.MapChannelIndex.MapIndex == g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.MapIndex && 
					playerInfo.Character.MapChannelIndex.ChannelIndex != g_pShuttleChild->m_myShuttleInfo.MapChannelIndex.ChannelIndex))
				
				)
			{
				continue;
			}
			
			RenderPlayerTableEntry(playerInfo.Character);
		}
		ImGui::EndTable();
	}

	if (m_openBlockInfoPopup) { ImGui::OpenPopup("BlockPlayerOptions"); }
	if (m_openMuteInfoPopup) { ImGui::OpenPopup("MutePlayerOptions"); }
	RenderBlockInfoPopup();
	RenderMuteInfoPopup();
}

void EvoAP_PlayerTab::AddPlayerInfo(MSG_IC_ADMIN_COMMAND_EVO_WHO_OK* playerInfo)
{
	m_playerInfo.push_back(*playerInfo);
}

void EvoAP_PlayerTab::SendPlayerInfoRequest()
{
	m_playerInfo.clear();
	TrySendCommand(STRCMD_CS_COMMAND_EVO_WHO); 	
}

void EvoAP_PlayerTab::GoToPlayer(std::string charactername)
{
	std::string commandString = std::string(STRCMD_CS_COMMAND_GOUSER);
	commandString += " " + charactername;

	TrySendCommand(commandString);
}

void EvoAP_PlayerTab::PlayerComeToMe(std::string charactername)
{
  	std::string commandString = std::string(STRCMD_CS_COMMAND_COMEON);
	commandString += " " + charactername;

	TrySendCommand(commandString);
}

void EvoAP_PlayerTab::KickPlayer(std::string charactername)
{
	std::string commandString = std::string(STRCMD_CS_COMMAND_KICK_0);
	commandString += " " + charactername;

	TrySendCommand(commandString);	
}

void EvoAP_PlayerTab::BlockPlayer(const BlockPlayerInfo& blockinfo)
{
	std::string commandString = std::string(STRCMD_CS_COMMAND_NEWACCOUNTBLOCK_0);
	commandString += " " + std::string(blockinfo.AccountName);
	commandString += " " + std::to_string(blockinfo.BlockType);
	commandString += " " + std::to_string(blockinfo.Days);
	commandString += " " + std::string(blockinfo.BlockReasonAdmin);
	commandString += "/" + std::string(blockinfo.BlockReasonPlayer);

	TrySendCommand(commandString);
}

void EvoAP_PlayerTab::MutePlayer(const MutePlayerInfo& blockinfo)
{
	std::string commandString = std::string(STRCMD_CS_COMMAND_CHATFORBID);
	commandString += " " + std::string(blockinfo.CharacterName);
	commandString += " " + std::to_string(blockinfo.Minutes);

	TrySendCommand(commandString);
}

bool EvoAP_PlayerTab::TrySendCommand(std::string commandString)
{		
	if (commandString.size() > SIZE_MAX_CHAT_MESSAGE)
		return false;

	auto deltaTime = std::chrono::system_clock::now() - m_lastCommand;
	if (deltaTime >= COMMAND_REQUEST_TIME)
	{ 		
		g_pGameMain->m_pChat->ProcessChatCommand((char*)commandString.c_str());
		m_lastCommand = std::chrono::system_clock::now();
		return true;
	}
	return false;
}

void EvoAP_PlayerTab::RenderPlayerTableEntry(const CHARACTER& character)
{
	ImGui::PushID(character.AccountUniqueNumber);

	ImGui::PushStyleColor(ImGuiCol_Text, EvoAdminPanel::GetInfluenceColor(character.InfluenceType));
	ImGui::Text(character.CharacterName);
	ImGui::SameLine();	
	ImGui::Text("/");
	ImGui::SameLine();
	ImGui::Text(character.AccountName);
	ImGui::PopStyleColor();

	ImGui::TableNextColumn();
	ImGui::Text("%s CH:%d", g_pGameMain->GetMapNameByIndex(character.MapChannelIndex.MapIndex), character.MapChannelIndex.ChannelIndex);
	ImGui::TableNextColumn();
	if (ImGui::Button("Go"))
	{
		GoToPlayer(character.CharacterName);
	}
	ImGui::SameLine();
	if (ImGui::Button("Come"))
	{
		PlayerComeToMe(character.CharacterName);
	}
	ImGui::SameLine();
	if (ImGui::Button("Mute"))
	{
		strcpy_s(m_MutePlayerInfo.CharacterName, character.CharacterName);
		m_openMuteInfoPopup = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Kick"))
	{
		KickPlayer(character.CharacterName);
	}
	ImGui::SameLine();
	if (ImGui::Button("Ban"))
	{
		strcpy_s(m_BlockPlayerInfo.AccountName, character.AccountName);
		m_openBlockInfoPopup = true;
	}
	ImGui::TableNextColumn();

	ImGui::PopID();
}

void EvoAP_PlayerTab::RenderBlockInfoPopup()
{
	ImGui::SetNextWindowSize(ImVec2(370.0f, 150.0f));
	if (ImGui::BeginPopup("BlockPlayerOptions", ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
	{
		std::string header = "Block Account: " + std::string(m_BlockPlayerInfo.AccountName);
		ImGui::Text(header.c_str());
		ImGui::Separator();

		const std::vector<std::string> blockOptions =
		{
			"Unknown", "Normal", "Related Money", "Related Item",
			"Related SpeedHack", "Related Chatting", "Related GameBug"
		};

		ImGui::SimpleCombo("Block type", blockOptions, reinterpret_cast<int*>(&m_BlockPlayerInfo.BlockType));
		ImGui::InputInt("Days blocked", &m_BlockPlayerInfo.Days);
		if (m_BlockPlayerInfo.Days < 0) m_BlockPlayerInfo.Days = 0;
		ImGui::InputText("Reason for Admin", m_BlockPlayerInfo.BlockReasonAdmin, sizeof(m_BlockPlayerInfo.BlockReasonAdmin));
		ImGui::InputText("Reason for Player", m_BlockPlayerInfo.BlockReasonPlayer, sizeof(m_BlockPlayerInfo.BlockReasonPlayer));

		if (ImGui::Button("Block") && m_BlockPlayerInfo.Days > 0 
			&& strlen(m_BlockPlayerInfo.BlockReasonAdmin) > 0 
			&& strlen(m_BlockPlayerInfo.BlockReasonPlayer) > 0)
		{
			BlockPlayer(m_BlockPlayerInfo);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void EvoAP_PlayerTab::RenderMuteInfoPopup()
{
	ImGui::SetNextWindowSize(ImVec2(370.0f, 80.0f));
	if (ImGui::BeginPopup("MutePlayerOptions", ImGuiWindowFlags_::ImGuiWindowFlags_NoResize))
	{
		std::string header = "Mute Character: " + std::string(m_MutePlayerInfo.CharacterName);
		ImGui::Text(header.c_str());
		ImGui::Separator();

		ImGui::InputInt("Minutes", &m_MutePlayerInfo.Minutes);
		if (m_MutePlayerInfo.Minutes < 0) m_MutePlayerInfo.Minutes = 0;

		if (ImGui::Button("Mute") &&
			m_MutePlayerInfo.Minutes > 0)
		{
			MutePlayer(m_MutePlayerInfo);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
