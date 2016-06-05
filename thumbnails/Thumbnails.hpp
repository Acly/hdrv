#pragma once

#include <Windows.h>
#include <atomic>

struct ThumbnailsDll
{
  static HINSTANCE hInstance;
  static std::atomic<long> dllReferenceCounter;
};
