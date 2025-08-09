#pragma once
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <any>

#include "Result.h"
#include "spdlog/spdlog.h"
#include "Utils.h"

/*
    lets define what a command is for this project
    a command is a string that contains all the information for a command. example:
    get_device_list*Description*<(int)> >> this means get device list which parameter is a int from 1 to n being default n (all the devices)
    the command can have n parameters separated by empty space <(int) (string)>
    the command would look like this: get device list 5
*/


namespace ConsoleIf
{
    typedef struct Command
    {
        std::string command = "";
        std::string description = "";
        std::vector<std::string> parameters;

        Command() = default;

        Command &operator=(const Command &other)
        {
            command = other.command;
            description = other.description;
            parameters = other.parameters;
            return *this;
        }

    } command_t;

    typedef struct Command_node
    {

        command_t command;
        Command_node() = default;
        ~Command_node()
        {
            node_action = nullptr;
        }
        std::map<std::string, std::shared_ptr<Command_node>> next_node{};
        std::function<void(const std::vector<std::any> &)> node_action = nullptr;

    } command_node_t;

    class ConsoleIf
    {
    public:
        ConsoleIf() = default;
        ~ConsoleIf() = default;
        /**
        * @brief Create a command for the console interface
        * 
        * @param command string with the correct format to create a command
        * @param callback function executed for that command  
        * @return true  command created succesfully
        * @return false if not a valid command
        */
        bool create_command(const std::string& command, std::function<void(const std::vector<std::any>&)> callback);
        void begin_app();
        void set_indicator(char indicator)
        {
            this->indicator = indicator;
        }


    private:
        /**
         * @brief validate a structure of a command
         *
         * @param command command to be validated
         * @return Result object Result with results information
         */
        es::result_t<command_t, std::string> valiedate_command(const std::string &command);

        std::map<std::string, std::shared_ptr<command_node_t>> command_map;
        bool add_description_to_command(const std::string& description);

        es::result_t<std::string, std::string> process_command(const std::string& cmd);

        char indicator = '>';
    };
}
