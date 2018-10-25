#include "notify.h"

using namespace Napi;

#define NTDDI_VERSION NTDDI_VISTA 
#define _WIN32_WINNT _WIN32_WINNT_VISTA 

#include <codecvt>
#include <napi-thread-safe-callback.hpp>
#include <wintoastlib.h>

using namespace WinToastLib;

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

std::wstring s2ws(const std::string& s)
{
    return converter.from_bytes(s);
}

class WinToastHandlerExample : public IWinToastHandler
{
public:
    WinToastHandlerExample(std::vector<std::string> actions, std::shared_ptr<ThreadSafeCallback>&& callback) : mActions(actions), mCallback(callback) {
    }

    virtual void toastActivated() const {
        mCallback->call([](Napi::Env env, std::vector<napi_value>& args) {
            args = { Napi::String::New(env, "activated") };
        });
    }
    virtual void toastActivated(int actionIndex) const {
        std::string action = "<invalid>";
        if (actionIndex >= 0 && actionIndex < mActions.size()) {
            action = mActions[actionIndex];
        }
        mCallback->call([action](Napi::Env env, std::vector<napi_value>& args) {
            args = { Napi::String::New(env, "activated"), Napi::String::New(env, action) };
        });
    }
    virtual void toastDismissed(WinToastDismissalReason state) const {
        std::string reason = "";
        if (state == WinToastDismissalReason::ApplicationHidden) {
            reason = "hidden";
        } else if (state == WinToastDismissalReason::TimedOut) {
            reason = "timeout";
        } else if (state == WinToastDismissalReason::UserCanceled) {
            reason = "canceled";
        }
        mCallback->call([reason](Napi::Env env, std::vector<napi_value>& args) {
            args = { Napi::String::New(env, "dismissed"), Napi::String::New(env, reason) };
        });
    }
    virtual void toastFailed() const {
        mCallback->call([](Napi::Env env, std::vector<napi_value>& args) {
            args = { Napi::String::New(env, "failed") };
        });
    }
private:
    std::vector<std::string> mActions;
    std::shared_ptr<ThreadSafeCallback> mCallback;
};

Notify::Notify(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Wrong number of arguments 2 expected (appName, aumi)")
          .ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Wrong arguments expected (appName, aumi)")
          .ThrowAsJavaScriptException();
        return;
    }
    
    if (!WinToast::isCompatible()) {
        Napi::TypeError::New(env, "Error, your system in not supported!")
            .ThrowAsJavaScriptException();
        return;
    }

    auto appName = info[0].As<Napi::String>().Utf8Value();
    auto aumi = info[1].As<Napi::String>().Utf8Value();

    WinToast::instance()->setAppName(s2ws(appName));
    WinToast::instance()->setAppUserModelId(s2ws(aumi));
    WinToast::WinToastError error;
    if (!WinToast::instance()->initialize(&error)) {
        printf("\n\nerror %d\n\n", error);
        Napi::TypeError::New(env, "Error, could not initialize the lib!")
            .ThrowAsJavaScriptException();
        return;
    }
}

Napi::Value Notify::Show(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 6) {
        Napi::TypeError::New(env, "Wrong number of arguments 6 expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString() || !info[1].IsString() || !info[2].IsString() || !info[3].IsBoolean() || !info[4].IsArray() || !info[5].IsFunction()) {
        Napi::TypeError::New(env, "5 arguments expected (image, title, content, sound, actions, callback)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    auto image = info[0].As<Napi::String>().Utf8Value();
    auto title = info[1].As<Napi::String>().Utf8Value();
    auto content = info[2].As<Napi::String>().Utf8Value();
    auto soundEnabled = info[3].As<Napi::Boolean>();
    auto actionsArray = info[4].As<Napi::Array>();
    auto callback = std::make_shared<ThreadSafeCallback>(info[5].As<Napi::Function>());

    WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageAndText02);
    templ.setImagePath(s2ws(image));
    templ.setTextField(s2ws(title), WinToastTemplate::FirstLine);
    templ.setTextField(s2ws(content), WinToastTemplate::SecondLine);
    templ.setAudioOption(soundEnabled ? WinToastTemplate::AudioOption::Default : WinToastTemplate::AudioOption::Silent);

    std::vector<std::string> actions;
    for (uint32_t i = 0; i < actionsArray.Length(); i++) {
        std::string action = actionsArray.Get(i).As<Napi::String>().Utf8Value();
        templ.addAction(s2ws(action));
        actions.push_back(action);
    }
    INT64 id = WinToast::instance()->showToast(templ, new WinToastHandlerExample(actions, move(callback)));
    napi_value value;
    napi_create_int64(env, id, &value);
    return Napi::Number(env, value);
}

Napi::Value Notify::Hide(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments 1 expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "1 arguments expected (id)")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    auto id = info[0].As<Napi::Number>().Int64Value();
    WinToast::instance()->hideToast(id);
    return env.Null();
}

Napi::Function Notify::GetClass(Napi::Env env) {
    return DefineClass(env, "Notify", {
        Notify::InstanceMethod("show", &Notify::Show),
        Notify::InstanceMethod("hide", &Notify::Hide),
    });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::String name = Napi::String::New(env, "Notify");
    exports.Set(name, Notify::GetClass(env));
    return exports;
}

NODE_API_MODULE(addon, Init)
