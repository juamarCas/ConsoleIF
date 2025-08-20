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
#include "Utils.h"

std::vector<std::string> split_string(const std::string &str, char delim){

    std::vector<std::string> res;
    std::string segment;
    std::stringstream ss(str);

    if (str.find(delim) == std::string::npos) {
        res.emplace_back(str);
        return res;
    }

    while(std::getline(ss, segment, delim))
    {
        res.emplace_back(segment);
    }

    return res;
}

std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
    auto end   = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    
    if (start >= end) return "";
    return std::string(start, end);
}

es::result_t<std::string, std::string> extract_between_chars(const std::string& str, const char delim_1, const char delim_2) {
   
    es::result_t<std::string, std::string> res;

    auto delim_start = std::find(str.begin(), str.end(), delim_1);
    auto delim_end = std::find(str.rbegin(), str.rend(), delim_2).base();

    if (delim_start == str.end() || delim_end == str.rend().base()) {
        res.set_error("No match found for text: " + str + " and chars " + delim_1 + " and " + delim_2);
        return res;
    }

    std::string text(delim_start + 1, delim_end - 1); 
    res = text;
    return res;
}


std::string get_string_until_delimiter(const std::string& input, const std::string& delimiters){
    size_t pos = input.find_first_of(delimiters);
    if (pos != std::string::npos) {
        return input.substr(0, pos);
    }
    return input;
}