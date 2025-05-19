#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

int add(int a, int b)
{
    return a + b;
}

int main()
{
    std::cout << add(5, 7) << std::endl;
}
