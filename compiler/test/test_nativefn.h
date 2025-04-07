#pragma once

#include <memory>
#include <vector>
#include "interpreter/Values.h"
#include "interpreter/Environment.h"
#include "interpreter/NativeFunctions.h"

void test_nativefn_print() {
  std::vector<std::unique_ptr<Values::RuntimeVal>> args;
  args.push_back(std::make_unique<Values::NumberVal>(5));
  Environment env;
  std::unique_ptr<Values::RuntimeVal> result = NativeFunctions::print(args, &env);
  TEST_ASSERT_EQUAL(Values::ValueType::Null, result->type);
}

void test_nativefn_rnd_onearg() {
  std::vector<std::unique_ptr<Values::RuntimeVal>> args;
  args.push_back(std::make_unique<Values::NumberVal>(5));
  Environment env;
  std::unique_ptr<Values::RuntimeVal> result = NativeFunctions::rnd(args, &env);
  TEST_ASSERT_EQUAL(Values::ValueType::Number, result->type);
  TEST_ASSERT_GREATER_OR_EQUAL(0, static_cast<Values::NumberVal *>(result.get())->value);
  TEST_ASSERT_LESS_THAN(5, static_cast<Values::NumberVal *>(result.get())->value);
}

void test_nativefn_rnd_twoargs() {
  std::vector<std::unique_ptr<Values::RuntimeVal>> args;
  args.push_back(std::make_unique<Values::NumberVal>(5));
  args.push_back(std::make_unique<Values::NumberVal>(10));
  Environment env;
  std::unique_ptr<Values::RuntimeVal> result = NativeFunctions::rnd(args, &env);
  TEST_ASSERT_EQUAL(Values::ValueType::Number, result->type);
  TEST_ASSERT_GREATER_OR_EQUAL(5, static_cast<Values::NumberVal *>(result.get())->value);
  TEST_ASSERT_LESS_THAN(10, static_cast<Values::NumberVal *>(result.get())->value);
}

// other functions cannot be tested as they rely on connected devices