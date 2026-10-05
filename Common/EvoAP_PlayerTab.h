#include "IEvoImGuiCallback.h"
#include "AtumProtocol.h"

#include <vector>
#include <chrono>				 

struct BlockPlayerInfo
{
	char AccountName[SIZE_MAX_ACCOUNT_NAME];
	EN_BLOCKED_TYPE BlockType;
	int Days;
	char BlockReasonAdmin[SIZE_MAX_BLOCKED_ACCOUNT_REASON];
	char BlockReasonPlayer[SIZE_MAX_BLOCKED_ACCOUNT_REASON];
};

struct MutePlayerInfo
{
	char CharacterName[SIZE_MAX_CHARACTER_NAME];
	int Minutes;
};

class EvoAP_PlayerTab  : public IEvoImGuiCallback
{
public:
	EvoAP_PlayerTab();
	virtual ~EvoAP_PlayerTab();

	// Geerbt über IEvoImGuiCallback
	virtual void RenderImGui() override;
	void AddPlayerInfo(MSG_IC_ADMIN_COMMAND_EVO_WHO_OK* playerInfo);

private:
	void SendPlayerInfoRequest(); // big command, has an extra timer to prevent spamming 	
	void GoToPlayer(std::string charactername);
	void PlayerComeToMe(std::string charactername);
	void KickPlayer(std::string charactername);
	void BlockPlayer(const BlockPlayerInfo& blockinfo);
	void MutePlayer(const MutePlayerInfo& blockinfo);

	// returns true when command has been send
	// prevents spamming server with commands when admin spams the buttons xD
	bool TrySendCommand(std::string commandString);

	void RenderPlayerTableEntry(const CHARACTER& character);
	void RenderBlockInfoPopup();
	void RenderMuteInfoPopup();

private:
	std::vector<MSG_IC_ADMIN_COMMAND_EVO_WHO_OK> m_playerInfo;
	std::chrono::time_point<std::chrono::system_clock> m_lastPlayerInfoRequest; // restrict the requests to not spam the server
	std::chrono::time_point<std::chrono::system_clock> m_lastCommand;			// restrict the requests to not spam the server

	bool m_onlyShowPlayersOnMyMap;

	BlockPlayerInfo m_BlockPlayerInfo;
	bool m_openBlockInfoPopup;
	MutePlayerInfo m_MutePlayerInfo;
	bool m_openMuteInfoPopup;
};