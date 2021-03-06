#include <napi.h>

#define CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS
#include <CUESDK.h>

Napi::Number corsairGetDeviceCount(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  auto count = CorsairGetDeviceCount();
  return Napi::Number::New(env, count);
}

Napi::Object corsairPerformProtocolHandshake(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  auto details = CorsairPerformProtocolHandshake();
  auto result = Napi::Object::New(env);
  result["sdkVersion"] = Napi::String::New(env, details.sdkVersion);
  if (details.serverVersion)
  {
    result["serverVersion"] = Napi::String::New(env, details.serverVersion);
  }
  else
  {
    // CE_ServerNotFound
    result["serverVersion"] = env.Null();
  }

  result["sdkProtocolVersion"] = details.sdkProtocolVersion;
  result["serverProtocolVersion"] = details.serverProtocolVersion;
  result["breakingChanges"] = details.breakingChanges;
  return result;
}

Napi::Number corsairGetLastError(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  auto err = CorsairGetLastError();
  return Napi::Number::New(env, err);
}

Napi::Value corsairGetDeviceInfo(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto deviceIndex = info[0].As<Napi::Number>().Int32Value();
  auto deviceInfo = CorsairGetDeviceInfo(deviceIndex);
  if (!deviceInfo) {
    return env.Undefined();
  }

  auto result = Napi::Object::New(env);
  auto channels = Napi::Object::New(env);

  result["type"] = (int)deviceInfo->type;
  result["model"] = std::string(deviceInfo->model);
  result["physicalLayout"] = (int)deviceInfo->physicalLayout;
  result["logicalLayout"] = (int)deviceInfo->logicalLayout;
  result["capsMask"] = deviceInfo->capsMask;
  result["ledsCount"] = deviceInfo->ledsCount;
  result["channels"] = channels;

  channels["channelsCount"] = deviceInfo->channels.channelsCount;
  if (deviceInfo->channels.channelsCount > 0) {
    auto channelInfos = Napi::Array::New(env);
    channels["channels"] = channelInfos;
    for (int i = 0; i < deviceInfo->channels.channelsCount; i++) {
      auto ci = deviceInfo->channels.channels[i];
      auto channelInfo = Napi::Object::New(env);
      channelInfo["totalLedsCount"] = ci.totalLedsCount;
      channelInfo["devicesCount"] = ci.devicesCount;
      auto channelDeviceInfos = Napi::Array::New(env);
      channelInfo["devices"] = channelDeviceInfos;
      for (int di = 0; di < ci.devicesCount; di++) {
        auto cdi = Napi::Object::New(env);
        cdi["type"] = (int)ci.devices[di].type;
        cdi["deviceLedCount"] = ci.devices[di].deviceLedCount;
        channelDeviceInfos[(uint32_t)di] = cdi;
      }

      channelInfos[(uint32_t)i] = channelInfo;
    }
  }

  return result;
}

Napi::Boolean corsairGetLedsColors(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  auto colors = info[0].As<Napi::Array>();
  auto len = colors.Length();
  CorsairLedColor *ledsColors = new CorsairLedColor[len]();
  for (size_t i = 0; i < len; i++)
  {
    auto c = colors.Get(i).As<Napi::Object>();
    CorsairLedColor led = {
        static_cast<CorsairLedId>(c.Get("ledId").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("r").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("g").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("b").As<Napi::Number>().Int32Value())};
    ledsColors[i] = led;
  }
  const auto result = CorsairGetLedsColors(len, ledsColors);
  if (result)
  {
    for (size_t i = 0; i < len; i++)
    {
      CorsairLedColor led = ledsColors[i];
      auto c = colors.Get(i).As<Napi::Object>();
      c["ledId"] = (int)led.ledId;
      c["r"] = led.r;
      c["g"] = led.g;
      c["b"] = led.b;
    }
  }

  delete [] ledsColors;

  return Napi::Boolean::New(env, result);
}

