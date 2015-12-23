#pragma once

#include <sstream>

using std::stringstream;

namespace NEgo {



    struct TEgoException : public std::exception
    {
        TEgoException() {}
        TEgoException(TEgoException &exc) {
            ss << exc.ss.str();
        }

        template <typename T>
        TEgoException& operator << (const T& s) {
            ss << s;
            return *this;
        }
        const char * what () const throw () {
            return ss.str().c_str();
        }
        stringstream ss;
    };

    struct TEgoInterrupt : public std::exception {
    };


    #define ENSURE(cond, str) \
        if(!(cond)) { \
            throw TEgoException() << str; \
        }\


} // namespace NEgo