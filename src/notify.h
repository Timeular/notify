#pragma once

#include <napi.h>

class Notify : public Napi::ObjectWrap<Notify>
{
public:
    Notify(const Napi::CallbackInfo&);
    Napi::Value Show(const Napi::CallbackInfo&);
    Napi::Value Hide(const Napi::CallbackInfo& info);

    static Napi::Function GetClass(Napi::Env);
};
