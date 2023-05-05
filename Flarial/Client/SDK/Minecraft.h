#pragma once
#include "Classes/ClientInstance.h"

class MC {
public:
	static Vec2<int16_t> windowSize;
	static Vec2<float> windowSizePercent;
	static int16_t frames;
	static int16_t fps;
	static int16_t rcps;
	static int16_t lcps;
	static inline Vec2<float> mousepos;
	static inline MouseAction mouseaction;
	static inline bool scrollUp;
	static inline bool held;
public:
	static ClientInstance* clientInstance;
	static LoopbackPacketSender* packetSender;
public:
	static ClientInstance* getClientInstance();
	static LocalPlayer* getLocalPlayer();
public:
	static MoveInputHandler* getMoveInputHandler();
public:
	static bool gameIsFullScreen();
};