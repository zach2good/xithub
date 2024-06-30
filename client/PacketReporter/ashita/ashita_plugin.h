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

#ifndef ASHITA_PacketReporter_HPP_INCLUDED
#define ASHITA_PacketReporter_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "../packet_reporter_core.h"

#include "Ashita.h"

class PacketReporter final : public IPlugin
{
    IAshitaCore* m_AshitaCore; // The pointer to the main AshitaCore object.
    ILogManager* m_LogManager; // The pointer to the main Ashita LogManager object.
    uint32_t m_PluginId;       // The plugins id. (The plugins current base address.)

    std::unique_ptr<PacketReporterCore> wrCore;

public:
    PacketReporter(void);
    ~PacketReporter(void) override;

    // Properties (Plugin Information)
    const char* GetName(void) const override;
    const char* GetAuthor(void) const override;
    const char* GetDescription(void) const override;
    const char* GetLink(void) const override;
    double GetVersion(void) const override;
    double GetInterfaceVersion(void) const override;
    int32_t GetPriority(void) const override;
    uint32_t GetFlags(void) const override;

    // Methods
    bool Initialize(IAshitaCore* core, ILogManager* logger, uint32_t id) override;
    void Release(void) override;

    bool HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked) override;
};

#endif // ASHITA_PacketReporter_HPP_INCLUDED
