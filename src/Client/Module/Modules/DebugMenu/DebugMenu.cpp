#include "DebugMenu.hpp"
#include "Tags.hpp"
#include "Modules/Time/Time.hpp"
#include "SDK/Client/Block/BlockLegacy.hpp"

void JavaDebugMenu::onEnable() {
	Listen(this, SetupAndRenderEvent, &JavaDebugMenu::onSetupAndRender)
		Listen(this, RenderEvent, &JavaDebugMenu::onRender)
		Listen(this, TickEvent, &JavaDebugMenu::onTick)
		Listen(this, KeyEvent, &JavaDebugMenu::onKey)
		Module::onEnable();
}

void JavaDebugMenu::onDisable() {
	Deafen(this, SetupAndRenderEvent, &JavaDebugMenu::onSetupAndRender)
		Deafen(this, RenderEvent, &JavaDebugMenu::onRender)
		Deafen(this, TickEvent, &JavaDebugMenu::onTick)
		Deafen(this, KeyEvent, &JavaDebugMenu::onKey)
		Module::onDisable();
}

void JavaDebugMenu::defaultConfig() {
	getKeybind();
	Module::defaultConfig("core");

	setDef("uiscale", 0.65f);
	setDef("rounding", 0.0f);
	setDef("showBg", true);
	setDef("text", (std::string)"ffffff", 1.f, false);
	setDef("bg", (std::string)"000000", 0.5f, false);
	setDef("imPoorButIWannaLookRich", false);
	setDef("showFPS", true);
	setDef("showDim", true);
	setDef("showCoords", true);
	setDef("showServer", true);
	setDef("showMemory", true);
	setDef("showCpuGpu", true);
	setDef("showTime", true);
	setDef("showUptime", true);
	setDef("showSpeed", true);
	setDef("showTargetedBlock", true);
	setDef("showTargetedBlockTags", true);
	setDef("showAllTags", true);
	setDef("noOfTags", 10);
}

void JavaDebugMenu::settingsRender(float settingsOffset) {

	float x = Constraints::PercentageConstraint(0.019, "left");
	float y = Constraints::PercentageConstraint(0.10, "top");

	const float scrollviewWidth = Constraints::RelativeConstraint(0.5, "height", true);

	FlarialGUI::ScrollBar(x, y, 140, Constraints::SpacingConstraint(5.5, scrollviewWidth), 2);
	FlarialGUI::SetScrollView(x - settingsOffset, Constraints::PercentageConstraint(0.00, "top"),
		Constraints::RelativeConstraint(1.0, "width"),
		Constraints::RelativeConstraint(0.88f, "height"));

	addHeader("Main");
	addKeybind("Keybind", "Hold for 2 seconds!", "keybind", true);
	addSlider("UI Scale", "", "uiscale", 2.0f);
	addSlider("Rounding", "Rounding of the rectangle", "rounding", 100, 0, false);
	addToggle("Background", "", "showBg");
	extraPadding();

	addHeader("Colors");
	addColorPicker("Text Color", "", "text");
	addColorPicker("Background Color", "", "bg");
	extraPadding();

	addHeader("Module Settings");
	addToggle("I'm broke but I wanna look rich :(", "only for the real broke sigmas", "imPoorButIWannaLookRich");
	addToggle("Show FPS and Frametime", "", "showFPS");
	addToggle("Show Entity Counter and Dimension", "", "showDim");
	addToggle("Show Coordinates Section", "", "showCoords");
	addToggle("Show Server Section", "", "showServer");
	addToggle("Show Memory", "", "showMemory");
	addToggle("Show CPU, GPU and Renderer", "", "showCpuGpu");
	addToggle("Show Time", "", "showTime");
	addToggle("Show Uptime", "", "showUptime");
	addToggle("Show Speed", "", "showSpeed");
	addToggle("Show Targeted Block", "", "showTargetedBlock");
	addConditionalToggle(getOps<bool>("showTargetedBlock"), "Show Targeted Block Tags", "", "showTargetedBlockTags");
	addConditionalToggle(getOps<bool>("showTargetedBlock") && getOps<bool>("showTargetedBlockTags"), "Show All Block Tags", "", "showAllTags");
	addConditionalSlider(getOps<bool>("showTargetedBlock") && getOps<bool>("showTargetedBlockTags") && !getOps<bool>("showAllTags"), "Number of tags to show", "", "noOfTags", 20, 1, true);

	FlarialGUI::UnsetScrollView();
	resetPadding();
}

int JavaDebugMenu::GetTicks() {
	if (tickList.empty()) {
		return 0;
	}
	double currentMicros = Microtime();
	auto count = std::count_if(tickList.begin(), tickList.end(), [currentMicros](const TickData& click) {
		return (currentMicros - click.timestamp <= 1.0);
		});

	return (int)std::round(count);
}

