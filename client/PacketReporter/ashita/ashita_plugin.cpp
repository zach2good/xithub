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
#include <string>

#pragma comment(lib, "psapi.lib")
#include <psapi.h>

PacketReporter::PacketReporter(void)
    : m_AshitaCore{nullptr}
    , m_LogManager{nullptr}
    , m_PluginId(0)
    , ntsys_{0}
    , gcmainsys_{0}
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

    auto name = this->m_AshitaCore->GetConfigurationManager()->GetString("boot", "bootloader", "gamemodule");
    if (name == nullptr)
    {
        name = "FFXiMain.dll";
    }

    MODULEINFO minfo{};
    if (!::GetModuleInformation(::GetCurrentProcess(), ::GetModuleHandleA(name), &minfo, sizeof(MODULEINFO)))
    {
        return false;
    }

    this->ntsys_     = Ashita::Memory::FindPattern(reinterpret_cast<uintptr_t>(minfo.lpBaseOfDll), minfo.SizeOfImage, "A1????????8B88B4000000C1E907F6C101E9", 1, 0);
    this->gcmainsys_ = Ashita::Memory::FindPattern(reinterpret_cast<uintptr_t>(minfo.lpBaseOfDll), minfo.SizeOfImage, "A1????????5685C05774??8BB0????????85F6", 1, 0);

    if (this->ntsys_ == 0 || this->gcmainsys_ == 0)
    {
        return false;
    }

    this->reporterCore = std::make_unique<PacketReporterCore>([this](const std::string& msg) {
        this->m_AshitaCore->GetChatManager()->Write(1, false, msg.c_str());
    });

    // TODO: load client token from config

    std::string str = std::string("[packetreporter] Client Token: ") + (!clientToken.empty() ? clientToken : std::string("<empty>"));
    this->m_AshitaCore->GetChatManager()->Write(1, false, str.c_str());

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
            std::string str = "[packetreporter] Setting Client Token: " + args[1];
            this->m_AshitaCore->GetChatManager()->Write(1, false, str.c_str());

            return true;
        }
        else
        {
            std::string str = "[packetreporter] How to use: (TODO)";
            this->m_AshitaCore->GetChatManager()->Write(1, false, str.c_str());
        }

        return true;
    }

    return false;
}

bool PacketReporter::HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    if (clientToken.empty())
    {
        return false;
    }

    PacketReporterCore::CharacterInfo info;

    // turn clientToken string into a uint64_t
    info.clientToken = std::stoi(clientToken);

    info.name     = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberName(0);
    info.zoneId   = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberZone(0);
    info.serverId = this->get_character_world(this->get_selected_character_index());

    this->reporterCore->HandlePacketData(info, modified, size);

    return false;
}

/**
     * Returns the index of the currently selected character that is playing.
     *
     * @return {uint32_t} The selected character index.
     */
auto PacketReporter::get_selected_character_index(void) const -> uint32_t
{
    return Ashita::Memory::SafeReadPtr<uint32_t>(this->ntsys_, {0x00, 0x00, 0x80}, 0);
}

/**
     * Returns the world index of the character at the given index.
     *
     * @param {uint32_t} index - The character index.
     * @return {uint32_t} The selected character world index.
     */
auto PacketReporter::get_character_world(const uint32_t index) const -> uint16_t
{
    const auto idx = static_cast<int32_t>(0x13824 + (index * 0x8C));

    return Ashita::Memory::SafeReadPtr<uint16_t>(this->gcmainsys_, {0x00, 0x00, idx + 0x06}, 0);
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
