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

#include "ConsoleIf.h"

namespace ConsoleIf
{
    void ConsoleIf::begin_app()
    {
        bool end_program = false;

#if DEBUG == 0
        std::system("clear");
#endif

        while (!end_program)
        {
            std::cout << indicator << " ";
            std::string command;
            std::getline(std::cin, command);
            command = trim(command);
            auto result = process_command(command);
            if (!result)
            {
                spdlog::error("Error processing command: {}", result.Error());
            }
        }
    }

    es::result_t<std::string, std::string> ConsoleIf::process_command(const std::string &cmd)
    {
        es::result_t<std::string, std::string> res;
        std::vector<std::string> commands = split_string(cmd, ' ');
        std::vector<std::any> args;
        std::shared_ptr<command_node_t> current_node = nullptr;
        auto *current_map = &command_map;
        std::function<void(const std::vector<std::any> &)> action = nullptr;

        for(std::uint8_t i = 0; i < commands.size() - 1 ; i++)
        {
            if(current_map->find(commands[i]) != current_map->end())
            {
                spdlog::debug("Command {} found", commands[i]);
                auto parameter_map = (*current_map)[commands[i]];
                std::uint8_t parameter_size = parameter_map->command.parameters.size();
                spdlog::debug("Command has {} parameters", parameter_size);
                std::string current_command = commands[i];

                if ((*current_map)[current_command]->node_action != nullptr)
                {
                    action = (*current_map)[current_command]->node_action;
                    spdlog::debug("Action found for command {}", current_command);
                }

                for (std::uint8_t j = 0; j < parameter_size; j++)
                {
                    spdlog::debug("Command parameter {}: {}", j, (*current_map)[current_command]->command.parameters[j]);
                    i++;
                    if ((*current_map)[current_command]->command.parameters[j] == "int")
                    {
                        try
                        {
                            int int_arg = std::stoi(commands[i]);
                            args.push_back(int_arg);
                        }
                        catch (const std::invalid_argument &e)
                        {
                            res.set_error(fmt::format("Parameter {} is not a valid int", commands[i + 1]));
                            return res;
                        }
                        catch (const std::out_of_range &e)
                        {
                            res.set_error(fmt::format("Parameter {} is out of range for int", commands[i + 1]));
                            return res;
                        }
                    }
                    else if ((*current_map)[current_command]->command.parameters[j] == "string")
                    {
                        args.push_back(commands[i]);
                    }
                }

                 if (current_map->find(current_command) == current_map->end())
                {
                    spdlog::debug("No next command found, breaking");
                    res.set_error(fmt::format("Command {} not found", current_command));
                    return res;
                }
                current_map = &((*current_map)[current_command]->next_node);

                // since if it has an action is because it found the last node, and to avoid the command_it is stuck in a parameter, in case parameters exist
                // at the end of the loop, just move the pointer to next command
                if (i >= commands.size() - 1 && action != nullptr){
                    break;
                }

                if (parameter_size > 0)
                {
                    if (i >= commands.size() - 1)
                    {
                        spdlog::debug("No more commands, breaking");
                        res.set_error(fmt::format("Command {} not found", current_command));
                        return res;
                    }
                }
            }
        }

        if (action != nullptr)
        {
            spdlog::debug("Executing action for command {}", cmd);
            action(args);
        }
        else
        {
            res.set_error(fmt::format("This command {} was not found", cmd));
            return res;
        }
        return res;

    }