std::string JavaDebugMenu::getFacingDirection(LocalPlayer* player) {
	std::string direction;
	if (lerpYaw >= -45 && lerpYaw < 45) direction = "South";
	else if (lerpYaw >= 45 && lerpYaw < 135) direction = "West";
	else if (lerpYaw >= 135 || lerpYaw < -135) direction = "North";
	else direction = "East";
	return std::format("Facing: {} ({:.1f} / {:.1f})", direction, lerpYaw, lerpPitch);
}

std::string JavaDebugMenu::getCPU() { // AI Slop
	int cpuInfo[4] = { -1 };
	char cpuBrand[0x40] = { 0 };
	__cpuid(cpuInfo, 0x80000002);
	memcpy(cpuBrand, cpuInfo, sizeof(cpuInfo));
	__cpuid(cpuInfo, 0x80000003);
	memcpy(cpuBrand + 16, cpuInfo, sizeof(cpuInfo));
	__cpuid(cpuInfo, 0x80000004);
	memcpy(cpuBrand + 32, cpuInfo, sizeof(cpuInfo));
	return std::string(cpuBrand);
}

std::string JavaDebugMenu::getDimensionName() {
	BlockSource* blocksrc = SDK::clientInstance->getBlockSource();
	if (!blocksrc) {
		return "Unknown dimension";
	}
	std::string dim = blocksrc->getDimension()->getName();
	if (dim == "Overworld") {
		return "minecraft:overworld";
	}
	else if (dim == "Nether") {
		return "minecraft:nether";
	}
	else if (dim == "TheEnd") {
		return "minecraft:the_end";
	}
	else {
		return dim;
	}
}

std::string JavaDebugMenu::getTime() {
	const auto now = std::time(nullptr);
	const std::tm calendarTime = localtime_xp(now);

	std::string meridiem;
	std::string seperator;

	int hour = calendarTime.tm_hour;
	int minute = calendarTime.tm_min;

	if (hour - 12 < 0)  meridiem = "AM";
	else if (hour == 0) hour = 12, meridiem = "AM";
	else if (hour == 12) hour = 12, meridiem = "PM";
	else meridiem = "PM", hour -= 12;

	seperator = minute < 10 ? ":0" : ":";

	if (hour == 24) hour = 0;

	return FlarialGUI::cached_to_string(hour) + seperator + FlarialGUI::cached_to_string(minute) + " " + meridiem;
}

std::string JavaDebugMenu::getFormattedTime(long long seconds) {
	std::string text;
	int days = seconds / 86400;
	int hours = (seconds % 86400) / 3600;
	int mins = (seconds % 3600) / 60;
	int secs = seconds % 60;
	if (days >= 1) text += std::format("{} days, ", days);
	if (hours >= 1) text += std::format("{} hours, ", hours);
	if (mins >= 1) text += std::format("{} mins, ", mins);
	text += std::format("{} secs", secs);
	return text;
}

void JavaDebugMenu::onTick(TickEvent& event) {
	if (!SDK::clientInstance->getLocalPlayer()) return;
	auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
	if (stateVectorComponent != nullptr) {
		speed = std::format("{:.2f}", stateVectorComponent->Pos.dist(PrevPos) * 20);
		PrevPos = stateVectorComponent->Pos;
	}
	TickData tick{};
	tick.timestamp = Microtime();
	tickList.insert(tickList.begin(), tick);
	if (tickList.size() >= 120) tickList.pop_back();
}

void JavaDebugMenu::onSetupAndRender(SetupAndRenderEvent& event) { // WAILA code
	if (!SDK::clientInstance->getLocalPlayer()) return;
	if (!SDK::clientInstance->getLocalPlayer()->getLevel()) return;
	if (!SDK::clientInstance->getBlockSource()) return;
	HitResult result = SDK::clientInstance->getLocalPlayer()->getLevel()->getHitResult();

	BlockPos pos = { result.blockPos.x,
					 result.blockPos.y ,
					 result.blockPos.z };
	BlockSource* blockSource = SDK::clientInstance->getBlockSource();
	try {
		BlockLegacy* block = blockSource->getBlock(pos)->getBlockLegacy();
		if (!block) return;
		try {
			lookingAt = block->getNamespace() + ":" + block->getName();

			if (lookingAt != lastLookingAt) {
				lastLookingAt = lookingAt;
				std::vector<std::string> tags = {};
				for (auto i: tagMap) {
					if (std::find(i.second.begin(), i.second.end(), block->getName()) != i.second.end()) {
						tags.emplace_back(i.first);
					}
				}
				lookingAtTags = tags;
			}

		}
		catch (const std::exception& e) { LOG_ERROR("Failed to get block name: {}", e.what()); }
	}
	catch (const std::exception& e) {
		LOG_ERROR("Failed to get block: {}", e.what());
	}
}

