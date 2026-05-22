#pragma once

#include <GameInput.h>

#ifndef GAMEINPUT_API_VERSION
#define GAMEINPUT_API_VERSION 0
#endif

#if GAMEINPUT_API_VERSION == 3
using namespace GameInput::v3;
#else
#error Code is expecting another GameInput version than was supplied.
#endif