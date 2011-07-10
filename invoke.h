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
#include <unordered_map>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "call_with_tuple.h" // https://github.com/Sydius/call-with-tuple
#include "serialize_tuple.h" // https://github.com/Sydius/serialize-tuple
#include "partial_tuple.h"

namespace invoke {

template<typename>
class InvokerFactory;

template<typename Res, typename... Args>
class InvokerFactory<Res(*)(Args...)>
{
    public:
        typedef Res(*Function)(Args...);

        template<typename... Extra>
        static std::function<void(boost::archive::text_iarchive & methodInput, boost::archive::text_oarchive & methodOutput, Extra... extra)> createInvoker(Function func)
        {
            return [func](boost::archive::text_iarchive & methodInput, boost::archive::text_oarchive & methodOutput, Extra && ... extra)
            {
                typename PartialTuple<sizeof...(Extra), typename std::decay<Args>::type...>::type serializedArgs;
                methodInput >> serializedArgs;
                InvokerFactory<Function>::invoke(func, methodOutput, serializedArgs, Result<Res>{}, std::forward<Extra>(extra)...);
            };
        }

    private:
        template<typename Type> struct Result { };

        template<typename TupleType, typename... Extra>
        static void invoke(Function func, boost::archive::text_oarchive & methodOutput, 
                const TupleType & args, Result<void>, Extra && ... extra)
        {
            callWithTuple<void>(func, args, std::forward<Extra>(extra)...);
        }

        template<typename Ret, typename TupleType, typename... Extra>
        static void invoke(Function func, boost::archive::text_oarchive & methodOutput, 
                const TupleType & args, Result<Ret>, Extra && ... extra)
        {
            Ret r{callWithTuple<Ret>(func, args, std::forward<Extra>(extra)...)};
            methodOutput << r;
        }
};

template<typename... Extra>
class Invoker
{
    public:
        /**
         * Register a function for dynamic invocation or serialization.
         *
         * @param name  Name of the function for serialization purposes
         * @param func  The function to register
         */
        template<typename Func>
        void registerFunction(const std::string & name, Func func)
        {
            _methods[name] = InvokerFactory<Func>:: template createInvoker<Extra...>(func);
        }

        /**
         * Invokes a registered function.
         *
         * @param input     Serialized input (includes the function name to be called)
         * @param extra...  Extra paramaters to be passed to the function
         * @return          Serialized return value from the invoked function
         */
        std::string invoke(const std::string & input, Extra && ... extra)
        {
            std::stringstream methodStreamIn{input};
            std::stringstream methodStreamOut;
            boost::archive::text_iarchive methodInput{methodStreamIn};
            boost::archive::text_oarchive methodOutput{methodStreamOut};
            std::string name;
            methodInput >> name;
            _methods[name](methodInput, methodOutput, std::forward<Extra>(extra)...);
            return methodStreamOut.str();
        }

        /**
         * Serialize a function call.
         *
         * @param name      Name of the function for which to serialize
         * @param func      Actual function for which the serialization is being done (for type-safety checks)
         * @param args...   Arguments to serialize for the function call
         * @return          Serialized form of the function call
         */
        template<typename Func, typename... Args>
        std::string serialize(const std::string & name, Func func, Args && ... args)
        {
            auto params = TupleFromFunc<Func>::createTuple(std::forward<Args>(args)...);
            std::stringstream ss;
            boost::archive::text_oarchive methodInput{ss};
            methodInput << name;
            methodInput << params;
            return ss.str();
        }

        template<typename>
        struct FuncResult;

        template<typename Res, typename... Args>
        struct FuncResult<Res(*)(Args...)>
        {
            typedef Res type;
        };

        /**
         * Deserialize a result.
         *
         * @param name      Name of the function for which the deserialization is being done
         * @param func      Actual function for which the deserialization is being done (for type-safety checks)
         * @param output    The serialized result to deserialize
         * @return          The deserialized result
         */
        template<typename Func>
        auto deserialize(const std::string & name, Func func, const std::string & output) -> typename FuncResult<Func>::type
        {
            typename FuncResult<Func>::type ret;
            std::stringstream ss{output};
            boost::archive::text_iarchive methodOutput{ss};
            methodOutput >> ret;
            return ret;
        }
    private:
        template<typename>
        struct TupleFromFunc;

        template<typename Res, typename... Args>
        struct TupleFromFunc<Res(*)(Args...)>
        {
            typedef typename PartialTuple<sizeof...(Extra), typename std::decay<Args>::type...>::type ReturnedTuple;

            template<typename... PassedArgs>
            static ReturnedTuple createTuple(PassedArgs && ... args)
            {
                static_assert(sizeof...(Args) - sizeof...(Extra) == sizeof...(PassedArgs), "Wrong number of arguments for serialize.");
                return ReturnedTuple{std::forward<PassedArgs>(args)...};
            }
        };

        std::unordered_map<std::string,
            std::function<void(boost::archive::text_iarchive & methodInput, boost::archive::text_oarchive & methodOutput, Extra && ... extra)>
            > _methods;
};

}
