/*
PacketReporter by zach2good, based upon:

XIPivot
Copyright © 2019-2022, Renee Koecher
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met :

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of XIPivot nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

extern "C"
{
#define LUA_BUILD_AS_DLL

#include "lauxlib.h"
#include "lua.h"
}

#include <sol/sol.hpp>

#include "../packet_reporter_core.h"

namespace
{
    std::unique_ptr<sol::state_view> lua;

    void Print(std::string str)
    {
        (*lua)["print"](str);
    }

    void SendPutRequest(sol::table table, std::string data)
    {
        static PacketReporterCore wrCore([](const std::string& str) { Print(str); });
        wrCore.HandlePacketData((uint8_t*)data.data(), data.size());
    }
} // namespace

extern "C" __declspec(dllexport) int luaopen__PacketReporter(lua_State* L)
{
    lua = std::make_unique<sol::state_view>(L);

    (*lua).create_table("_PacketReporter");
    (*lua)["_PacketReporter"]["print"]  = &::Print;
    (*lua)["_PacketReporter"]["submit"] = &::SendPutRequest;

    return 1;
}
