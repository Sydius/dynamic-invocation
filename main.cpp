#include <iostream>
#include "invoke.h"

void foo(void)
{
    std::cout << "Hello, world!" << std::endl;
}

int bar(int x, const std::string & blah)
{
    std::cout << blah << ": " << x << std::endl;
    return x + 5;
}

#define FUNC(x) #x, x

int main(int argc, char * argv[])
{
    invoke::Invoker invoker;
    invoker.registerFunction(FUNC(foo));
    invoker.registerFunction(FUNC(bar));

    std::string fooInput = invoke::serialize(FUNC(foo));
    std::string barInput = invoke::serialize(FUNC(bar), 5, "testing");

    std::string fooOutput = invoker.invoke(fooInput);
    std::string barOutput = invoker.invoke(barInput);

    int y = invoke::deserialize(FUNC(bar), barOutput);
    std::cout << y << std::endl;
}
