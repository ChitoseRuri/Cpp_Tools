#pragma once
#if defined(DEBUG)|defined(_DEBUG)
#else
#define NDBUG
#endif
#include <assert.h>
#include <cstdio>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <thread>

#pragma comment(lib, "winmm.lib")