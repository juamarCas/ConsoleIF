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

result_t extract_between_chars(const std::string& str, const char delim_1, const char delim_2) {
    result_t result {true, "", ""};

    auto delim_start = std::find(str.begin(), str.end(), delim_1);
    auto delim_end = std::find(str.rbegin(), str.rend(), delim_2).base();

    if (delim_start == str.end() || delim_end == str.rend().base()) {
        result.is_ok = false;
        result.error_message = "No match found for text: " + str + " and chars " + delim_1 + " " + delim_2;
        return result;
    }

    std::string text(delim_start + 1, delim_end - 1);  // correctly construct substring
    result.message_str = text;

    return result;
}