Napi::Boolean corsairSetLedsColors(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  auto colors = info[0].As<Napi::Array>();
  auto len = colors.Length();
  CorsairLedColor *ledsColors = new CorsairLedColor[len]();
  for (size_t i = 0; i < len; i++)
  {
    auto c = colors.Get(i).As<Napi::Object>();
    CorsairLedColor led = {
        static_cast<CorsairLedId>(c.Get("ledId").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("r").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("g").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("b").As<Napi::Number>().Int32Value())};
    ledsColors[i] = led;
  }

  const auto result = CorsairSetLedsColors(len, ledsColors);

  delete [] ledsColors;

  return Napi::Boolean::New(env, result);
}

Napi::Boolean corsairGetLedsColorsByDeviceIndex(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto deviceIndex = info[0].As<Napi::Number>().Int32Value();
  auto colors = info[1].As<Napi::Array>();
  auto len = colors.Length();
  CorsairLedColor *ledsColors = new CorsairLedColor[len]();
  for (size_t i = 0; i < len; i++)
  {
    auto c = colors.Get(i).As<Napi::Object>();
    CorsairLedColor led = {
        static_cast<CorsairLedId>(c.Get("ledId").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("r").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("g").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("b").As<Napi::Number>().Int32Value())};
    ledsColors[i] = led;
  }
  const auto result = CorsairGetLedsColorsByDeviceIndex(deviceIndex, len, ledsColors);
  if (result)
  {
    for (size_t i = 0; i < len; i++)
    {
      CorsairLedColor led = ledsColors[i];
      auto c = colors.Get(i).As<Napi::Object>();
      c["ledId"] = (int)led.ledId;
      c["r"] = led.r;
      c["g"] = led.g;
      c["b"] = led.b;
    }
  }

  delete [] ledsColors;

  return Napi::Boolean::New(env, result);
}

Napi::Boolean corsairSetLedsColorsBufferByDeviceIndex(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto deviceIndex = info[0].As<Napi::Number>().Int32Value();
  auto colors = info[1].As<Napi::Array>();
  auto len = colors.Length();
  CorsairLedColor *ledsColors = new CorsairLedColor[len]();
  for (size_t i = 0; i < len; i++)
  {
    auto c = colors.Get(i).As<Napi::Object>();
    CorsairLedColor led = {
        static_cast<CorsairLedId>(c.Get("ledId").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("r").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("g").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("b").As<Napi::Number>().Int32Value())};
    ledsColors[i] = led;
  }

  const auto result = CorsairSetLedsColorsBufferByDeviceIndex(deviceIndex, len, ledsColors);

  delete [] ledsColors;

  return Napi::Boolean::New(env, result);
}

Napi::Boolean corsairSetLedsColorsFlushBuffer(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const bool result = CorsairSetLedsColorsFlushBuffer();
  return Napi::Boolean::New(env, result);
}

Napi::ThreadSafeFunction tsfnCorsairSetLedsColorsFlushBufferAsync;
Napi::Boolean corsairSetLedsColorsFlushBufferAsync(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();

  if (info.Length() < 1 || !info[0].IsFunction()) {
    const bool result = CorsairSetLedsColorsFlushBufferAsync(nullptr, nullptr);
    return Napi::Boolean::New(env, result);
  }

  const auto jsCallback = info[0].As<Napi::Function>();
  tsfnCorsairSetLedsColorsFlushBufferAsync = Napi::ThreadSafeFunction::New(env, jsCallback, "CorsairSetLedsColorsFlushBufferAsync", 0, 1);
  auto cCallback = [](void *context, bool res, CorsairError err) {
    auto callback = [](Napi::Env env, Napi::Function f, int *value) {
      f.Call({Napi::Number::New(env, *value)});
      delete value;
    };
    int *value = new int(err);
    tsfnCorsairSetLedsColorsFlushBufferAsync.BlockingCall(value, callback);
    tsfnCorsairSetLedsColorsFlushBufferAsync.Release();
  };

  const bool result = CorsairSetLedsColorsFlushBufferAsync(cCallback, nullptr);
  if (!result)
  {
    tsfnCorsairSetLedsColorsFlushBufferAsync.Release();
  }

  return Napi::Boolean::New(env, result);
}

