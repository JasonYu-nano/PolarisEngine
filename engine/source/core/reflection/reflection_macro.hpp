#pragma once

namespace Engine
{
    #define DECLARE_METHOD(className, methodName) \
    { \
        Method& method = obj->AddMethod(Method(new MethodInvoker(&className::methodName)));

    #define DECLARE_STATIC_METHOD(className, methodName) \
    { \
        Method& method = obj->AddMethod(Method(new StaticMethodInvoker(&className::methodName)));

    #define DECLARE_END }

    #define ADD_PARAM(value) \
        method.AddParam(Param(value));

    #define DECLARE_FIELD(class, field)
}