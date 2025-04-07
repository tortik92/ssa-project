#pragma once

#include <map>
#include <set>
#include <memory>

#include "Constants.h"
#include "ErrorHandler.h"
#include "Values.h"
#include "NativeFunctions.h"
#include "PadsComm.h"

/**
 * @class Environment
 * @brief Represents an environment for managing variables, functions, and scope resolution.
 * 
 * It supports variable declarations, assignments, lookups, and handles parent-child environment resolution.
 */
class Environment {
public:
  /**
   * @brief Default constructor. Initializes an environment with no parent and creates global variables.
   */
  Environment()
    : parent(nullptr) {
    createGlobalEnv();
  }

  /**
   * @brief Constructor with parent environment.
   * 
   * @param _parent The parent environment this instance is resolving from.
   */
  Environment(Environment* _parent)
    : parent(_parent) {
  }

  /**
   * @brief Declares a variable with a given name, value, and constant flag.
   * 
   * @param varName The name of the variable.
   * @param value The value of the variable.
   * @param constant Boolean flag indicating whether the variable is constant.
   * @return The declared variable's value.
   * @throws ErrorHandler::restart if the variable already exists in the current environment.
   */
  std::unique_ptr<Values::RuntimeVal> declareVar(const char* varName, std::unique_ptr<Values::RuntimeVal>&& value, bool constant);

  /**
   * @brief Assigns a value to an existing variable in the environment.
   * 
   * @param varName The name of the variable.
   * @param value The new value to assign to the variable.
   * @return The assigned value.
   * @throws ErrorHandler::restart if attempting to reassign a constant variable.
   */
  std::unique_ptr<Values::RuntimeVal> assignVar(const char* varName, std::unique_ptr<Values::RuntimeVal>&& value);

  /**
   * @brief Looks up the value of a variable.
   * 
   * @param varName The name of the variable.
   * @return The value of the variable.
   * @throws ErrorHandler::restart if the variable cannot be resolved in the current or parent environments.
   */
  std::unique_ptr<Values::RuntimeVal> lookupVar(const char* varName);

  /**
   * @brief Resolves a variable from the current environment or any parent environment.
   * 
   * @param varName The name of the variable.
   * @return The environment in which the variable is found.
   * @throws ErrorHandler::restart if the variable cannot be resolved in any environment.
   */
  Environment* resolve(const char* varName);

  Values values; /**< The container for values in the environment. */
private:
  /**
   * @brief  Creates global environment variables and functions.
   * 
   * Initializes the built-in variables like 'true', 'false', and 'null',
   * and declares the native functions like 'print', 'playSound', etc.
   */
  void createGlobalEnv();

  Environment* parent;             /**< The parent environment for resolution of variables. */

  std::map<String, std::unique_ptr<Values::RuntimeVal>> variables; /**< Map of variables in the environment. */
  std::set<String> constants;                                      /**< Set of constant variables that cannot be reassigned. */
};
