#include <cstdlib>

#include "spdlog/spdlog.h"
#include "Utils.h"

#ifndef DEBUG
#define DEBUG 1
#endif

typedef struct Command {
    std::string command     = "";
    std::string description = "";
    std::vector<std::string> parameters;
} command_t;

typedef struct Command_node {
    command_t command;
    std::map<std::string, struct Command_node> next_node {};
    std::function<void(const std::vector<std::any>&)> node_action = nullptr;

} command_node_t;




/**
 * @brief validate a structure of a command
 * 
 * @param command command to be validated
 * @return Result object Result with results information 
 */
Result valiedate_command(const std::string& command, command_t& command_out);


std::map<std::string, command_node_t> command_map;



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


result_t process_command(const std::string& cmd);



/*
    lets define what a command is for this project
    a command is a string that contains all the information for a command. example:
    get_device_list*Description*<(int)[1-n]> >> this means get device list which parameter is a int from 1 to n being default n (all the devices)
    the command can have n parameters separated by empty space <(int)[1-n] (string)>
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
        process_command(command);
        if(command == "exit"){
            spdlog::debug("Exiting app");
            exit(0);
        }else{

        }
    }
}

int main(){

    #if DEBUG
    spdlog::set_level(spdlog::level::debug);
    #else
    spdlog::set_level(spdlog::level::error);
    #endif 

    std::string get_serial_command = "get_serial_list*get a list of every serial device available*<int>";
    std::string get_test = "get_test*this is a test*<int>_list<int string>";
    
    create_command("clear", [](const std::vector<std::any>& args){
        std::system("clear");
    });

    create_command(get_serial_command, get_serial_list);
    create_command(get_test, [](const std::vector<std::any>& args){

    });

    
    begin_app();
    
    return 0;
}

result_t process_command(const std::string& cmd){
    result_t res = {true, "", ""};
    std::vector<std::string> commands = split_string(cmd, ' ');
    std::vector<std::any> args; 
    std::uint8_t command_index = 0;

    auto command_it = commands.begin();
    while(command_it != commands.end()){
        spdlog::debug(*command_it);
        
        if(command_map.find(*command_it) == command_map.end()){
            spdlog::error("Not command found for {}", *command_it);
            res.is_ok = false;
            res.error_message = fmt::format("Not command found for {}", *command_it);
            return res;
        }
        
        command_node_t cmd_node = command_map[*command_it];
        spdlog::debug("command node name: {} has parameter size of: {}", cmd_node.command.command, cmd_node.command.parameters.size());

        if(cmd_node.command.parameters.size() > 0){
            command_it++;
            for(int i = 0; i < cmd_node.command.parameters.size(); i++){
                std::string parameter_type = cmd_node.command.parameters[i];
                spdlog::debug("processing parameter: {} of type {}", *command_it, parameter_type);

                if(parameter_type == "int"){
                    try {
                        int val = std::stoi(*command_it);
                        args.emplace_back(val);
                    } catch (const std::invalid_argument&) {
                        spdlog::error("Invalid int param: \"{}\"", *command_it);
                        res.is_ok = false;
                        res.error_message = fmt::format("Invalid int param: \"{}\"", *command_it);
                        return res;
                    }
                }else if(parameter_type == "string"){
                    args.emplace_back(*command_it);
                }
                
            }
        }
        
        if(command_it == commands.end()) break;
        command_it++;
    }

    spdlog::debug("all parameter size is: {}", args.size());

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

bool create_command(const std::string& command, std::function<void(const std::vector<std::any>&)> callback){
    std::vector commands = split_string(command, '_');
    std::string prev_node = "";

    for(std::uint8_t i = 0; i < commands.size(); i++){
        command_t command_obj;
        result_t command_res = valiedate_command(commands[i], command_obj);

        if(!command_res.is_ok){
            spdlog::error("Not a valid command: {} for the command {}", command_res.error_message, commands[i]);
            command_res.is_ok = false;
            command_res.error_message = fmt::format("Not a valid command: {} for the command {}", command_res.error_message, commands[i]);
            return false;          
        }

        //spdlog::debug("command created for: \"{}\" with description: \"{}\" and parameters: \"{}\"", command_obj.command, command_obj.description, command_obj.parameters);

        //creates node
        command_node_t command_node;
        command_node.command = command_obj;

        if(command_map.find(command_obj.command) == command_map.end()){
            command_map[command_obj.command] = command_node;
        }

        if(prev_node != ""){
            command_map[prev_node].next_node[command_obj.command] = command_node;
        }

        // the last node is the one who is getting the callback
        if(i == commands.size() - 1){
            command_node.node_action = callback;
        }

        prev_node = command_obj.command;
    }

    return true;
}



/*
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
there are 3 basic parameter types:
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
Result valiedate_command(const std::string& command, command_t& command_out){
    result_t res = {true, "", ""};
    std::string description = "";
    command_out.command = command;
    
    //check if any description opener
    std::uint8_t astrk_count = std::count(command.begin(), command.end(), '*');
    if(astrk_count > 0 && astrk_count != 2){
        res.is_ok = false;
        res.error_message = "A valid description has just two \"*\"";
        return res;

    }else if(astrk_count == 2){
        result_t desc = extract_between_chars(command, '*', '*');
        auto start = std::find(command.begin(), command.end(), '*');

        if (!desc.is_ok){
            res.is_ok = false;
            res.error_message = desc.error_message;
            return res;
        }
        
        spdlog::debug("Extracted description: {}", desc.message_str);
        command_out.description = desc.message_str;
        if(desc.message_str != ""){
            // if there is any description, just strip it and just get the command
            std::string strip_cmd(command.begin(), start);
            command_out.command = strip_cmd;
            spdlog::debug(desc.message_str);
        }
            
        
    }
    
    //check if has any parameter
    std::uint8_t triangle_back_count = std::count(command.begin(), command.end(), '<');
    std::uint8_t triangle_front_count = std::count(command.begin(), command.end(), '>');

    bool has_parameters = false;
    if (triangle_back_count > 0 and triangle_back_count != 1 and triangle_front_count > 0 and triangle_front_count != 1){
        res.is_ok = false;
        res.error_message = "Invalid parameter closure format";
        return res;
    }else if(triangle_back_count == 1 and triangle_front_count == 1){
        has_parameters = true;
    }

    if(has_parameters){
        result_t parameters = {true, "", ""};
        parameters = extract_between_chars(command,'<','>');

        if (!parameters.is_ok){
            res.is_ok = false;
            res.error_message = parameters.error_message;
            return res;
        }

        spdlog::debug("The parameters of the command \"{}\" are: {}", command, parameters.message_str);
        std::vector<std::string> parameters_arr;
        parameters_arr = split_string(parameters.message_str, ' ');

        for(auto parameter : parameters_arr){
            spdlog::debug("parameter: {}", parameter);
     
            if (parameter != "int" && parameter != "string"){
                res.is_ok = false;
                res.error_message = "Not valid parameter found: " + parameter;
                return res;
            }

            command_out.parameters.emplace_back(parameter);
        }
        
    }
    return res;
}


