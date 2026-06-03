**Forbidden features**

Taking example from chromium banned features:
Modern C++ use in Chromium:

- std::shared_ptr/std::weak_ptr 
- std::regex 
- std::bind 
- std::function 
- std::any 
- std::list/std::forward_list 
- new/delete (please use unique_ptr) or malloc/free 
- c++20 modules 
- parallel STL with execution policies 
- mutable except for std::mutex as private member of a thread-safe class 
- const_cast 
- C-style cast (aka (int)not_int_var) (use static-cast, reinterpret_cast when it makes sense) Google C++ Style Guide 
- std::lerp (the reason why: Compiler Explorer - C++ )
- std::vector<bool>
- coroutine 
- RTTI and dynamic_cast (https://google.github.io/styleguide/cppguide.html#Casting )
- using namespace (especially in header~~~~)