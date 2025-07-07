#pragma once

#include <algorithm>
#include <any>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

/**
 * @brief Result of an operation
 *         is_ok determines if the proccess is succesfull
 *         message_str is to give more information or even the result of a proccess
 *         error_message more information in case of any error
 */
typedef struct Result{
    bool is_ok;
    std::string message_str;
    std::string error_message;
}result_t;


std::string trim(const std::string& str);
std::vector<std::string> split_string(const std::string& str, char delim);


/**
 * @brief extract the string that is between two chars
 * 
 * @param str string to be proccesed 
 * @param delim_1 left side delimiter
 * @param delim_2 right side delimiter
 * @return Result object result with is_ok if true the message_str is the extracted string, if is_ok is false, the message is the error
 */
result_t extract_between_chars(const std::string& str, const char delim_1, const char delim_2);