/**
 @file CommonDefs.h
 @brief Common definitions
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#define SATOOLS_SUCCESS 0
#define SATOOLS_FAILURE 1

//default settings for ode solver

#define DEFAULT_ATOL 0.00001
#define DEFAULT_RTOL 0.00001



//error code
//

#define ERROR_UNSUPPORTED_MODEL 1000 // simulation for the model is not supported
#define ERROR_UNSUPPORTED_FEATURE 1001 //feature has not been implemented

#define ERROR_INVALID_SIMULATION_INPUT 1100
#define ERROR_INVALID_PROCEDURE 1101

#define ERROR_RULE_WITHOUT_MATH 2000
#define ERROR_INVALID_REACTION 2001 //reaction without kinetic law or with invalid kinetic law (no math)
#define ERROR_AST_EVALUATION 2002
#define FATAL_AST_EVALUATION 2003
#define ERROR_CVODE_ERROR 2004
