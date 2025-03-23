#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "\n Argument one is:" << argv[0];

    if (argc == 1) {
        std::cout << "\n One argument provided \n";
        std::cout << "\n Argument one is:" << argv[0];
    }

    if (argc == 2) {
        std::cout << "\n Two arguments provided \n";
        std::cout << "\n Argument two is:" << argv[1];
    }

    if (argc == 3) {
        std::cout << "\n Three arguments provided \n";
    }

    if (argc > 3) {
        std::cout << "\n Too many args! \n";
    }
}