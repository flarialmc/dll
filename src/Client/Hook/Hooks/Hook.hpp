#pragma once

#include "MinHook.h"
#include <string>
#include <format>

#include "../../../Utils/Logger/Logger.hpp"
#include "../../../Utils/Memory/Memory.hpp"

class Hook
{
public:
  std::string signature;
  std::string name;

  Hook(std::string name, std::string signature);
  virtual ~Hook();

  virtual void enableHook();

  bool autoHook(void *callback_ptr, void **func_original);
  bool manualHook(void *hook_addr, void *callback_ptr, void **func_original);

    void disableHook();
};