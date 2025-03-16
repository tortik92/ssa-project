#pragma once

#include <unity.h>
#include "interpreter/Environment.h"
#include "interpreter/Values.h"
#include "interpreter/NativeFunctions.h"

void test_environment_global_env() {
    Environment env;
    std::unique_ptr<Values::RuntimeVal> result = env.lookupVar("true");
    TEST_ASSERT_EQUAL(Values::ValueType::Boolean, result->type);
    TEST_ASSERT_EQUAL(true, static_cast<Values::BooleanVal*>(result.get())->value);
    result = env.lookupVar("null");
    TEST_ASSERT_EQUAL(Values::ValueType::Null, result->type);
}

void test_environment_nativefn_random() {
    Environment env;
    std::unique_ptr<Values::RuntimeVal> result = env.lookupVar("random");
    TEST_ASSERT_EQUAL(Values::ValueType::NativeFn, result->type);
    Values::FunctionCall rnd = static_cast<Values::NativeFnVal*>(result.get())->call;
    
    std::vector<std::unique_ptr<Values::RuntimeVal>> args;
    args.push_back(std::make_unique<Values::NumberVal>(0));
    args.push_back(std::make_unique<Values::NumberVal>(5));
    result = rnd(args, &env);
    TEST_ASSERT_GREATER_OR_EQUAL(0, static_cast<Values::NumberVal*>(result.get())->value);
    TEST_ASSERT_LESS_THAN(5, static_cast<Values::NumberVal*>(result.get())->value);
}

void test_environment_declare_var() {
    Environment env;
    std::unique_ptr<Values::NumberVal> val = std::make_unique<Values::NumberVal>(42);
    std::unique_ptr<Values::RuntimeVal> result = env.declareVar("x", std::move(val), false);
    TEST_ASSERT_EQUAL(42, static_cast<Values::NumberVal*>(result.get())->value);
}

void test_environment_assign_var() {
    Environment env;
    std::unique_ptr<Values::NumberVal> val = std::make_unique<Values::NumberVal>(42);
    env.declareVar("x", std::move(val), false);
    auto newVal = std::make_unique<Values::NumberVal>(100);
    std::unique_ptr<Values::RuntimeVal> result = env.assignVar("x", std::move(newVal));
    TEST_ASSERT_EQUAL(100, static_cast<Values::NumberVal*>(result.get())->value);
}

void test_environment_lookup_var() {
    Environment env;
    std::unique_ptr<Values::NumberVal> val = std::make_unique<Values::NumberVal>(42);
    env.declareVar("x", std::move(val), false);
    std::unique_ptr<Values::RuntimeVal> result = env.lookupVar("x");
    TEST_ASSERT_EQUAL(42, static_cast<Values::NumberVal*>(result.get())->value);
}

void test_environment_resolve_var() {
    Environment parentEnv;
    std::unique_ptr<Values::NumberVal> val = std::make_unique<Values::NumberVal>(42);
    parentEnv.declareVar("x", std::move(val), false);

    Environment childEnv(&parentEnv);
    std::unique_ptr<Values::RuntimeVal> result = childEnv.lookupVar("x");
    TEST_ASSERT_EQUAL(42, static_cast<Values::NumberVal*>(result.get())->value);
}

void test_environment_reassign_const_var() {
    Environment env;
    std::unique_ptr<Values::NumberVal> val = std::make_unique<Values::NumberVal>(42);
    env.declareVar("x", std::move(val), true);
    std::unique_ptr<Values::NumberVal> newVal = std::make_unique<Values::NumberVal>(100);
    // TEST_ASSERT_THROWS(env.assignVar("x", std::move(newVal)), ErrorHandler::restart);
}

void test_environment_var_not_found() {
    Environment env;
    // TEST_ASSERT_THROWS(env.lookupVar("y"), ErrorHandler::restart);
}