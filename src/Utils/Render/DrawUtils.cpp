#include "DrawUtils.hpp"

#include "../../SDK/SDK.hpp"
#include "../../SDK/Client/Render/Matrix.hpp"

MinecraftUIRenderContext* renderCtx;
GuiData* guiData;
ScreenContext* screenContext;
Tessellator* tessellator;
Matrix viewMatrix;
Matrix projMatrix;
MCCColor* colorHolder;
Vec2<float> screenSize;
Vec3<float> origin;

static MaterialPtr* uiMaterial = nullptr;

void DrawUtils::setCtx(MinecraftUIRenderContext* ctx, GuiData* gui) {
	guiData = gui;
	renderCtx = ctx;
	screenContext = renderCtx->screenContext;
	tessellator = screenContext->getTessellator();
	colorHolder = screenContext->getColorHolder();
	viewMatrix = SDK::clientInstance->camera.viewMatrix.correct();
	projMatrix = SDK::clientInstance->getProjectionMatrix();
	screenSize.x = gui->ScreenSizeScaled.x;
	screenSize.y = gui->ScreenSizeScaled.y;

	if (SDK::clientInstance->getLevelRender() != nullptr)
		origin = SDK::clientInstance->getLevelRender()->getOrigin();

	if (uiMaterial == nullptr)
		uiMaterial = MaterialPtr::createMaterial(HashedString("ui_fill_color"));
}

void DrawUtils::setGameRenderContext(class ScreenContext* ctx) {
	screenContext = ctx;

	if (SDK::clientInstance->getLevelRender() != nullptr)
		origin = SDK::clientInstance->getLevelRender()->getOrigin();
}

void DrawUtils::setColor(float r, float g, float b, float a) {
	if (colorHolder != nullptr) {
		colorHolder->r = r;
		colorHolder->g = g;
		colorHolder->b = b;
		colorHolder->a = a;
		colorHolder->shouldDelete = true;
	}
}

Tessellator* DrawUtils::getTessellator() {
	return tessellator;
}

ScreenContext* DrawUtils::getScreenContext() {
	return screenContext;
}

MaterialPtr* DrawUtils::getUIMaterial() {
	return uiMaterial;
}

void DrawUtils::addLine(const Vec2<float>& start, const Vec2<float>& end, float lineWidth, MCCColor color) {
	tessellator->color(color.r, color.g, color.b, color.a);

	float modX = 0.f;
	float modY = 0.f;
	float len = 0.f;

#define tessLine(startVert, endVert)								\
	modX = -(startVert.y - endVert.y);								\
	modY = startVert.x - endVert.x;									\
	len = sqrtf(modX * modX + modY * modY);							\
	modX /= len;													\
	modY /= len;													\
	modX *= lineWidth;												\
	modY *= lineWidth;												\
	tessellator->vertex(startVert.x + modX, startVert.y + modY, 0); \
	tessellator->vertex(startVert.x - modX, startVert.y - modY, 0); \
	tessellator->vertex(endVert.x - modX, endVert.y - modY, 0);		\
	tessellator->vertex(startVert.x + modX, startVert.y + modY, 0); \
	tessellator->vertex(endVert.x + modX, endVert.y + modY, 0);		\
	tessellator->vertex(endVert.x - modX, endVert.y - modY, 0);

	tessLine(start, end);
	tessLine(end, start);
#undef tessLine
}

