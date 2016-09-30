#include "core/base/arena.h"

#include "core/base/macros.h"

#include <gtest/gtest.h>

namespace core {

static void TestMem(void* mem, int size) {
  memset(mem, 0xaa, size);
  char* tmp[100];
  for (size_t i = 0; i < ARRAYSIZE(tmp); ++i) {
    tmp[i] = new char[i*i + 1];
  }
  
  memset(mem, 0xcc, size);

  for (size_t i = 0; i < ARRAYSIZE(tmp); ++i) {
    delete[] tmp[i];
  }

  memset(mem, 0xee, size);
}

TEST(Arena, BasicArena) {
  Arena a(1024);
  char* mem = a.Alloc(100);
  ASSERT_NE(mem, nullptr);
  TestMem(mem, 100);

  mem = a.Alloc(100);
  ASSERT_NE(mem, nullptr);
  TestMem(mem, 100);
}


} // namespace core

