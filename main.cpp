#include <cstdlib>
#include <memory>

#include "spdlog/spdlog.h"
#include "Utils.h"
#include "Result.h"

#ifndef DEBUG
#define DEBUG 1
#endif

typedef struct Command {
    std::string command     = "";
    std::string description = "";
    std::vector<std::string> parameters;

    Command() = default;

    Command& operator=(const Command& other){
        command = other.command;
        description = other.description;
        parameters = other.parameters;
        return *this;
    }
    
} command_t;

typedef struct Command_node {

    command_t command;
    Command_node() = default;
    ~Command_node(){
        node_action = nullptr;
    }
    std::map<std::string, std::shared_ptr<Command_node>> next_node {};
    std::function<void(const std::vector<std::any>&)> node_action = nullptr;

} command_node_t;




/**
 * @brief validate a structure of a command
 * 
 * @param command command to be validated
 * @return Result object Result with results information 
 */
es::result_t<command_t, std::string> valiedate_command(const std::string& command);


std::map<std::string, std::shared_ptr<command_node_t>> command_map;



/**
 * @brief Create a command for the console interface
 * 
 * @param command string with the correct format to create a command
 * @param callback function executed for that command  
 * @return true  command created succesfully
 * @return false if not a valid command
 */

bool create_command(const std::string& command, std::function<void(const std::vector<std::any>&)> callback);
bool create_command(const std::string& command, std::function<void(const int&)> callback);
bool create_command(const std::string& command, std::function<void(void)> callback);

bool add_description_to_command(const std::string& description);


es::result_t<std::string, std::string> process_command(const std::string& cmd);



/*
    lets define what a command is for this project
    a command is a string that contains all the information for a command. example:
    get_device_list*Description*<(int)> >> this means get device list which parameter is a int from 1 to n being default n (all the devices)
    the command can have n parameters separated by empty space <(int) (string)>
    the command would look like this: get device list 5
*/

/**
 * @brief prints the available serial interfaces
 * 
 * @param args NA
 */
void get_serial_list(const std::vector<std::any>& args){
    spdlog::debug("YEY");
}

void begin_app(){
    bool end_program = false;

    #if DEBUG == 0
    std::system("clear");
    #endif

    while(!end_program){
        std::cout<<"> ";
        std::string command;
        std::getline(std::cin, command);
        command = trim(command);
        auto result = process_command(command);
        if(!result){
            spdlog::error("Error processing command: {}", result.Error());
        }
    }
}

void get_test_list(const std::vector<std::any>& args){
    spdlog::debug("Getting test list");
}

int main(){

    #if DEBUG
    spdlog::set_level(spdlog::level::debug);
    #else
    spdlog::set_level(spdlog::level::error);
    #endif 

    std::string get_test = "get_test*this is a test*<int>_list<int string>";
    std::string get_serial_command = "get_serial_list*get a list of every serial device available*<int>";
    create_command(get_test, get_test_list);
    
    create_command("clear", [](const std::vector<std::any>& args){
        std::system("clear");
    });

    create_command("exit", [](const std::vector<std::any>& args){
        spdlog::debug("Exiting app");
        exit(0);
    });

    create_command(get_serial_command, get_serial_list);

    begin_app();
    
    return 0;
}

es::result_t<std::string, std::string> process_command(const std::string& cmd){
    es::result_t<std::string, std::string> res;
    std::vector<std::string> commands = split_string(cmd, ' ');
    std::vector<std::any> args;
    std::shared_ptr<command_node_t> current_node = nullptr;
    auto command_it = commands.begin();
    auto * current_map = &command_map;
    std::function<void(const std::vector<std::any>&)> action = nullptr;
    while(command_it != commands.end()){
        if(current_map->find(*command_it) != current_map->end()){
            spdlog::debug("Command {} found", *command_it);
            auto parameter_map = (*current_map)[*command_it];
            std::uint8_t parameter_size = parameter_map->command.parameters.size();
            spdlog::debug("Command has {} parameters", parameter_size);
            std::string current_command = *command_it;


            if ((*current_map)[current_command]->node_action != nullptr) {
                action = (*current_map)[current_command]->node_action;
                spdlog::debug("Action found for command {}", current_command);
            }


            for (std::uint8_t i = 0; i < parameter_size; i++) {
                spdlog::debug("Command parameter {}: {}", i, (*current_map)[current_command]->command.parameters[i]);
                command_it++;
                if ((*current_map)[current_command]->command.parameters[i] == "int") {
                    try {
                        int int_arg = std::stoi(*(command_it));
                        args.push_back(int_arg);
                    } catch (const std::invalid_argument& e) {
                        res.set_error(fmt::format("Parameter {} is not a valid int", *(command_it + 1)));
                        return res;
                    } catch (const std::out_of_range& e) {
                        res.set_error(fmt::format("Parameter {} is out of range for int", *(command_it + 1)));
                        return res;
                    }
                } else if ((*current_map)[current_command]->command.parameters[i] == "string") {
                    args.push_back(*command_it);
                }
               
            }

            if (current_map->find(current_command) == current_map->end()) {
                spdlog::debug("No next command found, breaking");
                res.set_error(fmt::format("Command {} not found", current_command));
                return res;
            }
            current_map = &((*current_map)[current_command]->next_node);

            // since if it has an action is because it found the last node, and to avoid the command_it is stuck in a parameter, in case parameters exist
            // at the end of the loop, just move the pointer to next command
            if (action != nullptr) break;
        
            if (parameter_size > 0){ 
                if (command_it == commands.end()) {
                    spdlog::debug("No more commands, breaking");
                    break;
                }
                command_it++;
                current_command = *command_it;
                continue;
            }

            

            
        }

        command_it++;
    }

    
    if (action != nullptr) {
        spdlog::debug("Executing action for command {}", cmd);
        action(args);
    } else {
        res.set_error(fmt::format("This command {} was not found", cmd));
        return res;
    }
    return res;
}

