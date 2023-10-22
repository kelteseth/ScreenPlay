// SPDX-License-Identifier: LicenseRef-EliasSteurerTachiom OR AGPL-3.0-only

#include "WindowsPipe.h"

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test WindowsPipe with Python echo server", "[WindowsPipe]")
{
    WindowsPipe client;
    client.setPipeName(L"ScreenPlay");
    REQUIRE(client.start());
    const int count = 100;

    SECTION("Ping server and expect ping back")
    {
        for (size_t i = 0; i < count; i++) {
            std::string message = "ping;";
            REQUIRE(client.writeToPipe(message));

            std::string response;
            REQUIRE(client.readFromPipe(response));
            REQUIRE(response == "pong;");
            std::cout << response <<"ok\n";
        }
    }
}