void JavaDebugMenu::onRender(RenderEvent& event) {
	if (this->active && SDK::clientInstance->getScreenName() == "hud_screen") {
		float textHeight = Constraints::RelativeConstraint(0.1f * getOps<float>("uiscale"));
		float textSize = Constraints::SpacingConstraint(2.0f, textHeight);
		float yPadding = Constraints::SpacingConstraint(0.025f, textHeight);
		float rounding = getOps<float>("rounding");

		D2D1_COLOR_F textColor = getColor("text");
		D2D1_COLOR_F bgColor = getColor("bg");

		LocalPlayer* player = SDK::clientInstance->getLocalPlayer();
		ActorRotationComponent* rotComponent = player->getActorRotationComponent();
		BlockPos targetPos = { 0, 0, 0 };

		if (rotComponent) {
			if (rotComponent->rot.y != 0) lerpYaw = rotComponent->rot.y;
			if (rotComponent->rot.x != 0) lerpPitch = rotComponent->rot.x;
		}

		std::vector<std::string> left;
		std::vector<std::string> right;

		if (versionName.empty()) {
			versionName = std::format("Flarial V2 Open Beta, Minecraft {}", VersionUtils::getFormattedVersion());
		}
		left.emplace_back(versionName);
		if (getOps<bool>("showFPS")) {
			if (getOps<bool>("imPoorButIWannaLookRich")) {
				left.emplace_back(std::format("{} FPS", static_cast<int>(MC::fps * 222.2)));
			}
			else {
				left.emplace_back(std::format("{} FPS", MC::fps));
				left.emplace_back(std::format("Frametime: {:.2f}ms", MC::frameTime));
			}
		}

		left.emplace_back("");

		if (getOps<bool>("showDim")) {
			left.emplace_back(std::format("E: {}", player->getLevel()->getRuntimeActorList().size()));
			left.emplace_back(getDimensionName());
			left.emplace_back("");
		}

		if (getOps<bool>("showCoords")) {
			Vec3<float> pos = *player->getPosition();
			left.emplace_back(std::format("XYZ: {:.1f} / {:.1f} / {:.1f}", pos.x, pos.y, pos.z));
			Vec3<int> blockPos(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(pos.z));
			left.emplace_back(std::format("Block: {} {} {}", blockPos.x, blockPos.y, blockPos.z));
			left.emplace_back(std::format("Chunk: {} {} {}", static_cast<int>(pos.x / 16), static_cast<int>(pos.y / 16), static_cast<int>(pos.z / 16)));
			left.emplace_back(std::format("Chunk Coordinate: {} {}", static_cast<int>(pos.x) % 16, static_cast<int>(pos.y) % 16));
			left.emplace_back(getFacingDirection(player));
			left.emplace_back("");
			HitResult target = player->getLevel()->getHitResult();
			targetPos = { target.blockPos.x, target.blockPos.y, target.blockPos.z };
			left.emplace_back(std::format("Looking at block: {} {} {}", targetPos.x, targetPos.y, targetPos.z));
			left.emplace_back("");
		}

		if (getOps<bool>("showServer")) {
			left.emplace_back("Server:");
			left.emplace_back(std::format("IP: {}", SDK::getServerIP()));
			left.emplace_back(std::format("Port: {}", SDK::getServerPort()));
			left.emplace_back(std::format("Ping: {} ms", SDK::getServerPing()));
			left.emplace_back(std::format("TPS: {}", std::to_string(GetTicks())));
		}


		right.emplace_back("64bit");
		if (getOps<bool>("showMemory")) {
			MEMORYSTATUSEX memory_status;
			memory_status.dwLength = sizeof(memory_status);
			GlobalMemoryStatusEx(&memory_status);
			int total_memory = static_cast<int>(memory_status.ullTotalPhys / 1000000);
			int free_memory = static_cast<int>(memory_status.ullAvailPhys / 1000000);
			int used_memory = total_memory - free_memory;
			if (getOps<bool>("imPoorButIWannaLookRich")) {
				int totallyRealTotalMemory = 2097152;
				right.emplace_back(std::format("Mem: {}% {}/{} MB", static_cast<int>((used_memory * 100) / totallyRealTotalMemory), used_memory, totallyRealTotalMemory));
			}
			else right.emplace_back(std::format("Mem: {}% {}/{} MB", static_cast<int>((used_memory * 100) / total_memory), used_memory, total_memory));
			right.emplace_back("");
		}

		if (getOps<bool>("showCpuGpu")) {
			if (getOps<bool>("imPoorButIWannaLookRich")) right.emplace_back("Intel 9 7900X3D ProMax Plus");
			else {
				if (cpuName.empty()) std::string cpuName = getCPU();
				right.emplace_back(std::format("CPU: {}", getCPU()));
			}

			right.emplace_back("");

			right.emplace_back(std::format("Display: {}x{}", MC::windowSize.x, MC::windowSize.y));
			right.emplace_back(std::format("Active Renderer: {}", std::array<std::string, 5>{"Couldn't initialize", "DX9", "DX10", "DX11", "DX12"}[kiero::getRenderType()]));
			if (getOps<bool>("imPoorButIWannaLookRich")) right.emplace_back("AMD GFX 5090 Ti AI Accelerated DLSS 12.0");
			else {
				if (!MC::GPU.empty()) right.emplace_back(MC::GPU);
				else right.emplace_back("Unknown GPU");
			}

			right.emplace_back("");
		}

		if (getOps<bool>("showTime")) {
			right.emplace_back(std::format("Local Time: {}", getTime()));
		}
		if (getOps<bool>("showUptime")) {
			right.emplace_back(std::format("CPU Uptime: {}", getFormattedTime(static_cast<long long>(GetTickCount64() / 1000))));
			right.emplace_back(std::format("Minecraft Uptime: {}", getFormattedTime(
				static_cast<long long>(
					std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count()
					) / 1000
			)));
			right.emplace_back("");
		}

		if (getOps<bool>("showSpeed")) {
			right.emplace_back(std::format("Speed: {} blocks/s", speed));
			right.emplace_back("");
		}

		if (getOps<bool>("showTargetedBlock")) {
			right.emplace_back("Targeted Block:");
			right.emplace_back(lookingAt);
			right.emplace_back("");
			if (getOps<bool>("showTargetedBlockTags")) {
				int maxTags = static_cast<int>(getOps<float>("noOfTags"));
				if (getOps<bool>("showAllTags") || maxTags >= lookingAtTags.size()) {
					for (const auto & i: lookingAtTags) {
						right.emplace_back('#' + i);
					}
				}
				else {
					for (int i = 0; i < maxTags; i++ ) {
						right.emplace_back('#' + lookingAtTags[i]);
					}
					right.emplace_back(std::format("{} more tags...", lookingAtTags.size() - maxTags));
				}
			}
		}


		int leftYoffset = 0.0f;
		for (const auto & i : left) {
			if (getOps<bool>("showBg") && !i.empty()) {
				float lineWidth = FlarialGUI::getFlarialTextSize(
					String::StrToWStr(i).c_str(),
					30.0f, textHeight / 3.0f,
					DWRITE_TEXT_ALIGNMENT_LEADING,
					textSize,
					DWRITE_FONT_WEIGHT_NORMAL,
					true
				).x;
				FlarialGUI::RoundedRect(
					0.0f, leftYoffset - yPadding + 0.05f,
					bgColor,
					lineWidth, textHeight / 3.0f + yPadding * 2 - 1.0f,
					rounding, rounding
				);
			}
			FlarialGUI::FlarialTextWithFont(
				0.0f, leftYoffset,
				String::StrToWStr(i).c_str(),
				30.0f, textHeight / 3.0f,
				DWRITE_TEXT_ALIGNMENT_LEADING,
				textSize,
				DWRITE_FONT_WEIGHT_NORMAL,
				textColor,
				true
			);
			leftYoffset += textHeight / 3.0f + yPadding * 2;
		}

		int rightYoffset = 0.0f;
		for (const auto & i : right) {
			if (getOps<bool>("showBg") && !i.empty()) {
				float lineWidth = FlarialGUI::getFlarialTextSize(
					String::StrToWStr(i).c_str(),
					30.0f, textHeight / 3.0f,
					DWRITE_TEXT_ALIGNMENT_TRAILING,
					textSize,
					DWRITE_FONT_WEIGHT_NORMAL,
					true
				).x;
				FlarialGUI::RoundedRect(
					MC::windowSize.x - lineWidth, rightYoffset - yPadding + 0.05f,
					bgColor,
					lineWidth, textHeight / 3.0f + yPadding * 2 - 1.0f,
					rounding, rounding
				);
			}
			FlarialGUI::FlarialTextWithFont(
				MC::windowSize.x - 30.0f, rightYoffset,
				String::StrToWStr(i).c_str(),
				30.0f, textHeight / 3.0f,
				DWRITE_TEXT_ALIGNMENT_TRAILING,
				textSize,
				DWRITE_FONT_WEIGHT_NORMAL,
				textColor,
				true
			);
			rightYoffset += textHeight / 3.0f + yPadding * 2;
		}

	}
}

void JavaDebugMenu::onKey(KeyEvent& event) {
	if (this->isKeyPartOfKeybind(event.key)) {
		if (this->isKeybind(event.keys)) {
			keybindActions[0]({});
		}
	}
}
