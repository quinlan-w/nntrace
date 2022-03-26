#include <iostream>
#include <string>
#include "Timer.h"
#include <cmath>
 
void Function1()
{
    PROFILE_FUNCTION();
    for (int i = 0; i < 1000; i++)
    {
        std::cout << pow(i, sqrt(sqrt(i))) << " ";
    }
    std::cout << std::endl;
}
 
void Function2(int v)
{
    PROFILE_FUNCTION();
    for (int i = 0; i < 1000; i++)
    {
        std::cout << abs(i * v) << " ";
    }
    std::cout << std::endl;
}
 
namespace benchmark {
    void RunApp()
    {
        PROFILE_FUNCTION();
        std::thread a([]() {Function1(); });
        std::thread b([]() {Function2(10); });
 
        a.join();
        b.join();
    }
}
 
int main()
{
    Instrumentor::Get().BeginSession("profile");
    benchmark::RunApp();
    Instrumentor::Get().EndSession();
 
}
