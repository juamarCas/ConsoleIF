#include <cstdlib>

#include "spdlog/spdlog.h"
#include "Utils.h"
#ifndef DEBUG
#define DEBUG 1
#endif

typedef struct command_node {
    std::string node_name;
    std::string node_parameters_structure;
    std::string node_description;
    std::map<std::string, struct command_node> next_node {};
    std::function<void(const std::vector<std::any>&)> node_action = nullptr;
} Command_node;

typedef struct command {
    std::string command     = "";
    std::string description = "";
    std::string parameters  = ""; 
} Command;


/**
 * @brief validate a structure of a command
 * 
 * @param command command to be validated
 * @return Result object Result with results information 
 */
Result valiedate_command(const std::string& command, Command& command_out);


std::map<std::string, Command_node> command_map;



/**
 * @brief Create a command for the console interface
 * 
 * @param command string with the correct format to create a command
 * @param callback function executed for that command  
 * @return true  command created succesfully
 * @return false if not a valid command
 */

bool create_command(const std::string& command, std::function<void(const std::vector<std::any>&)> callback);
bool add_description_to_command(const std::string& description);



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

        if(command == "exit"){
            spdlog::debug("Exiting app");
            exit(0);
        }
    }
}

int main(){

    #if DEBUG
    spdlog::set_level(spdlog::level::debug);
    #else
    spdlog::set_level(spdlog::level::error);
    #endif 

    std::string get_serial_command = "get_serial_list*get a list of every serial device available*<(int)[1-n] (string)>";
    
    create_command("clear", [](const std::vector<std::any>& args){
        std::system("clear");
    });

    create_command(get_serial_command, get_serial_list);
    begin_app();
    
    return 0;
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

    //validate each command
    for(auto cmd : commands){
        Command command_obj;
        result_t res = valiedate_command(cmd, command_obj);
        if(res.is_ok == true){
            continue;  
        } 
        else spdlog::error("Not a valid command: {} for the command {}", res.error_message, cmd);
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
        > validate if correct position of the ()[]
    the program will validate the int and float format. The user is in charge of validating the string format
*/


/*
Parameter rules:
there are 3 basic parameter types:
    > int
    > string

    parameter structure:
        > () where defines the type (int or string)
        > [] in case of int you can define a range [a-b] (optional)
        > [] in case of string, is the max length of the string

        example:
        (int)[0-100]
        (int)
        (int)[1-n]
        (string)[n]

rules:
    > int. Can be configured to accept or not a range, if not set a range, by default de range would be -inf to inf, but most have a default value
      default value can be a number or the letter "n" that means all info available

    > string is the simplest since the user only need to stablish that waits a string. But the user is in charge to validate that string

*/

/**
 * @brief parses a string and check if is a valid command added and creates a command node object for that string
 * 
 * @param command command to be validated
 * @param command_out command object with the command information
 * @return Result result object with result information
 */
Result valiedate_command(const std::string& command, Command& command_out){
    result_t res = {true, "", ""};
    std::string description = "";
    
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

            //check if int
            
            //check if string
        }
         
    }
   

    return res;
}


