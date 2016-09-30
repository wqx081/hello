#include "core/base/arena.h"

#include "core/base/macros.h"
#include "core/base/logging.h"
#include "core/platform/mem.h"

namespace core {

Arena::Arena(const size_t block_size)
    : remaining_(0),
      block_size_(block_size),
      freestart_(nullptr),
      blocks_alloced_(1),
      overflow_blocks_(nullptr) {
  DCHECK(block_size_ > kDefaultAlignment);
  
  first_blocks_[0].mem = reinterpret_cast<char*>(malloc(block_size_));
  first_blocks_[0].size = block_size_;

  Reset();
}

Arena::~Arena() {
  FreeBlocks();
  DCHECK(overflow_blocks_ == nullptr);
  for (size_t i = 0; i < blocks_alloced_; ++i) {
    free(first_blocks_[i].mem);
  }
}

// 清除所用正在使用的内存
void Arena::Reset() {
  FreeBlocks();
  freestart_ = first_blocks_[0].mem;
  remaining_ = first_blocks_[0].size;

  CHECK(SatisfyAlignment(kDefaultAlignment));

  freestart_when_empty_ = freestart_;
}

/////////////////////////////////////////////

 
bool Arena::SatisfyAlignment(size_t alignment) {
  const size_t overage = reinterpret_cast<size_t>(freestart_) & (alignment - 1);
  if (overage > 0) {
    const size_t waste = alignment - overage;
    if (waste >= remaining_) {
      return false;
    }
    freestart_ += waste;
    remaining_ -= waste;
  }

  DCHECK_EQ(size_t{0}, reinterpret_cast<size_t>(freestart_) & (alignment - 1));

  return true;
}

void Arena::MakeNewBlock(const uint32_t alignment) {
  AllocatedBlock* block = AllocNewBlock(block_size_, alignment);
  freestart_ = block->mem;
  remaining_ = block->size;
  CHECK(SatisfyAlignment(alignment));
}

static uint32_t GCD(uint32_t x, uint32_t y) {
  while (y != 0) {
    uint32_t r = x % y;
    x = y;
    y = r;
  }
  return x;
}

static uint32_t LeastCommonMultiple(uint32_t a, uint32_t b) {
  if (a > b) {
    return (a / GCD(a, b)) * b;
  } else if (a < b) {
    return (b / GCD(b, a)) * a;
  } else {
    return a;
  }
}

Arena::AllocatedBlock* Arena::AllocNewBlock(const size_t block_size,
                                             const uint32_t alignment) {
  AllocatedBlock* block;
  if (blocks_alloced_ < ARRAYSIZE(first_blocks_)) {
    block = &first_blocks_[blocks_alloced_++];
  } else {  // oops, out of space, move to the vector
    if (overflow_blocks_ == NULL)
      overflow_blocks_ = new std::vector<AllocatedBlock>;
    overflow_blocks_->resize(overflow_blocks_->size() + 1);
    block = &overflow_blocks_->back();
  }
  
  const uint32_t adjusted_alignment =
        (alignment > 1 ? LeastCommonMultiple(alignment, kDefaultAlignment) : 1);
        
  CHECK_LE(adjusted_alignment, static_cast<uint32_t>(1 << 20))
        << "Alignment on boundaries greater than 1MB not supported.";
  size_t adjusted_block_size = block_size;
  if (adjusted_alignment > 1) {
    if (adjusted_block_size > adjusted_alignment) {
      const uint32_t excess = adjusted_block_size % adjusted_alignment;
      adjusted_block_size += (excess > 0 ? adjusted_alignment - excess : 0);
    } 
    block->mem = reinterpret_cast<char*>(
          aligned_malloc(adjusted_block_size, adjusted_alignment));
  } else {
    block->mem = reinterpret_cast<char*>(malloc(adjusted_block_size));
  } 
  block->size = adjusted_block_size;
  CHECK(NULL != block->mem) << "block_size=" << block_size
                            << " adjusted_block_size=" << adjusted_block_size
                            << " alignment=" << alignment 
                            << " adjusted_alignment=" << adjusted_alignment;
                              
  return block;
} 

void* Arena::GetMemoryFallback(const size_t size, const int alignment) {
  if (0 == size) {
    return NULL;  // stl/stl_alloc.h says this is okay
  }
  
  CHECK(alignment > 0 && 0 == (alignment & (alignment - 1)));
  
  if (block_size_ == 0 || size > block_size_ / 4) {
    return AllocNewBlock(size, alignment)->mem;
  } 
    
  if (!SatisfyAlignment(alignment) || size > remaining_) {
    MakeNewBlock(alignment);
  } 
  CHECK_LE(size, remaining_);
    
  remaining_ -= size;
  void* result = freestart_;
  freestart_ += size;
    
  return result;
} 

void Arena::FreeBlocks() {
  for (size_t i = 1; i < blocks_alloced_; ++i) {  // keep first block alloced
    free(first_blocks_[i].mem);
    first_blocks_[i].mem = NULL;
    first_blocks_[i].size = 0;
  } 

  blocks_alloced_ = 1;
  if (overflow_blocks_ != NULL) {
    std::vector<AllocatedBlock>::iterator it;
    for (it = overflow_blocks_->begin(); it != overflow_blocks_->end(); ++it) {
      free(it->mem);
    } 
    delete overflow_blocks_;  // These should be used very rarely
    overflow_blocks_ = NULL;  
  } 
} 

} // namespace core
