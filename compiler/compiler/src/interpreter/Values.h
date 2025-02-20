#pragma once

#include <functional>
#include <map>

#include "Constants.h"

class Environment;

/**
 * @brief The Values class provides the structure for different types of runtime values.
 * 
 * It contains several value types (Null, Boolean, Number, NativeFn, Break) 
 * and a typedef for a function call handler (FunctionCall).
 */
class Values {
public:

  /**
   * @brief Enum to define different types of values in the runtime.
   */
  enum class ValueType {
    Null,       ///< Represents a null value.
    Boolean,    ///< Represents a boolean value.
    Number,     ///< Represents a numeric value.
    String,     ///< Represents a string value.
    ObjectVal,  ///< Represents an object value.
    NativeFn,   ///< Represents a native function value.
    Break,      ///< Represents a break statement.
  };

  /**
   * @brief A structure representing a runtime value.
   * 
   * The base structure includes a type which specifies the kind of value.
   */
  typedef struct RuntimeVal {
    ValueType type;  ///< The type of the value.

    /**
     * @brief Default constructor initializing the type to Null.
     */
    RuntimeVal()
      : type(ValueType::Null) {}

    /**
     * @brief Constructor initializing the value with a specific type.
     * @param _type The type to initialize the value with.
     */
    RuntimeVal(ValueType _type)
      : type(_type) {}
  } RuntimeVal;

  /**
   * @brief A structure representing a null value. Inherits from RuntimeVal.
   */
  typedef struct NullVal : RuntimeVal {
    /**
     * @brief Default constructor initializing the type to Null.
     */
    NullVal()
      : RuntimeVal(ValueType::Null) {}
  } NullVal;

  /**
   * @brief A structure representing a boolean value. Inherits from RuntimeVal.
   */
  typedef struct BooleanVal : RuntimeVal {
    bool value;  ///< The boolean value.

    /**
     * @brief Default constructor initializing the value to true.
     */
    BooleanVal()
      : RuntimeVal(ValueType::Boolean), value(true) {}

    /**
     * @brief Constructor initializing the boolean value to the given value.
     * @param _value The boolean value to initialize.
     */
    BooleanVal(bool _value)
      : RuntimeVal(ValueType::Boolean), value(_value) {}
  } BooleanVal;

  /**
   * @brief A structure representing a numeric value. Inherits from RuntimeVal.
   */
  typedef struct NumberVal : RuntimeVal {
    int value;  ///< The numeric value.

    /**
     * @brief Default constructor initializing the value to 0.
     */
    NumberVal()
      : RuntimeVal(ValueType::Number), value(0) {}

    /**
     * @brief Constructor initializing the numeric value to the given value.
     * @param _value The numeric value to initialize.
     */
    NumberVal(int _value)
      : RuntimeVal(ValueType::Number), value(_value) {}
  } NumberVal;

  /**
   * @struct StringVal
   * 
   * @brief Structure representing a string value. Inherits from RuntimeVal.
   */
  typedef struct StringVal : RuntimeVal {
    char* str;

    StringVal()
      : RuntimeVal(ValueType::String), str(nullptr) {}
    StringVal(const char* _str) {
      size_t len = strlen(_str);
      str = new char[len + 1];
      strcpy(str, _str);
      str[len] = '\0';
    }

    

    ~StringVal() {
      delete[] str;
    }
  } StringVal;

  typedef struct ObjectVal : RuntimeVal {
    std::map<String, RuntimeVal> properties;

    ObjectVal()
      : RuntimeVal(ValueType::ObjectVal) {}
  } ObjectVal;

  /**
   * @brief Function call typedef, used to define the type of a callable function.
   * 
   * The function takes a vector of runtime values and an environment pointer, 
   * and returns a runtime value.
   */
  using FunctionCall = std::function<RuntimeVal(std::vector<RuntimeVal> args, Environment* env)>;

  /**
   * @brief A structure representing a native function value. Inherits from RuntimeVal.
   */
  typedef struct NativeFnVal : RuntimeVal {
    FunctionCall call;  ///< The native function call handler.

    /**
     * Default constructor initializing the type to NativeFn.
     */
    NativeFnVal()
      : RuntimeVal(ValueType::NativeFn) {}
  } NativeFnVal;

  /**
   * @brief A structure representing a break value. Inherits from RuntimeVal.
   */
  typedef struct BreakVal : RuntimeVal {
    /**
     * @brief Default constructor initializing the type to Break.
     */
    BreakVal()
      : RuntimeVal(ValueType::Break) {}
  } BreakVal;

  static String getString(ValueType valueType) {
    return valueTypeString.at(valueType);
  }

private:
  static inline const std::map<ValueType, String> valueTypeString = {
    { ValueType::Null, "Null" },
    { ValueType::Boolean, "Boolean" },
    { ValueType::Number, "Number" },
    { ValueType::String, "String" },
    { ValueType::ObjectVal, "ObjectVal" },
    { ValueType::NativeFn, "NativeFn" },
    { ValueType::Break, "Break" }
  };
};