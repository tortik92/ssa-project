#pragma once

#include "Constants.h"

template<typename T, size_t size> class MemoryPool {
public:
  MemoryPool()
    : pool(new T[size]), index(0) {
    Serial.println("Created memory pool");
  }

  // Copy constructor
  MemoryPool(const MemoryPool& other)
    : pool(new T[size]), index(other.index) {
    std::copy(other.pool, other.pool + other.index, pool);
    Serial.println("Copied memory pool");
  }

  // Move constructor
  MemoryPool(MemoryPool&& other) noexcept
    : pool(other.pool), index(other.index) {
    other.pool = nullptr;
    other.index = 0;
    Serial.println("Moved memory pool");
  }

  // Move assignment operator
  MemoryPool& operator=(MemoryPool&& other) noexcept {
    if (this != &other) {
      delete[] pool;
      pool = other.pool;
      index = other.index;
      other.pool = nullptr;
      other.index = 0;
      Serial.println("Move assigned memory pool");
    }
    return *this;
  }

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
    Serial.println("Cleaning up memory pool");
    delete[] pool;
    pool = new T[size];
    index = 0;
  }

private:
  T* pool;
  size_t index;
};