void DrawUtils::addBox(const Vec3<float>& lower, const Vec3<float>& upper, float lineWidth, int mode, MCCColor color) {
	// Calculate the dimensions of the box
	Vec3<float> diff = Vec3<float>(upper.x - lower.x, upper.y - lower.y, upper.z - lower.z);

	// Create an array of vertices representing the corners of the box
	Vec3<float> vertices[8];
	vertices[0] = Vec3<float>(lower.x, lower.y, lower.z);
	vertices[1] = Vec3<float>(lower.x + diff.x, lower.y, lower.z);
	vertices[2] = Vec3<float>(lower.x, lower.y + diff.y, lower.z);
	vertices[3] = Vec3<float>(lower.x + diff.x, lower.y + diff.y, lower.z);
	vertices[4] = Vec3<float>(lower.x, lower.y, lower.z + diff.z);
	vertices[5] = Vec3<float>(lower.x + diff.x, lower.y, lower.z + diff.z);
	vertices[6] = Vec3<float>(lower.x, lower.y + diff.y, lower.z + diff.z);
	vertices[7] = Vec3<float>(lower.x + diff.x, lower.y + diff.y, lower.z + diff.z);

	if (mode == 1 || mode == 2) {
		// Convert the vertices to screen coordinates
		std::vector<std::tuple<int, Vec2<float>>> screenCords;

		for (int i = 0; i < 8; i++) {
			Vec2<float> screen;

			if (viewMatrix.OWorldToScreen(origin, vertices[i], screen, projMatrix, screenSize))
				screenCords.emplace_back(mode == 2 ? (int)screenCords.size() : i, screen);
		}

		// Return if there are less than two points to draw lines between
		if (screenCords.size() < 2)
			return;

		switch (mode) {
		case 1: {
			// Draw lines between all pairs of vertices
			for (auto it = screenCords.begin(); it != screenCords.end(); it++) {
				auto from = *it;
				auto fromOrig = vertices[std::get<0>(from)];

				for (auto to : screenCords) {
					auto toOrig = vertices[std::get<0>(to)];

					// Determine if the line should be drawn based on the relative positions of the vertices
					bool shouldDraw = false;
					// X direction
					shouldDraw |= fromOrig.y == toOrig.y && fromOrig.z == toOrig.z && fromOrig.x < toOrig.x;
					// Y direction
					shouldDraw |= fromOrig.x == toOrig.x && fromOrig.z == toOrig.z && fromOrig.y < toOrig.y;
					// Z direction
					shouldDraw |= fromOrig.x == toOrig.x && fromOrig.y == toOrig.y && fromOrig.z < toOrig.z;

					if (shouldDraw)
						DrawUtils::addLine(std::get<1>(from), std::get<1>(to), lineWidth, color);
				}
			}
			return;
			break;
		}
		case 2: {
			// Find start vertex
			auto it = screenCords.begin();
			std::tuple<int, Vec2<float>> start = *it;
			it++;

			for (; it != screenCords.end(); it++) {
				auto cur = *it;
				if (std::get<1>(cur).x < std::get<1>(start).x) {
					start = cur;
				}
			}

			// Follow outer line
			std::vector<int> indices;

			auto current = start;
			indices.push_back(std::get<0>(current));
			Vec2<float> lastDir(0, -1);

			static constexpr float PI = 3.1415927f;

			do {
				float smallestAngle = PI * 2;
				Vec2<float> smallestDir;
				std::tuple<int, Vec2<float>> smallestE;
				auto lastDirAtan2 = atan2(lastDir.y, lastDir.x);

				for (auto cur : screenCords) {
					if (std::get<0>(current) == std::get<0>(cur))
						continue;

					// angle between vecs
					Vec2<float> dir = Vec2<float>(std::get<1>(cur)).sub(std::get<1>(current));
					float angle = atan2(dir.y, dir.x) - lastDirAtan2;

					if (angle > PI)
						angle -= 2 * PI;
					else if (angle <= -PI)
						angle += 2 * PI;

					if (angle >= 0 && angle < smallestAngle) {
						smallestAngle = angle;
						smallestDir = dir;
						smallestE = cur;
					}
				}

				indices.push_back(std::get<0>(smallestE));
				lastDir = smallestDir;
				current = smallestE;
			} while (std::get<0>(current) != std::get<0>(start) && indices.size() < 8);

			Vec2<float> lastVertex;
			bool hasLastVertex = false;

			for (auto& indice : indices) {
				Vec2<float> curVertex = std::get<1>(screenCords[indice]);

				if (!hasLastVertex) {
					hasLastVertex = true;
					lastVertex = curVertex;
					continue;
				}

				addLine(lastVertex, curVertex, lineWidth, color);
				lastVertex = curVertex;
			}
			return;
		}
		}
	}
}

void DrawUtils::addEntityBox(Actor* ent, float lineWidth, MCCColor color) {
	Vec3<float> end = ent->getRenderPositionComponent()->renderPos;
	AABB render;

	if (ent->isPlayer()) {
		render = AABB(end, ent->aabb->size.x, ent->aabb->size.y, ent->aabb->size.y);
		render.upper.y += 0.2f;
		render.lower.y += 0.2f;
	}
	else
		render = AABB(end, ent->aabb->size.x, ent->aabb->size.y, 0);

	render.upper.y += 0.1f;

	float LineWidth = (float)fmax(0.5f, 1 / (float)fmax(1, (float)SDK::clientInstance->getLocalPlayer()->getRenderPositionComponent()->renderPos.dist(end)));
	DrawUtils::addBox(render.lower, render.upper, lineWidth == 0 ? LineWidth : lineWidth, 1, color);
}
