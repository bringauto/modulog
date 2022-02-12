#include <modulog/agent_client/Helpers.hpp>
#include <modulog/agent_client/AgentClient.hpp>

#include <iostream>
#include <thread>
#include <fstream>



int getTemperature(std::ifstream &tempSource){
    std::string tempStr;
    getline(tempSource, tempStr);
    int temperature = std::stoi(tempStr) / 1000; // /1000 to get degree celsius

    tempSource.clear();
    tempSource.seekg(0); // Read from beginning
    return temperature;
}

int main(int argc, char** argv){

    auto programStart = std::chrono::system_clock::now();
    nlohmann::json configJson = modulog::agent_client::Helpers::parseConfig(argv[0]);

    if(!configJson.contains("id")){
        std::cerr << "Include config with id defined." << std::endl;
        throw std::runtime_error("...");
    }
    int temperatureNotSmallerThan = INT_MIN;
    if(configJson.contains("temperatureNotSmallerThan")){
        temperatureNotSmallerThan = configJson["temperatureNotSmallerThan"];
    }
    int temperatureNotBiggerThan = INT_MAX;
    if(configJson.contains("temperatureNotBiggerThan")){
        temperatureNotBiggerThan = configJson["temperatureNotBiggerThan"];
    }
    int logInterval = 4;
    if(configJson.contains("logInterval")){
        logInterval = configJson["logInterval"];
    }

    if(!configJson.contains("temperatureSource")){
        std::cerr << "Specify temperatureSource!" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << configJson["temp"] << std::endl;
    std::filesystem::path src = configJson["temperatureSource"];
    std::ifstream tempSource(src);

    if(!tempSource.is_open()){
        std::cerr << "Cannot open file temperature source." << std::endl;
        exit(EXIT_FAILURE);
    }

    auto ioContext = std::make_shared<asio::io_context>();
    modulog::agent_client::AgentClient agentClient(ioContext, false, configJson["id"] );
    agentClient.initClient();
    while(true){
        int temperature = getTemperature(tempSource);
        if(temperatureNotSmallerThan > temperature || temperatureNotBiggerThan < temperature){
            auto errMsg = std::make_shared<modulog::communication::LogMessage>(modulog::communication::LogMessage::LOG_MSG_TYPE::ERROR, "temperature", std::to_string(temperature));
            agentClient.sendLog(errMsg);
        }else{
            auto logMsg = std::make_shared<modulog::communication::LogMessage>(modulog::communication::LogMessage::LOG_MSG_TYPE::LOG, "temperature", std::to_string(temperature));
            agentClient.sendLog(logMsg);
        }
        std::this_thread::sleep_for(std::chrono::seconds(logInterval));
    }

    return 0;
}
