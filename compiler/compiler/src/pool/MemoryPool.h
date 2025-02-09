#pragma once

#include "Constants.h"

template<typename T, size_t size> class MemoryPool {
public:
  MemoryPool()
    : pool(new T[size]), index(0) {}

  ~MemoryPool() {
    delete[] pool;
  }

  T* allocate() {
    if (index >= size) {
      ErrorHandler::restart("Out of memory for memory pool nodes");
    }
    return &pool[index++];
  }

  void cleanup() {
    delete[] pool;
    pool = new T[size];
    index = 0;
    Serial.println("Cleaned up pool");
  }

private:
  T* pool;
  size_t index;
};