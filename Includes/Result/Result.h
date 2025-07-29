#pragma once
#include <memory>
#include <string>
#include <stdexcept>
#include <variant>
#include <utility>

namespace es {

    /**
     * @brief Result structure for function return values
     * 
     * @tparam R (result) Type of the successful result value
     * @tparam E (error value) Type of the error value
     */
    template<typename R, typename E = std::string>
    struct result{
        
        typedef union data {
            data(){ 
                value = nullptr;
            };

            ~data(){ 
                if (value){
                    delete value;
                    return;
                }

                delete error;
            };

            R * value;
            E * error;
        } data_u;

        data_u * data = nullptr;
       
        bool is_ok;

        result(){
            is_ok = true;
            data = new data_u();
        }

        ~result(){ delete data; }

        void set_error(const E& error_val) {

            if (data->value != nullptr) {
                delete data->value;
                data->value = nullptr;
            }

            if (data->error != nullptr) {
                delete data->error;
                data->error = nullptr;
            }

            data->error = new E;
          
            *data->error = error_val;
            is_ok = false;
        }

        void set_value(const R& value_val) {
            *data->value = value_val;
            is_ok = true;
        }

        E Error() const { return *data->error; }
        R Value() const { return *data->value; }

        operator bool() const {return is_ok;}
        R operator*() const{
            if (!is_ok) {
                throw std::runtime_error("Attempted to dereference an error result");
            }
            return *data->value;
        }

        R& operator=(const R& value) {
            
            if(data->value != nullptr) {
                delete data->value;
            }

            if (data->error != nullptr) {
                delete data->error;
                data->error = nullptr;
            }

            data->value = new R;
            *data->value = value;
            
            
            return *data->value;
        }
        
    };

    template<typename R, typename E = std::string>
    using result_t = result<R, E>;

}