Napi::ThreadSafeFunction tsfnCorsairSetLedsColorsAsync;
Napi::Boolean corsairSetLedsColorsAsync(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();

  auto colors = info[0].As<Napi::Array>();
  auto len = colors.Length();
  CorsairLedColor *ledsColors = new CorsairLedColor[len]();
  for (size_t i = 0; i < len; i++)
  {
    auto c = colors.Get(i).As<Napi::Object>();
    CorsairLedColor led = {
        static_cast<CorsairLedId>(c.Get("ledId").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("r").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("g").As<Napi::Number>().Int32Value()),
        static_cast<int>(c.Get("b").As<Napi::Number>().Int32Value())};
    ledsColors[i] = led;
  }

  if (info.Length() < 2 || !info[1].IsFunction()) {
    const bool result = CorsairSetLedsColorsAsync(len, ledsColors, nullptr, nullptr);
    return Napi::Boolean::New(env, result);
  }

  const auto jsCallback = info[1].As<Napi::Function>();
  tsfnCorsairSetLedsColorsAsync = Napi::ThreadSafeFunction::New(env, jsCallback, "CorsairSetLedsColorsAsync", 0, 1);
  auto cCallback = [](void *context, bool res, CorsairError err) {
    auto callback = [](Napi::Env env, Napi::Function f, int *value) {
      f.Call({Napi::Number::New(env, *value)});
      delete value;
    };
    int *value = new int(err);
    tsfnCorsairSetLedsColorsAsync.BlockingCall(value, callback);
    tsfnCorsairSetLedsColorsAsync.Release();
  };

  const bool result = CorsairSetLedsColorsAsync(len, ledsColors, cCallback, nullptr);
  if (!result)
  {
    tsfnCorsairSetLedsColorsAsync.Release();
  }

  delete [] ledsColors;

  return Napi::Boolean::New(env, result);
}

Napi::Array corsairGetLedPositions(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto result = CorsairGetLedPositions();
  if (result == nullptr) {
    return Napi::Array::New(env);
  }

  auto arr = Napi::Array::New(env, result->numberOfLed);
  for (int i = 0; i < result->numberOfLed; ++i)
  {
    auto elem = Napi::Object::New(env);
    auto pos = result->pLedPosition[i];
    elem["ledId"] = (int)pos.ledId;
    elem["top"] = pos.top;
    elem["left"] = pos.left;
    elem["height"] = pos.height;
    elem["width"] = pos.width;
    arr[(uint32_t)i] = elem;
  }

  return arr;
}

Napi::Array corsairGetLedPositionsByDeviceIndex(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto deviceIndex = info[0].As<Napi::Number>().Int32Value();
  const auto result = CorsairGetLedPositionsByDeviceIndex(deviceIndex);
  if (result == nullptr) {
    return Napi::Array::New(env);
  }

  auto arr = Napi::Array::New(env, result->numberOfLed);
  for (int i = 0; i < result->numberOfLed; ++i)
  {
    auto elem = Napi::Object::New(env);
    auto pos = result->pLedPosition[i];
    elem["ledId"] = (int)pos.ledId;
    elem["top"] = pos.top;
    elem["left"] = pos.left;
    elem["height"] = pos.height;
    elem["width"] = pos.width;
    arr[(uint32_t)i] = elem;
  }

  return arr;
}

Napi::Number corsairGetLedIdForKeyName(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto keyName = info[0].As<Napi::String>().Utf8Value()[0];
  const auto result = CorsairGetLedIdForKeyName(keyName);
  return Napi::Number::New(env, result);
}

Napi::Boolean corsairRequestControl(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const bool result = CorsairRequestControl(CorsairAccessMode::CAM_ExclusiveLightingControl);
  return Napi::Boolean::New(env, result);
}

