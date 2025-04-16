#pragma once

#include <memory>
#include <vector>

#include "ErrorHandler.h"
#include "Values.h"
#include "PadsComm.h"

class NativeFunctions {
public:
  /**
   * @brief Prints the value of the argument to the serial console.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments. 
   *             Expects exactly one argument.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a NullVal object.
   * @throws ErrorHandler::restart if there is not exactly one argument.
   */
  static std::unique_ptr<Values::RuntimeVal> print(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  /**
   * @brief Returns a random number between the specified range or a random number up to the specified maximum.
   * 
   * If one argument is provided, it is treated as the maximum value (exclusive).
   * 
   * If two arguments are provided, they are treated as the minimum (inclusive) and maximum (exclusive) values.
   * 
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a NumberVal object containing the random number.
   * @throws ErrorHandler::restart if required arguments are missing or not of type Number.
   */
  static std::unique_ptr<Values::RuntimeVal> rnd(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Plays a sound on a specific pad or all pads.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 2 or 3 arguments: sound value, sound length, and optionally pad index.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a NullVal object.
   * @throws ErrorHandler::restart if required arguments are missing or not of type Number.
   */
  static std::unique_ptr<Values::RuntimeVal> playSound(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Plays the "correct action" jingle on a specific pad or all pads.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 0 or 1 argument: pad index.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating success.
   * @throws ErrorHandler::restart if the argument is not of type Number.
   */
  static std::unique_ptr<Values::RuntimeVal> playCorrectActionJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Plays the "wrong action" jingle on a specific pad or all pads.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 0 or 1 argument: pad index.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating success.
   * @throws ErrorHandler::restart if the argument is not of type Number.
   */
  static std::unique_ptr<Values::RuntimeVal> playWrongActionJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Plays the "winner" jingle on a specific pad or all pads.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 0 or 1 argument: pad index.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating success.
   * @throws ErrorHandler::restart if the argument is not of type Number.
   */
  static std::unique_ptr<Values::RuntimeVal> playWinnerJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Plays the "loser" jingle on a specific pad or all pads.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 0 or 1 argument: pad index.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating success.
   * @throws ErrorHandler::restart if the argument is not of type Number.
   */
  static std::unique_ptr<Values::RuntimeVal> playLoserJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Waits for a player to step on a specific pad.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 0 or 1 argument: pad index.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating success.
   * @throws ErrorHandler::restart if the argument is not of type Number.
   */
  static std::unique_ptr<Values::RuntimeVal> waitForPlayerOnPad(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Waits for a player to step on any pad.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects no arguments.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a NumberVal object containing the index of the pad stepped on, or -1 if canceled.
   * @throws ErrorHandler::restart if any arguments are provided.
   */
  static std::unique_ptr<Values::RuntimeVal> waitForPlayerOnAnyPad(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Waits for players to step on all active pads.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects no arguments.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating success.
   * @throws ErrorHandler::restart if any arguments are provided.
   */
  static std::unique_ptr<Values::RuntimeVal> waitForPlayersOnAllActivePads(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Waits for a specified duration while checking for a cancel signal.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 1 argument: delay in milliseconds.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating whether the user canceled.
   * @throws ErrorHandler::restart if the argument is not of type Number or if the number of arguments is incorrect.
   */
  static std::unique_ptr<Values::RuntimeVal> waitWithCancelCheck(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);

  /**
   * @brief Checks if a specific pad is occupied.
   * @param args A vector of unique pointers to RuntimeVal objects representing the arguments.
   *             Expects 1 argument: pad index.
   * @param scope A pointer to the Environment object representing the current scope.
   * @return A unique pointer to a BooleanVal object indicating whether the pad is occupied.
   * @throws ErrorHandler::restart if the argument is not of type Number or if the number of arguments is incorrect.
   */
  static std::unique_ptr<Values::RuntimeVal> isPadOccupied(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
};