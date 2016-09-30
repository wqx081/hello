#ifndef CORE_PLATFORM_MEM_H_
#define CORE_PLATFORM_MEM_H_

#include <stddef.h>

namespace core {

void* aligned_malloc(size_t size, int minimum_alignment);
void aligned_free(void* aligned_memory);

} // namespace core
#endif // CORE_PLATFORM_MEM_H_
