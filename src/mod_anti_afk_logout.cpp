/*
 * mod-anti-afk-logout
 * Prevents automatic AFK logout while allowing manual logout
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Config.h"
#include "Log.h"

class AntiAFKLogoutServerScript : public ServerScript
{
public:
    AntiAFKLogoutServerScript() : ServerScript("AntiAFKLogoutServerScript") { }

    bool OnPacketReceive(WorldSession* session, WorldPacket& packet) override
    {
        // Only intercept logout request packets
        if (packet.GetOpcode() != CMSG_LOGOUT_REQUEST)
            return true; // Allow packet to be processed normally

        // Check if module is enabled
        if (!sConfigMgr->GetOption<bool>("AntiAFKLogout.Enable", true))
            return true;

        Player* player = session->GetPlayer();
        if (!player)
            return true;

        // If player has AFK flag set, this is likely an automatic AFK logout
        // Block it and send cancel packet to client
        if (player->isAFK())
        {
            // Check if we should also require out of combat
            bool requireOutOfCombat = sConfigMgr->GetOption<bool>("AntiAFKLogout.OnlyWhenNotInCombat", true);
            
            if (!requireOutOfCombat || !player->IsInCombat())
            {
                // Log the blocked logout if debug is enabled
                if (sConfigMgr->GetOption<bool>("AntiAFKLogout.LogBlocked", false))
                {
                    LOG_INFO("module", "mod-anti-afk-logout: Blocked AFK logout for player {} ({})",
                        player->GetName(), player->GetGUID().ToString());
                }

                // Clear AFK flag to prevent spam
                if (sConfigMgr->GetOption<bool>("AntiAFKLogout.ClearAFKFlag", true))
                {
                    player->ToggleAFK();
                }

                // Send logout cancel to client
                WorldPacket data(SMSG_LOGOUT_CANCEL_ACK, 0);
                session->SendPacket(&data);

                // Block the packet from being processed
                return false;
            }
        }

        // Allow normal logout (manual logout, not AFK)
        return true;
    }
};

class AntiAFKLogoutWorldScript : public WorldScript
{
public:
    AntiAFKLogoutWorldScript() : WorldScript("AntiAFKLogoutWorldScript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        bool enabled = sConfigMgr->GetOption<bool>("AntiAFKLogout.Enable", true);
        LOG_INFO("module", "mod-anti-afk-logout: Module is {}", enabled ? "enabled" : "disabled");
    }
};

void AddAntiAFKLogoutScripts()
{
    new AntiAFKLogoutServerScript();
    new AntiAFKLogoutWorldScript();
}
