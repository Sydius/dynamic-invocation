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

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "invoke.h"
#define FUNC(x) #x, x

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE invoke
#include <boost/test/unit_test.hpp>

void noArgumentsFunc() { }

BOOST_AUTO_TEST_CASE(no_arguments)
{
    invoke::Invoker<boost::archive::text_iarchive, boost::archive::text_oarchive> invoker;
    invoker.registerFunction(FUNC(noArgumentsFunc));

    std::stringstream serialized;
    invoker.serialize(FUNC(noArgumentsFunc), serialized);
    
    std::stringstream result;
    bool hasResult{invoker.invoke("noArgumentsFunc", serialized, result)};
    
    BOOST_CHECK_EQUAL(hasResult, false);
}

int oneArgumentFunc(int x) { return x + 5; }

BOOST_AUTO_TEST_CASE(one_argument)
{
    invoke::Invoker<boost::archive::text_iarchive, boost::archive::text_oarchive> invoker;
    invoker.registerFunction(FUNC(oneArgumentFunc));
    
    std::stringstream serialized;
    invoker.serialize(FUNC(oneArgumentFunc), serialized, 5);

    std::stringstream result;
    bool hasResult{invoker.invoke("oneArgumentFunc", serialized, result)};

    BOOST_CHECK_EQUAL(hasResult, true);
    BOOST_CHECK_EQUAL(invoker.deserialize(FUNC(oneArgumentFunc), result), 10);
}

BOOST_AUTO_TEST_CASE(extra_argument)
{
    invoke::Invoker<boost::archive::text_iarchive, boost::archive::text_oarchive, int> invoker;
    invoker.registerFunction(FUNC(oneArgumentFunc));

    std::stringstream serialized;
    invoker.serialize(FUNC(oneArgumentFunc), serialized);

    std::stringstream result;
    bool hasResult{invoker.invoke("oneArgumentFunc", serialized, result, 5)};

    BOOST_CHECK_EQUAL(hasResult, true);
    BOOST_CHECK_EQUAL(invoker.deserialize(FUNC(oneArgumentFunc), result), 10);
}
