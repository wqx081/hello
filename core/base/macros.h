#ifndef CORE_BASE_MACROS_H_
#define CORE_BASE_MACROS_H_

// For GCC
#define ATTRIBUTE_NORETURN  __attribute__((noreturn))
#define ATTRIBUTE_NOINLINE  __attribute__((noinline))
#define ATTRIBUTE_UNUSED    __attribute__((unused))
#define ATTRIBUTE_PACKED    __attribute__((packed))
#define ATTRIBUTE_MUST_USE_RESULT __attribute__((warn_unused_result))
#define PRINTF_ATTRIBUTE(string_index, first_to_check) \
    __attribute__((__format__(__printf__, string_index, first_to_check)))
#define SCANF_ATTRIBUTE(string_index, first_to_check)   \
    __attribute__((__format__(__scanf__, string_index, first_to_check)))

#define PREDICT_FALSE(x)    (__builtin_expect(x, 0))
#define PREDICT_TRUE(x)     (__builtin_expect(!!(x), 1))

#define DISALLOW_COPY_AND_ASSIGN(TypeName)  \
    TypeName(const TypeName&) = delete;     \
    void operator=(const TypeName&) = delete

#define ARRAYSIZE(a)              \
    ((sizeof(a) / sizeof(*(a))) / \
      static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

#define FALLTHROUGH_INTENDED      \
  do {                            \
  } while (0)

#endif // CORE_BASE_MACROS_H_
