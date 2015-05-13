/**
 @file SolverException.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SimException_INC
#define  SimException_INC

#include <stdexcept>
#include <string>

class SolverException : public std::runtime_error
{
  public:
    SolverException(const std::string& message);
    SolverException(const char* message);
};

#endif   /* ----- #ifndef SimException_INC  ----- */
