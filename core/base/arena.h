#ifndef CORE_BASE_ARENA_H_
#define CORE_BASE_ARENA_H_

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "core/base/macros.h"

namespace core {

// 线程兼容: 不同的线程可以同时访问同一块arena, 不需要额外的加锁
class Arena {
 public:
  explicit Arena(const size_t block_size);
  ~Arena();

  char* Alloc(const size_t size) {
    return reinterpret_cast<char*>(GetMemory(size, 1));
  }

  void Reset();

  // 32-bits
  // static const int kDefaultAlignment = 4;
  // 64-bits
  static const int kDefaultAlignment = 8;

 protected:
  bool SatisfyAlignment(const size_t alignment);
  void MakeNewBlock(const uint32_t alignment);
  void* GetMemoryFallback(const size_t size, const int alignment);
  void* GetMemory(const size_t size, const int alignment) {
    assert(remaining_ <= block_size_);
    if (size > 0 && size < remaining_ && alignment == 1) {
      void* result = freestart_;
      freestart_ += size;
      remaining_ -= size;
      return result;
    }
    return GetMemoryFallback(size, alignment);
  }
  
  size_t remaining_;

 private:
  // 具体的实现
  struct AllocatedBlock {
    char* mem;
    size_t size;
  };

  // 分配一个至少为 block_size 的 block, alignment指明对齐
  // 
  AllocatedBlock* AllocNewBlock(const size_t block_size,
                                const uint32_t alignment);
  const size_t block_size_;
  char* freestart_;
  char* freestart_when_empty_;
  size_t blocks_alloced_;
  AllocatedBlock first_blocks_[16];
  std::vector<AllocatedBlock>* overflow_blocks_;

  void FreeBlocks();

  DISALLOW_COPY_AND_ASSIGN(Arena);
};

} // namespace core
#endif // CORE_BASE_ARENA_H_
