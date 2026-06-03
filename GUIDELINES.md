[STD] Standard C++

1. Write C++23 standard code. If writing platform/compiler-specific code, make sure to put it around #if/#endif (for example clang-specific warnings, Intel intrinsics SIMD code, Nintendo Switch specific code). On MSVC, it means adding this compilation flag: /permissive-

2. Treat warnings as compilation error. When coming from a 3rd-party library (SDL, fmt, etc…), put the include around warning disabling macro and only import the library through this header. For example a specific sdl_include.h like this:

```cpp
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
#include <SDL3/SDL.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
```
3. All classes/functions should be in a namespace except for C-style function.

4. C-style function should be put in this scope

```cpp
#ifdef __cplusplus
extern "C"
{
#endif
void your_c_function();
#ifdef __cplusplus
}
#endif
```

[ERR] Error handling
1. For exceptionnal behavior (things that should not happen), use exceptions.

2. For expected errors, use std::expected https://en.cppreference.com/w/cpp/utility/expected.html .

3. Do not use std::optional for error handling. Only use it when a function could return a value.

[INT] Integer types

1. prefer using cstdint types instead of basic types except for int (https://google.github.io/styleguide/cppguide.html#Casting ):
- uint64_t rather than unsigned long
- int16_t rather than short
- int rather than int32_t
- uint32_t instead of unsigned

2. prefer using signed type for size with std::ssize():
```
for(int64_t i = 0; i < std::ssize(vector); i++) or
for(auto i = std::ssize(vector); i >= 0; i--)
```

3. use by default int

4. do not use unsigned types to signal that an integer is not negative

[FLOAT] Floating number type
1. use float, not double or long double.

[HEAD] Header
1. Do not use pragma once, use header guard: https://google.github.io/styleguide/cppguide.html#The__define_Guard

2. Do not use using namespace in a header especially not using namespace std;

3. Do not implement functions/methods in the header file except if they are trivial (constructor for example) or template.

[API] API definition
1. Do not implement Singleton class, if a class is supposed to be a system then it should be a namespace with free functions, except when the class needs to inherit from an interface, but then define it in the source file, not the header file.

2. Make sure that name link correctly in plain English: begin/end, init/deinit, create/destroy

3. Function/method names should start with a verb except for getter according to https://google.github.io/styleguide/cppguide.html#Function_Names

[SMEL] Code smell
1. Do not use using namespace at all

2. Do not define global variables! When defining constant, please use static constexpr. From [API] 1, put variables in anonymous namespace in a source file. Singletons are global variables, so not allowed.

3. Do not define constant as macro: #define WIDTH 1280 