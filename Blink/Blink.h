#pragma once
#include <Windows.h>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include "windivert.h"

#pragma comment(lib,"WinDivert.lib")
#pragma comment(lib,"winmm.lib")

void runBlink(int mode, int capture_delay, int release_delay);