/**
 * @brief Defines a custom command
 * 
 * @param command string representing the command
 * @param callback the expected output
 * @return true if created correctly
 * @return false if any error ocurred during the command validation
 */

 // for now, only supports parameters which in the same node map can't have the same name
bool create_command(const std::string& command, std::function<void(const std::vector<std::any>&)> callback){
    std::vector commands = split_string(command, '_');
    std::shared_ptr<command_node_t>  prev_node;

    auto * current_map = &command_map;

    for(std::uint8_t i = 0; i < commands.size(); i++){
        command_t command_obj;
        auto command_res = valiedate_command(commands[i]);

        if(!command_res){
            spdlog::error("Not a valid command: {} for the command {}", command_res.Error(), commands[i]);
            command_res.set_error(fmt::format("Not a valid command: {} for the command {}", command_res.Error(), commands[i]));
            return false;
        }

        command_obj = *command_res;

        std::shared_ptr<command_node_t> command_node = std::make_shared<command_node_t>();
        command_node->command = command_obj;

        if(current_map->find(command_obj.command) == current_map->end()){
            spdlog::debug("Adding command {} to map", command_obj.command);
            (*current_map)[command_obj.command] = command_node;
            spdlog::debug("check command map: {}", (*current_map)[command_obj.command]->command.command);
        }

        current_map = &((*current_map)[command_obj.command]->next_node);
        

        if (i == commands.size() - 1){
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

/**
 * @brief parses a string and check if is a valid command added and creates a command node object for that string
 * 
 * @param command command to be validated
 * @param command_out command object with the command information
 * @return Result result object with result information
 */
es::result_t<command_t, std::string> valiedate_command(const std::string& command){
    command_t command_out;
    es::result_t<command_t, std::string> res;
    std::string description = "";
    command_out.command = command;
    
    //check if any description opener
    std::uint8_t astrk_count = std::count(command.begin(), command.end(), '*');
    if(astrk_count > 0 && astrk_count != 2){
        res.set_error("A valid description has just two \"*\"");
        return res;

    }else if(astrk_count == 2){
        auto desc = extract_between_chars(command, '*', '*');
        auto start = std::find(command.begin(), command.end(), '*');

        if (!desc){
            res.set_error(desc.Error());
            return res;
        }

        spdlog::debug("Extracted description: {}", *desc);
        command_out.description = *desc;
        if(*desc != ""){
            // if there is any description, just strip it and just get the command
            std::string strip_cmd(command.begin(), start);
            command_out.command = strip_cmd;
            spdlog::debug("Stripped command: {}", strip_cmd);
        }
            
        
    }
    
    //check if has any parameter
    std::uint8_t triangle_back_count = std::count(command.begin(), command.end(), '<');
    std::uint8_t triangle_front_count = std::count(command.begin(), command.end(), '>');

    bool has_parameters = false;
    if (triangle_back_count > 0 and triangle_back_count != 1 and triangle_front_count > 0 and triangle_front_count != 1){
        res.set_error("Invalid parameter closure format");
        return res;
    }else if(triangle_back_count == 1 and triangle_front_count == 1){
        has_parameters = true;
    }

    if(has_parameters){
        auto parameters = extract_between_chars(command, '<', '>');
      
        if (!parameters){
            res.set_error(parameters.Error());
            return res;
        }

        spdlog::debug("The parameters of the command \"{}\" are: {}", command, *parameters);
        std::vector<std::string> parameters_arr;
        parameters_arr = split_string(*parameters, ' ');

        for(auto parameter : parameters_arr){
            spdlog::debug("parameter: {}", parameter);
     
            if (parameter != "int" && parameter != "string"){
                res.set_error("Not valid parameter found: " + parameter);
                return res;
            }

            command_out.parameters.emplace_back(parameter);
        }
        
    }

    if(astrk_count == 2 || has_parameters){
        std::string strip_cmd = get_string_until_delimiter(command, "*<");
        command_out.command = trim(strip_cmd);
        spdlog::debug("Command stripped to: {}", command_out.command);
    }

    res = command_out;
    return res;
}


