/**
 * \file Logger.hpp
 * \author Andrey Popov
 * 
 * The module implements a light-weight thread-safe logging facility. The output is performed to
 * the standard output stream. The user should use a globally-available instance logger. Several
 * manipulators are supported.
 */

#pragma once

#include <iostream>
#include <mutex>


/// An embedding namespace
namespace logging{

// Forward declarations
struct _EndOfMessage;
struct _TimeStamp;


/// Manipulator to mark the end of a message
_EndOfMessage eom();


/// Manipulator to print a timestamp
_TimeStamp timestamp();


/**
 * \struct _EndOfMessage
 * \brief User is not expected to use this structure explicitly
 * 
 * An instance of this structure is returned by manipulator eom
 */
struct _EndOfMessage
{
    private:
        _EndOfMessage() = default;
    
    friend _EndOfMessage eom();
};


/**
 * \struct _TimeStamp
 * \brief User is not expected to use this structure explicitly
 * 
 * An instance of this structure is returned by manipulator timestamp
 */
struct _TimeStamp
{
    private:
        _TimeStamp() = default;
    
    friend _TimeStamp timestamp();
};


/**
 * \class Logger
 * \brief Implements a thread-safe logging facility
 */
class Logger
{
    public:
        /// Default constructor
        Logger() = default;
    
    public:
        /**
         * \brief Overload to end the current message
         * 
         * Prints a newline symbol, flushes the buffer, releases the mutex.
         */
        Logger &operator<<(_EndOfMessage (*)());
        
        /**
         * \brief Overload to print a timestamp
         * 
         * Prints the current date and time.
         */
        Logger &operator<<(_TimeStamp (*)());
        
        /// Performs the actual write
        template<typename T>
        Logger &operator<<(T const &msg);
    
    private:
        /**
         * \brief Makes sure the current thread own the mutex
         * 
         * Method lock the recursive mutex. If it finds that the mutex has already been locked by
         * the current thread, it unlock the mutex one time. Therefore, the mutex is locked two
         * times at maximum.
         */
        void VerifyLock();
    
    private:
        /// A mutex to protect output
        static std::recursive_mutex outputMutex;
        
        /**
         * \brief Indicates if the mutex is locked
         * 
         * This data member is used to determine if the mutex is locked for the second time.
         */
        static bool isLocked;
};


template<typename T>
Logger &Logger::operator<<(T const &msg)
{
    // Lock the mutex
    VerifyLock();
    
    // Print the message
    std::cout << msg;
    
    return *this;
}


/// A globally-available instance of class Logger (defined in the source file)
extern Logger logger;

}  // end of namespace logger