Napi::Boolean corsairReleaseControl(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const bool result = CorsairReleaseControl(CorsairAccessMode::CAM_ExclusiveLightingControl);
  return Napi::Boolean::New(env, result);
}

Napi::Boolean corsairSetLayerPriority(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto priority = info[0].As<Napi::Number>().Int32Value();
  const bool result = CorsairSetLayerPriority(priority);
  return Napi::Boolean::New(env, result);
}

Napi::Value corsairGetDeviceProperty(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const auto deviceIndex = info[0].As<Napi::Number>().Int32Value();
  const auto propertyId = info[1].As<Napi::Number>().Int32Value();
  if ((propertyId & CorsairDevicePropertyType::CDPT_Boolean) != 0) {
    bool value;
    bool success = CorsairGetBoolPropertyValue(deviceIndex, static_cast<CorsairDevicePropertyId>(propertyId), &value);
    if (success) {
      auto property = Napi::Object::New(env);
      property["value"] = value;
      return property;
    }
  } else if ((propertyId & CorsairDevicePropertyType::CDPT_Int32) != 0) {
    int value;
    bool success = CorsairGetInt32PropertyValue(deviceIndex, static_cast<CorsairDevicePropertyId>(propertyId), &value);
    if (success) {
      auto property = Napi::Object::New(env);
      property["value"] = value;
      return property;
    }
  }

  return env.Undefined();
}

Napi::ThreadSafeFunction *tsfnCorsairSubscribeForEvents;
Napi::Boolean corsairSubscribeForEvents(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();

  if (info.Length() < 1 || !info[0].IsFunction())
  {
    const bool result = CorsairSubscribeForEvents(nullptr, nullptr);
    return Napi::Boolean::New(env, result);
  }

  const auto jsCallback = info[0].As<Napi::Function>();

  if (tsfnCorsairSubscribeForEvents != nullptr)
  {
    tsfnCorsairSubscribeForEvents->Abort();
    tsfnCorsairSubscribeForEvents = nullptr;
  }
  
  tsfnCorsairSubscribeForEvents = new Napi::ThreadSafeFunction();
  *tsfnCorsairSubscribeForEvents = Napi::ThreadSafeFunction::New(env, jsCallback, "CorsairSubscribeForEvents", 0, 1);

  auto cCallback = [](void *context, const CorsairEvent *event) {
    auto callback = [](Napi::Env env, Napi::Function f, CorsairEvent *value) {
      auto evt = Napi::Object::New(env);
      if (value->id == CEI_KeyEvent)
      {
        evt["id"] = (value->keyEvent->isPressed ? std::string("macrokeydown") : std::string("macrokeyup"));
        evt["deviceId"] = std::string(value->keyEvent->deviceId);
        evt["keyId"] = (int)value->keyEvent->keyId;
      }
      else if (value->id == CEI_DeviceConnectionStatusChangedEvent)
      {
        evt["id"] = (value->deviceConnectionStatusChangedEvent->isConnected ? std::string("deviceconnect") : std::string("devicedisconnect"));
        evt["deviceId"] = std::string(value->deviceConnectionStatusChangedEvent->deviceId);
      }
      else
      {
        evt["id"] = env.Null();
      }
      delete value;
      f.Call({evt});
    };

    CorsairEvent* ev = new CorsairEvent();
    ev->id = event->id;
    if (ev->id == CEI_KeyEvent)
    {
      auto ke = new CorsairKeyEvent();
      ke->keyId = event->keyEvent->keyId;
      ke->isPressed = event->keyEvent->isPressed;
      strncpy(ke->deviceId, event->keyEvent->deviceId, CORSAIR_DEVICE_ID_MAX);
      ev->keyEvent = ke;
    }
    else if (ev->id == CEI_DeviceConnectionStatusChangedEvent)
    {
      auto de = new CorsairDeviceConnectionStatusChangedEvent();
      de->isConnected = event->deviceConnectionStatusChangedEvent->isConnected;
      strncpy(de->deviceId, event->deviceConnectionStatusChangedEvent->deviceId, CORSAIR_DEVICE_ID_MAX);
      ev->deviceConnectionStatusChangedEvent = de;
    }

    tsfnCorsairSubscribeForEvents->BlockingCall(ev, callback);
  };

  const bool result = CorsairSubscribeForEvents(cCallback, nullptr);
  if (!result)
  {
    tsfnCorsairSubscribeForEvents->Release();
    tsfnCorsairSubscribeForEvents = nullptr;
  }

  return Napi::Boolean::New(env, result);
}

