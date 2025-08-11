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