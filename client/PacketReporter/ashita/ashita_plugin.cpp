/**
 * PacketReporter by zach2good, based upon:
 *
 * Ashita Example Plugin - Copyright (c) Ashita Development Team
 * Contact: https://www.ashitaxi.com/
 * Contact: https://discord.gg/Ashita
 *
 * This file is part of Ashita Example Plugin.
 *
 * Ashita Example Plugin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ashita Example Plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Ashita Example Plugin.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ashita_plugin.h"

#pragma comment(lib, "psapi.lib")
#include <psapi.h>

PacketReporter::PacketReporter(void)
    : m_AshitaCore{nullptr}
    , m_LogManager{nullptr}
    , m_PluginId(0)
{
}

PacketReporter::~PacketReporter(void)
{
}

const char* PacketReporter::GetName(void) const
{
    return "PacketReporter";
}

const char* PacketReporter::GetAuthor(void) const
{
    return "zach2good";
}

const char* PacketReporter::GetDescription(void) const
{
    return "";
}

const char* PacketReporter::GetLink(void) const
{
    return "";
}

double PacketReporter::GetVersion(void) const
{
    return 1.0f;
}

double PacketReporter::GetInterfaceVersion(void) const
{
    return ASHITA_INTERFACE_VERSION;
}

int32_t PacketReporter::GetPriority(void) const
{
    return 0;
}

uint32_t PacketReporter::GetFlags(void) const
{
    return (uint32_t)Ashita::PluginFlags::Legacy;
}

bool PacketReporter::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id)
{
    this->m_AshitaCore = core;
    this->m_LogManager = logger;
    this->m_PluginId   = id;

    this->reporterCore = std::make_unique<PacketReporterCore>([this](const std::string& msg) {
        this->m_AshitaCore->GetChatManager()->Write(1, false, msg.c_str());
    });

    // TODO: load client token from config

    return true;
}

void PacketReporter::Release(void)
{
    this->reporterCore = nullptr;
}

auto PacketReporter::HandleCommand(int32_t mode, const char* command, bool injected) -> bool
{
    UNREFERENCED_PARAMETER(mode);
    UNREFERENCED_PARAMETER(injected);

    std::vector<std::string> args{};
    const auto count = Ashita::Commands::GetCommandArgs(command, &args);

    HANDLECOMMAND("/packetreporter")
    {
        if (count >= 2)
        {
            clientToken     = args[1];
            std::string str = "[packetreporter] Command executed: /packetreporter " + args[1];
            this->m_AshitaCore->GetChatManager()->Write(1, false, str.c_str());

            return true;
        }
        else
        {
            std::string str = "[packetreporter] Command executed: /packetreporter";
            this->m_AshitaCore->GetChatManager()->Write(1, false, str.c_str());
        }

        return true;
    }

    return false;
}

bool PacketReporter::HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    PacketReporterCore::CharacterInfo info;
    info.name     = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberName(0);
    info.zoneId   = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberZone(0);
    info.serverId = 0; // TODO
    this->reporterCore->HandlePacketData(info, modified, size);
    return false;
}

extern "C" __declspec(dllexport) IPlugin* __stdcall expCreatePlugin(const char* args)
{
    UNREFERENCED_PARAMETER(args);
    return new PacketReporter();
}

extern "C" __declspec(dllexport) double __stdcall expGetInterfaceVersion(void)
{
    return ASHITA_INTERFACE_VERSION;
}
