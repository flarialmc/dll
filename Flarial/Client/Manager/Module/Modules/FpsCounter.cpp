#include "FpsCounter.h"

void FpsCounter::onRender()
{
	if(enabled){
		Vec2<float> frameStart(MC::windowSize.x * position.x, MC::windowSize.y * position.y);
		ImVec4 moduleFrame(frameStart.x, frameStart.y, frameStart.x + 100, frameStart.y + 25);

		RenderUtils::fillRect(nullptr, moduleFrame, ImColor(0, 0, 0, 70), borderRadius);

		if (showOutline) {
			RenderUtils::drawRect(nullptr, ImVec4(0, 0, 100, 25), ImColor(0, 0, 0, 150), borderRadius, 1.f);
		}

		RenderUtils::drawText(nullptr, ImVec2(0, 0), std::format("FPS: {}", MC::fps), 20.f, ImColor(textColor[0], textColor[1], textColor[2], textColor[3]));
	}
}