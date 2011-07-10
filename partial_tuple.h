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

#pragma once
#include <tuple>

template <unsigned int NumberToIgnore, typename... Args>
class PartialTuple
{
    private:
        template <unsigned int, typename...> struct TupleHead;

        template <typename... Tail>
        struct TupleHead<0, Tail...>
        {
            typedef std::tuple<> type;
        };

        template <typename Head>
        struct TupleHead<0, Head>
        {
            typedef std::tuple<> type;
        };

        template <unsigned int NumberToPrepend, typename Head, typename... Tail>
        struct TupleHead<NumberToPrepend, Head, Tail...>
        {
            private:
                template <typename ToAdd, typename Tuple> struct PrependTuple;

                template <typename ToAdd, typename... TupleArgs>
                struct PrependTuple<ToAdd, std::tuple<TupleArgs...>>
                {
                    typedef typename std::tuple<ToAdd, TupleArgs...> type;
                };

            public:
                typedef typename PrependTuple<Head, typename TupleHead<NumberToPrepend - 1, Tail...>::type>::type type;
        };

    public:
        typedef typename TupleHead<sizeof...(Args) - NumberToIgnore, Args...>::type type;
};
