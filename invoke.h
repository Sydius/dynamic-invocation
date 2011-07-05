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
#include <unordered_map>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "call_with_tuple.h" // https://github.com/Sydius/call-with-tuple
#include "serialize_tuple.h" // https://github.com/Sydius/serialize-tuple

namespace invoke {

typedef std::function<void(boost::archive::text_iarchive & methodInput, boost::archive::text_oarchive & methodOutput)> Invokable;

template<typename _Signature>
class InvokerFactory;

template<typename _Res, typename... _Args>
class InvokerFactory<_Res(*)(_Args...)>
{
    public:
        typedef _Res(*Function)(_Args...);

        static Invokable createInvoker(Function func)
        {
            return [&func](boost::archive::text_iarchive & methodInput, boost::archive::text_oarchive & methodOutput) {
                
                std::tuple<typename std::decay<_Args>::type...> args;
                methodInput >> args;
                InvokerFactory<_Res(*)(_Args...)>::_invoke(func, methodOutput, args, _Result<_Res>{});
            };
        }

    private:
        template<typename _Type> struct _Result { };

        static void _invoke(Function func, boost::archive::text_oarchive & methodOutput, 
                const std::tuple<_Args...> & args, _Result<void>)
        {
            callWithTuple<void>(func, args);
        }

        template<typename _Ret>
        static void _invoke(Function func, boost::archive::text_oarchive & methodOutput, 
                const std::tuple<_Args...> & args, _Result<_Ret>)
        {
            _Ret r = callWithTuple<_Ret>(func, args);
            methodOutput << r;
        }
};

class Invoker
{
    public:
        template<typename _Func>
        void registerFunction(const std::string & name, _Func func)
        {
            _methods[name] = InvokerFactory<_Func>::createInvoker(func);
        }

        std::string invoke(const std::string & input)
        {
            std::stringstream methodStreamIn{input};
            std::stringstream methodStreamOut;
            boost::archive::text_iarchive methodInput{methodStreamIn};
            boost::archive::text_oarchive methodOutput{methodStreamOut};
            std::string name;
            methodInput >> name;
            _methods[name](methodInput, methodOutput);
            return methodStreamOut.str();
        }

    private:
        std::unordered_map<std::string, Invokable> _methods;
};



template<typename _Signature>
struct TupleFromFunc;

template<typename _Res, typename... _Args>
struct TupleFromFunc<_Res(*)(_Args...)>
{
    static std::tuple<typename std::decay<_Args>::type...> createTuple(_Args && ... args)
    {
        return std::tuple<typename std::decay<_Args>::type...>{std::forward<_Args>(args)...};
    }
};

template<typename _Func, typename... _Args>
std::string serialize(const std::string & name, _Func func, _Args && ... args)
{
    auto params = TupleFromFunc<_Func>::createTuple(std::forward<_Args>(args)...);
    std::stringstream ss;
    boost::archive::text_oarchive methodInput{ss};
    methodInput << name;
    methodInput << params;
    return ss.str();
}

template<typename _Signature>
struct FuncResult;

template<typename _Res, typename... _Args>
struct FuncResult<_Res(*)(_Args...)>
{
    typedef _Res type;
};

template<typename _Func>
auto deserialize(const std::string &, _Func func, const std::string & output) -> typename FuncResult<_Func>::type
{
    typename FuncResult<_Func>::type ret;
    std::stringstream ss{output};
    boost::archive::text_iarchive methodOutput{ss};
    methodOutput >> ret;
    return ret;
}

}
