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
#include <memory>
#include <string>
#include <stdexcept>
#include <variant>
#include <utility>

/*
To implement this with custom objects, it needs to define a constructor with no parameters
and a copy constructor, and the operator= to copy the values from one object to another.
*/
namespace es {

    /**
     * @brief Result structure for function return values
     * 
     * @tparam R (result) Type of the successful result value
     * @tparam E (error value) Type of the error value
     */
    template<typename R, typename E = std::string>
    struct result{

        R * value = nullptr;
        E * error = nullptr;

        bool is_ok;

        result(){
            is_ok = true;
            value = new R;
        }

        ~result(){ 
            if(value) {
                delete value;
            }
            if(error) {
                delete error;
            }
        }

        void set_error(const E& error_val) {

            if(error) {
                delete error;
            }

            error = new E;
            *error = error_val;
            is_ok = false;
        }

        void set_value(const R& value_val) {
            if(value) {
                delete value;
            }
            value = new R;
            *value = value_val;
            is_ok = true;
        }

        E Error() const { return *error; }
        R Value() const { return *value; }

        operator bool() const {return is_ok;}
        R operator*() const{
            if (!is_ok) {
                throw std::runtime_error("Attempted to dereference an error result");
            }
            return *value;
        }

        R& operator=(const R& value_val) {
            
            if (value) {
                delete value;
            }

            value = new R;
            *value = value_val;
            return *value;
        }
        
    };

    template<typename R, typename E = std::string>
    using result_t = result<R, E>;

}