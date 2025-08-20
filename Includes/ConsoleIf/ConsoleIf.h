/*
ConsoleIf, is a lightweight header only framework to create 
command line applications

Copyright (C) 2025  Juan D. Martín

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see
<https://www.gnu.org/licenses/>.
*/

#pragma once
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <any>

#if TESTS
#include <gtest/gtest.h>
#include <gtest/gtest_prod.h>
#endif

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
        ConsoleIf(){
            #if DEBUG
            spdlog::set_level(spdlog::level::debug);
            #else
            spdlog::set_level(spdlog::level::err);
            #endif 
        }
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
        
        #if TESTS
        FRIEND_TEST(ConsoleIfTest, TestProcessCommand);
        #endif
        es::result_t<std::string, std::string> process_command(const std::string& cmd);

        std::map<std::string, std::shared_ptr<command_node_t>> command_map;
        bool add_description_to_command(const std::string& description);


        char indicator = '>';
    };

    #if TESTS
    class ConsoleIfTest : public ::testing::Test
    {
    protected:
        ConsoleIf console_if;
    };
    #endif
}
