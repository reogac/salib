add_library(sim OBJECT ASTNodeOptimizer.cpp
                       CvodeHelper.cpp
                       CvodeSettings.cpp
                       CvodeSolver.cpp
                       EventExecution.cpp
                       EventExecutionList.cpp
                       KineticParameter.cpp
                       MathPrinter.cpp
                       OdeModel.cpp
                       OdeModelEquation.cpp
                       OdeModelEventAssignment.cpp
                       OdeModelEvent.cpp
                       OdeModelVariable.cpp
                       OdeSim.cpp
                       SbmlModelParsingHelper.cpp
                       SbmlOdeModel.cpp
                       SbmlResult.cpp
                       SbmlSim.cpp
                       SimSettings.cpp
                       SolverBase.cpp
                       SolverError.cpp
                       SolverException.cpp
                       SolverHelperBuilder.cpp
                       SolverHelper.cpp
                       SolverSettings.cpp )