Napi::Boolean corsairUnsubscribeFromEvents(const Napi::CallbackInfo &info)
{
  const auto env = info.Env();
  const bool result = CorsairUnsubscribeFromEvents();
  if (tsfnCorsairSubscribeForEvents != nullptr) {
    tsfnCorsairSubscribeForEvents->Abort();
    tsfnCorsairSubscribeForEvents = nullptr;
  }

  return Napi::Boolean::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports["CorsairGetDeviceCount"] = Napi::Function::New(env, corsairGetDeviceCount);
  exports["CorsairPerformProtocolHandshake"] = Napi::Function::New(env, corsairPerformProtocolHandshake);
  exports["CorsairGetLastError"] = Napi::Function::New(env, corsairGetLastError);

  exports["CorsairGetDeviceInfo"] = Napi::Function::New(env, corsairGetDeviceInfo);

  exports["CorsairGetLedsColors"] = Napi::Function::New(env, corsairGetLedsColors);
  exports["CorsairSetLedsColors"] = Napi::Function::New(env, corsairSetLedsColors);
  exports["CorsairGetLedsColorsByDeviceIndex"] = Napi::Function::New(env, corsairGetLedsColorsByDeviceIndex);
  exports["CorsairSetLedsColorsBufferByDeviceIndex"] = Napi::Function::New(env, corsairSetLedsColorsBufferByDeviceIndex);
  exports["CorsairSetLedsColorsFlushBuffer"] = Napi::Function::New(env, corsairSetLedsColorsFlushBuffer);
  exports["CorsairSetLedsColorsFlushBufferAsync"] = Napi::Function::New(env, corsairSetLedsColorsFlushBufferAsync);
  exports["CorsairSetLedsColorsAsync"] = Napi::Function::New(env, corsairSetLedsColorsAsync);
  exports["CorsairGetLedPositions"] = Napi::Function::New(env, corsairGetLedPositions);
  exports["CorsairGetLedPositionsByDeviceIndex"] = Napi::Function::New(env, corsairGetLedPositionsByDeviceIndex);
  exports["CorsairGetLedIdForKeyName"] = Napi::Function::New(env, corsairGetLedIdForKeyName);

  exports["CorsairRequestControl"] = Napi::Function::New(env, corsairRequestControl);
  exports["CorsairReleaseControl"] = Napi::Function::New(env, corsairReleaseControl);
  exports["CorsairSetLayerPriority"] = Napi::Function::New(env, corsairSetLayerPriority);
  //  exports["CorsairRegisterKeypressCallback"] = Napi::Function::New(env, corsairRegisterKeypressCallback);

  //  exports["CorsairGetBoolPropertyValue"] = Napi::Function::New(env, corsairGetBoolPropertyValue);
  //  exports["CorsairGetInt32PropertyValue"] = Napi::Function::New(env, corsairGetInt32PropertyValue);
  exports["CorsairGetDeviceProperty"] = Napi::Function::New(env, corsairGetDeviceProperty);

  exports["CorsairSubscribeForEvents"] = Napi::Function::New(env, corsairSubscribeForEvents);
  exports["CorsairUnsubscribeFromEvents"] = Napi::Function::New(env, corsairUnsubscribeFromEvents);

  return exports;
}

NODE_API_MODULE(cue_sdk, Init)