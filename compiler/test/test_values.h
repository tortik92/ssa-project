#pragma once

#include <unity.h>
#include <memory>
#include <map>
#include <vector>
#include "interpreter/Values.h"

void test_values_null_constructor() {
  Values::NullVal nullVal;
  TEST_ASSERT_EQUAL(Values::ValueType::Null, nullVal.type);
}

void test_values_boolean_default_constructor() {
  Values::BooleanVal boolVal;
  TEST_ASSERT_EQUAL(Values::ValueType::Boolean, boolVal.type);
  TEST_ASSERT_TRUE(boolVal.value);
}

void test_values_boolean_parameterized_constructor() {
  Values::BooleanVal boolVal(false);
  TEST_ASSERT_EQUAL(Values::ValueType::Boolean, boolVal.type);
  TEST_ASSERT_FALSE(boolVal.value);
}

void test_values_number_default_constructor() {
  Values::NumberVal numVal;
  TEST_ASSERT_EQUAL(Values::ValueType::Number, numVal.type);
  TEST_ASSERT_EQUAL(0, numVal.value);
}

void test_values_number_parameterized_constructor() {
  Values::NumberVal numVal(42);
  TEST_ASSERT_EQUAL(Values::ValueType::Number, numVal.type);
  TEST_ASSERT_EQUAL(42, numVal.value);
}

void test_values_string_default_constructor() {
  Values::StringVal strVal;
  TEST_ASSERT_EQUAL(Values::ValueType::String, strVal.type);
  TEST_ASSERT_NULL(strVal.str);
}

void test_values_string_parameterized_constructor() {
  Values::StringVal strVal("Hello");
  TEST_ASSERT_EQUAL(Values::ValueType::String, strVal.type);
  TEST_ASSERT_NOT_NULL(strVal.str);
  TEST_ASSERT_EQUAL_STRING("Hello", strVal.str);
}

void test_values_string_copy_constructor() {
  Values::StringVal strVal1("Hello");
  Values::StringVal strVal2(strVal1);
  TEST_ASSERT_EQUAL(Values::ValueType::String, strVal2.type);
  TEST_ASSERT_NOT_NULL(strVal2.str);
  TEST_ASSERT_EQUAL_STRING("Hello", strVal2.str);
}

void test_values_string_move_constructor() {
  Values::StringVal strVal1("Hello");
  Values::StringVal strVal2(std::move(strVal1));
  TEST_ASSERT_EQUAL(Values::ValueType::String, strVal2.type);
  TEST_ASSERT_NOT_NULL(strVal2.str);
  TEST_ASSERT_EQUAL_STRING("Hello", strVal2.str);
  TEST_ASSERT_NULL(strVal1.str);
}

void test_values_object_default_constructor() {
  Values::ObjectVal objVal;
  TEST_ASSERT_EQUAL(Values::ValueType::ObjectVal, objVal.type);
  TEST_ASSERT_TRUE(objVal.properties.empty());
}

void test_values_object_insertion() {
  Values::ObjectVal obj;
  obj.properties["num"] = std::make_unique<Values::NumberVal>(5);
  obj.properties["str"] = std::make_unique<Values::StringVal>("hello");

  TEST_ASSERT_EQUAL(Values::ValueType::Number, obj.properties["num"]->type);
  TEST_ASSERT_EQUAL(Values::ValueType::String, obj.properties["str"]->type);

  TEST_ASSERT_EQUAL(2, obj.properties.size());
  TEST_ASSERT_EQUAL(5, static_cast<Values::NumberVal *>(obj.properties["num"].get())->value);
  TEST_ASSERT_EQUAL_STRING("hello", static_cast<Values::StringVal *>(obj.properties["str"].get())->str);
}

void test_values_object_move_constructor() {
  Values::ObjectVal obj;
  obj.properties["num"] = std::make_unique<Values::NumberVal>(5);
  obj.properties["str"] = std::make_unique<Values::StringVal>("hello");
  TEST_ASSERT_EQUAL(5, static_cast<Values::NumberVal *>(obj.properties.at("num").get())->value);
  TEST_ASSERT_EQUAL_STRING("hello", static_cast<Values::StringVal *>(obj.properties.at("str").get())->str);
  TEST_ASSERT_EQUAL(2, obj.properties.size());

  Values::ObjectVal obj2(std::move(obj));
  TEST_ASSERT_EQUAL(5, static_cast<Values::NumberVal *>(obj2.properties["num"].get())->value);
  TEST_ASSERT_EQUAL_STRING("hello", static_cast<Values::StringVal *>(obj2.properties["str"].get())->str);
  TEST_ASSERT_EQUAL(2, obj2.properties.size());
}

void test_values_object_clone() {
  Values::ObjectVal obj;
  obj.properties["num"] = std::make_unique<Values::NumberVal>(5);
  obj.properties["str"] = std::make_unique<Values::StringVal>("hello");

  std::unique_ptr<Values::RuntimeVal> clonedObj = obj.clone();
  Values::ObjectVal* clonedObjPtr = static_cast<Values::ObjectVal* >(clonedObj.get());
  TEST_ASSERT_EQUAL(5, static_cast<Values::NumberVal *>(clonedObjPtr->properties["num"].get())->value);
  TEST_ASSERT_EQUAL_STRING("hello", static_cast<Values::StringVal *>(clonedObjPtr->properties["str"].get())->str);
  TEST_ASSERT_EQUAL(2, clonedObjPtr->properties.size());
}