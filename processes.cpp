#include <iostream>

int main()
{
    int* p;

    std::cout << "Here's a pointer: " << p << '\n';

    int x = 123;
    int* y = &x;

    std::cout << "Here's the value of x: " << x << '\n' << "And here's the pointer y that points to x: " << y << '\n';

    int* pp = nullptr;

    std::cout << "Here's a null pointer that points to nothing: " << pp << '\n';

    // And in order to obtain value that the pointer is pointing to, we need to dereference it:

    int readfromy = *y;

    std::cout << "Reading from pointer y from previous example: " << readfromy << '\n';

    // In fact, we can use pointers to change the value of the pointed-to object:

    int a = 999;
    int* b = &a;
    *b = 123456;
    
    std::cout << "The value of a used to be 999, but now it is: " << a << '\n';
}