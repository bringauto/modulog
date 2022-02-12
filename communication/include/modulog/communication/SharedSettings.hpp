#pragma once
#include <filesystem>
namespace modulog::communication{
    struct SharedSettings{
        struct ServerSettings{
            int portNumber = 1234; // Port number, where will be TCP server listening
            int connectionTimeoutSec = 3; // How many seconds has agent to connect to server before is killed
        } ServerSettings;
        struct LogSettings{
            std::filesystem::path agentListPath = "./agents-enabled.conf";
            std::filesystem::path logsDestination = "./logs"; // Where will be saved logs (folder is created if not existing)
            int isAliveIntervalSec = 1; // How often check, if agent is not freezed (in seconds)
            int isAliveTimeoutSec = 2; // How long wait for agents isAlive response
            std::filesystem::path sharedAgentsConfig = ""; // Path to file - its content will be sent to all agents (if not using, leave this variable empty)
        } LogSettings;
    };
}