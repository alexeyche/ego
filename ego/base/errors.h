#pragma once

#include <sstream>

using std::stringstream;

namespace NEgo {

    #define DEFINE_ERROR(type) \
        struct type : public std::exception \
        { \
            type() {} \
            type(type &exc) { \
                ss << exc.ss.str(); \
            } \
            \
            template <typename T> \
            type& operator << (const T& s) { \
                ss << s; \
                return *this; \
            } \
            \
            const char * what () const throw () { \
                return ss.str().c_str(); \
            } \
            stringstream ss; \
        };

    DEFINE_ERROR(TEgoException);
    DEFINE_ERROR(TEgoInterrupt);
    DEFINE_ERROR(TEgoNotImplemented);
    DEFINE_ERROR(TEgoFileNotFound);
    DEFINE_ERROR(TEgoElementNotFound);
    DEFINE_ERROR(TEgoLogicError);
    DEFINE_ERROR(TEgoAlgebraError);
    DEFINE_ERROR(TEgoNotAvailable);

    #define ENSURE(cond, str) \
        if(!(cond)) { \
            throw TEgoException() << str; \
        }\

    #define ENSURE_ERR(cond, exc) \
        if(!(cond)) { \
            throw exc; \
        }\


} // namespace NEgo