    bool ConsoleIf::create_command(const std::string &command, std::function<void(const std::vector<std::any> &)> callback)
    {
        std::vector commands = split_string(command, '_');
        std::shared_ptr<command_node_t> prev_node;

        auto *current_map = &command_map;

        for (std::uint8_t i = 0; i < commands.size(); i++)
        {
            command_t command_obj;
            auto command_res = valiedate_command(commands[i]);

            if (!command_res)
            {
                spdlog::error("Not a valid command: {} for the command {}", command_res.Error(), commands[i]);
                command_res.set_error(fmt::format("Not a valid command: {} for the command {}", command_res.Error(), commands[i]));
                return false;
            }

            command_obj = *command_res;

            std::shared_ptr<command_node_t> command_node = std::make_shared<command_node_t>();
            command_node->command = command_obj;

            if (current_map->find(command_obj.command) == current_map->end())
            {
                spdlog::debug("Adding command {} to map", command_obj.command);
                (*current_map)[command_obj.command] = command_node;
                spdlog::debug("check command map: {}", (*current_map)[command_obj.command]->command.command);
            }

            current_map = &((*current_map)[command_obj.command]->next_node);

            if (i == commands.size() - 1)
            {
                // if is the last command, set the action
                command_node->node_action = callback;
                spdlog::debug("Setting action for command: {}", command_node->command.command);
            }
        }
        spdlog::debug("Command {} created successfully", command);
        return true;
    }

    /*
the commands are stored in a series of node in a tree, thats why is using has maps, to have a track of the
commands and its next step.

remember this: in this project, "**" refers a description of a command
               also "<>" refers as all the posible parameters

RULES:
1)  validates first the position of the ** description
        > validates if closes somewhere
        > the only valid character after a ** closes is "<" not even an empty space
2) validates if has parameters
        > validate <> position
        > validate if correct position of the ()
    the program will validate the int and float format. The user is in charge of validating the string format
*/

    /*
    Parameter rules:
    there are 2 basic parameter types:
        > int
        > string

        parameter structure:
            > () where defines the type (int or string)
            example:
            (int)
            (string)
    */

  
    es::result_t<command_t, std::string> ConsoleIf::valiedate_command(const std::string &command)
    {
        command_t command_out;
        es::result_t<command_t, std::string> res;
        std::string description = "";
        command_out.command = command;

        // check if any description opener
        std::uint8_t astrk_count = std::count(command.begin(), command.end(), '*');
        if (astrk_count > 0 && astrk_count != 2)
        {
            res.set_error("A valid description has just two \"*\"");
            return res;
        }
        else if (astrk_count == 2)
        {
            auto desc = extract_between_chars(command, '*', '*');
            auto start = std::find(command.begin(), command.end(), '*');

            if (!desc)
            {
                res.set_error(desc.Error());
                return res;
            }

            spdlog::debug("Extracted description: {}", *desc);
            command_out.description = *desc;
            if (*desc != "")
            {
                // if there is any description, just strip it and just get the command
                std::string strip_cmd(command.begin(), start);
                command_out.command = strip_cmd;
                spdlog::debug("Stripped command: {}", strip_cmd);
            }
        }

        // check if has any parameter
        std::uint8_t triangle_back_count = std::count(command.begin(), command.end(), '<');
        std::uint8_t triangle_front_count = std::count(command.begin(), command.end(), '>');

        bool has_parameters = false;
        if (triangle_back_count > 0 and triangle_back_count != 1 and triangle_front_count > 0 and triangle_front_count != 1)
        {
            res.set_error("Invalid parameter closure format");
            return res;
        }
        else if (triangle_back_count == 1 and triangle_front_count == 1)
        {
            has_parameters = true;
        }else if(triangle_back_count != triangle_front_count){
            res.set_error("Invalid parameter closure format, missing a closing or opening triangle bracket");
            return res;
        }

        if (has_parameters)
        {
            auto parameters = extract_between_chars(command, '<', '>');

            if (!parameters)
            {
                res.set_error(parameters.Error());
                return res;
            }

            spdlog::debug("The parameters of the command \"{}\" are: {}", command, *parameters);
            std::vector<std::string> parameters_arr;
            parameters_arr = split_string(*parameters, ' ');

            for (auto parameter : parameters_arr)
            {
                spdlog::debug("parameter: {}", parameter);

                if (parameter != "int" && parameter != "string")
                {
                    res.set_error("Not valid parameter found: " + parameter);
                    return res;
                }

                command_out.parameters.emplace_back(parameter);
            }
        }

        if (astrk_count == 2 || has_parameters)
        {
            std::string strip_cmd = get_string_until_delimiter(command, "*<");
            command_out.command = trim(strip_cmd);
            spdlog::debug("Command stripped to: {}", command_out.command);
        }

        res = command_out;
        return res;
    }

}