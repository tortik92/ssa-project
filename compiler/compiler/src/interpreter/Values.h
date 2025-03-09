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

    /**
     * @brief Destructor for RuntimeVal.
     */
    virtual ~RuntimeVal() = default;

    /**
     * @brief Clone function to create a deep copy of the RuntimeVal.
     * @return A unique pointer to the cloned RuntimeVal.
     */
    virtual std::unique_ptr<RuntimeVal> clone() const = 0;
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

    /**
     * @brief Clone function to create a deep copy of the NullVal.
     * @return A unique pointer to the cloned NullVal.
     */
    std::unique_ptr<RuntimeVal> clone() const override {
      return std::make_unique<NullVal>();
    }
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

    std::unique_ptr<RuntimeVal> clone() const override {
      return std::make_unique<BooleanVal>(*this);
    }
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

    std::unique_ptr<RuntimeVal> clone() const override {
      return std::make_unique<NumberVal>(*this);
    }
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
    StringVal(const char* _str)
      : RuntimeVal(ValueType::String) {
      size_t len = strlen(_str);
      str = new char[len + 1];
      strcpy(str, _str);
      str[len] = '\0';
    }

    /**
     * @brief Copy constructor for StringVal.
     * 
     * Creates a deep copy of the given StringVal.
     * @param other The StringVal to copy from.
     */
    StringVal(const StringVal& other)
      : RuntimeVal(other.type) {
      if (other.str) {
        size_t len = strlen(other.str);
        str = new char[len + 1];
        strcpy(str, other.str);
      } else {
        str = nullptr;
      }
    }

    /**
     * @brief Move constructor for StringVal.
     * 
     * Transfers ownership of the string from the given StringVal.
     * @param other The StringVal to move from.
     */
    StringVal(StringVal&& other) noexcept
      : RuntimeVal(other.type), str(other.str) {
      other.str = nullptr;
    }

    /**
     * @brief Move assignment operator for StringVal.
     * 
     * Transfers ownership of the string from the given StringVal.
     * @param other The StringVal to move from.
     * @return A reference to this StringVal.
     */
    StringVal& operator=(StringVal&& other) noexcept {
      if (this != &other) {
        delete[] str;
        str = other.str;

        other.str = nullptr;
      }
      return *this;
    }

    ~StringVal() {
      delete[] str;
    }

    std::unique_ptr<RuntimeVal> clone() const override {
      return std::make_unique<StringVal>(*this);
    }
  } StringVal;

  typedef struct ObjectVal : RuntimeVal {
    std::map<String, std::unique_ptr<RuntimeVal>> properties;

    ObjectVal()
      : RuntimeVal(ValueType::ObjectVal) {}

    std::unique_ptr<RuntimeVal> clone() const override {
      std::unique_ptr<ObjectVal> clonedObject = std::make_unique<ObjectVal>();
      for (const auto& [key, value] : properties) {
        clonedObject->properties[key] = value->clone();
      }
      return clonedObject;
    }
  } ObjectVal;

  /**
   * @brief Function call typedef, used to define the type of a callable function.
   * 
   * The function takes a vector of unique_ptr of runtime values and an environment pointer, 
   * and returns a unique_ptr to a runtime value.
   */
  using FunctionCall = std::function<std::unique_ptr<RuntimeVal>&&(std::vector<std::unique_ptr<RuntimeVal>>& args, Environment* env)>;

  /**
   * @brief A structure representing a native function value. Inherits from RuntimeVal.
   */
  typedef struct NativeFnVal : RuntimeVal {
    FunctionCall call;  ///< The native function call handler.

    /**
     * @brief Default constructor initializing the type to NativeFn.
     */
    NativeFnVal()
      : RuntimeVal(ValueType::NativeFn) {}

    /**
     * @brief Constructor initializing the native function value with a function call handler.
     * @param _call The function call handler to initialize the native function value with.
     */
    NativeFnVal(FunctionCall _call)
      : RuntimeVal(ValueType::NativeFn), call(_call) {}
    /**
     * @brief Clone function to create a deep copy of the NativeFnVal.
     * @return A unique pointer to the cloned NativeFnVal.
     */
    std::unique_ptr<RuntimeVal> clone() const override {
      return std::make_unique<NativeFnVal>(*this);
    }
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

    std::unique_ptr<RuntimeVal> clone() const override {
      return std::make_unique<BreakVal>(*this);
    }
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