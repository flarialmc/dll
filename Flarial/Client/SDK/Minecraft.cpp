#include "Minecraft.h"

Vec2<int16_t> MC::windowSize = Vec2<int16_t>(0, 0);
Vec2<float> MC::windowSizePercent = Vec2<float>(0.f, 0.f);
int16_t MC::frames = 0;
int16_t MC::fps = 0;
int16_t MC::rcps = 0;
int16_t MC::lcps = 0;

ClientInstance* MC::clientInstance = nullptr;
LoopbackPacketSender* MC::packetSender = nullptr;

ClientInstance* MC::getClientInstance() {

	/* static uintptr_t sig = NULL;

	if (sig == NULL)
		sig = Mem::findSig("48 8B 15 ?? ?? ?? ?? 48 8B 12 48 89 16 40 84 FF 74 ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B C6 48 8B 4C 24 ?? 48 33 CC E8 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 8B 74 24 ?? 48 83 C4 ?? 5F C3 E8 ?? ?? ?? ?? 90 CC CC CC CC CC CC CC CC CC 48 89 6C 24");

	auto offset = *(int*)(sig + 3);
	auto basePtr = (uintptr_t)(uintptr_t**)(sig + offset + 7);

	return ((ClientInstance*)Mem::findMultiLvlPtr(basePtr, { 0x0, 0x48, 0x0, 0x0 }, true)); */

	return clientInstance;

};

LocalPlayer* MC::getLocalPlayer() {

	return (getClientInstance() != nullptr ? getClientInstance()->getLocalPlayer() : nullptr);

};

MoveInputHandler* MC::getMoveInputHandler() {

	return (getClientInstance() != nullptr ? getClientInstance()->getMoveInputHandler() : nullptr);

};

bool MC::gameIsFullScreen() {

	ClientInstance* instance = getClientInstance();
	GuiData* guidata = (instance != nullptr ? instance->getGuiData() : nullptr);

	if (guidata == nullptr)
		return false;

	if (guidata->uiScale < 3.f)
		return false;

	auto window = (HWND)FindWindowA(nullptr, (LPCSTR)"Minecraft");
	
	DWORD windowProcessId;
	GetWindowThreadProcessId(window, &windowProcessId);

	WINDOWPLACEMENT placement;
	GetWindowPlacement(window, &placement);

	return placement.showCmd == 1;

};
