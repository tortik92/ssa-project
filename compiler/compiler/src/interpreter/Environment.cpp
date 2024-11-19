#include "Environment.h"



Values::RuntimeVal* Environment::declareVar(const char* varName, Values::RuntimeVal* value) {
  if (this->has(varName)) {
    GlobalFunctions::restart("Cannot declare variable \"", varName, "\", as it is already defined");
  }
  this->set(varName, value);

  return value;
}

Values::RuntimeVal* Environment::assignVar(const char* varName, Values::RuntimeVal* value) {
  Environment* env = this->resolve(varName);
  env->set(varName, value);
  return value;
}

Values::RuntimeVal* Environment::lookupVar(const char* varName) {
  Environment* env = this->resolve(varName);
  return env->get(varName);
}

Environment* Environment::resolve(const char* varName) {
  if (this->has(varName)) {
    return this;
  }

  if (this->parent == nullptr) {
    GlobalFunctions::restart("Cannot resolve variable \"", varName, "\"");
  }

  return parent->resolve(varName);
}

bool Environment::has(const char* varName) {
  for (size_t i = 0; i < varIndex; i++) {
    if (strcmp(varNames[i], varName) == 0) {
      return true;
    }
  }
  return false;
}

void Environment::set(const char* varName, Values::RuntimeVal* value) {
  int index = getIndex(varName);
  bool isNotDeclared = false;
  if (index == -1) {
    index = varIndex;
    isNotDeclared = true;
  }

  if (index >= maxVariables) {
    GlobalFunctions::restart("Out of variables for scope");
  }
  Serial.print("Setting \"");
  Serial.print(varName);
  Serial.print("\" to ");

  if (isNotDeclared) {
    size_t varLen = strlen(varName);
    varNames[index] = new char[varLen + 1];
    strncpy(varNames[index], varName, varLen);
    varNames[index][varLen] = '\0';
  }

  switch (value->type) {
    case Values::ValueType::Null:
      if (isNotDeclared) {
        valuesArray[index] = values.newNullVal();
      }
      Serial.println("Null");
      break;
    case Values::ValueType::Boolean:
      {
        if (isNotDeclared) {
          valuesArray[index] = values.newBooleanVal();
        }
        Values::BooleanVal* boolVal = static_cast<Values::BooleanVal*>(value);

        static_cast<Values::BooleanVal*>(valuesArray[index])->value = boolVal->value;

        Serial.print("Boolean: ");
        Serial.println(boolVal->value);

        break;
      }
    case Values::ValueType::Number:
      {
        if (isNotDeclared) {
          valuesArray[index] = values.newNumberVal();
        }
        Values::NumberVal* numVal = static_cast<Values::NumberVal*>(value);

        static_cast<Values::NumberVal*>(valuesArray[index])->value = numVal->value;

        Serial.print("Number: ");
        Serial.println(static_cast<Values::NumberVal*>(valuesArray[index])->value);
        break;
      }
    default:
      GlobalFunctions::restart("Unknown runtime value type encountered!");
      // do not forget to edit valueTypesCount in Constants.h!
      break;
  }

  if (isNotDeclared) varIndex++;
}

Values::RuntimeVal* Environment::get(const char* varName) {
  for (size_t i = 0; i < varIndex; i++) {
    if (strcmp(varNames[i], varName) == 0) {
      return valuesArray[i];
    }
  }
  return nullptr;
}

int Environment::getIndex(const char* varName) {
  for (int i = 0; i < (int)varIndex; i++) {
    if (strcmp(varNames[i], varName) == 0) {
      return i;
    }
  }
  return -1;
}