#pragma once

#include <memory>
#include <vector>

#include "ErrorHandler.h"
#include "Values.h"
#include "comm/PadsComm.h"

class NativeFunctions {
public:
  static std::unique_ptr<Values::RuntimeVal> print(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> rnd(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> playSound(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> playCorrectActionJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> playWrongActionJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> playWinnerJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> playLoserJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> waitForPlayerOnPad(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> waitForPlayerOnAnyPad(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> waitForPlayersOnAllActivePads(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> waitWithCancelCheck(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
  static std::unique_ptr<Values::RuntimeVal> isPadOccupied(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope);
};