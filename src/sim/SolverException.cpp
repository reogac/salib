/**
 @file SolverException.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "SolverException.h"
SolverException::SolverException(const std::string& message)
  : std::runtime_error(message) 
{
}

SolverException::SolverException(const char* message)
  : std::runtime_error(message)
{

}
