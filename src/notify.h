#pragma once

#include <napi.h>

class Notify : public Napi::ObjectWrap<Notify>
{
public:
    Notify(const Napi::CallbackInfo&);
    Napi::Value Show(const Napi::CallbackInfo&);

    static Napi::Function GetClass(Napi::Env);
};
