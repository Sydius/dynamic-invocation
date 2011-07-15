/*
Copyright 2011 Christopher Allen Ogden. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY CHRISTOPHER ALLEN OGDEN ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CHRISTOPHER ALLEN OGDEN OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Christopher Allen Ogden.
*/

#include <iostream>
#include "invoke.h"

void foo(int extra)
{
    std::cout << "Hello, world!" << extra << std::endl;
}

int bar(int x, const std::string & blah, int extra)
{
    std::cout << blah << ": " << x << std::endl;
    return x + 5;
}

#define FUNC(x) #x, x

int main(int argc, char * argv[])
{
    invoke::Invoker<int> invoker;
    invoker.registerFunction(FUNC(foo));
    invoker.registerFunction(FUNC(bar));

    std::string fooInput = invoker.serialize(FUNC(foo));
    std::string barInput = invoker.serialize(FUNC(bar), 5, "testing");

    std::string fooOutput = invoker.invoke("foo", fooInput, 100);
    std::string barOutput = invoker.invoke("bar", barInput, 200);

    int y = invoker.deserialize(FUNC(bar), barOutput);
    std::cout << y << std::endl;
}
