#include "DrawUtils.hpp"

#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../SDK/SDK.hpp"
#include "../../SDK/Client/Render/Matrix.hpp"

void DrawUtils::addLine(Vec2<float> start, Vec2<float> end, float lineWidth, D2D_COLOR_F color) {

    if (start.x < 0 || start.x > SDK::clientInstance->getGuiData()->ScreenSize.x || start.y < 0 ||
        start.y > SDK::clientInstance->getGuiData()->ScreenSize.y) {
        return;
    }

    if (end.x < 0 || end.x > SDK::clientInstance->getGuiData()->ScreenSize.x || end.y < 0 ||
        end.y > SDK::clientInstance->getGuiData()->ScreenSize.y) {
        return;
    }

    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), FlarialGUI::D2DColorToImColor(color), lineWidth);
}

void DrawUtils::addLine3D(Vec3<float> start, Vec3<float> end, float lineWidth, D2D_COLOR_F color) {
    Vec2<float> screenStart, screenEnd;

    if (!Matrix::WorldToScreen(start, screenStart)) return;
    if (!Matrix::WorldToScreen(end, screenEnd)) return;

    addLine(screenStart, screenEnd, lineWidth, color);
}

void DrawUtils::addBox(Vec3<float> lower, Vec3<float> upper, float lineWidth, int mode, D2D_COLOR_F color) {
    // Create an array of vertices representing the corners of the box
    Vec3<float> vertices[8];
    vertices[0] = Vec3<float>(lower.x, lower.y, lower.z);
    vertices[1] = Vec3<float>(upper.x, lower.y, lower.z);
    vertices[2] = Vec3<float>(lower.x, upper.y, lower.z);
    vertices[3] = Vec3<float>(upper.x, upper.y, lower.z);
    vertices[4] = Vec3<float>(lower.x, lower.y, upper.z);
    vertices[5] = Vec3<float>(upper.x, lower.y, upper.z);
    vertices[6] = Vec3<float>(lower.x, upper.y, upper.z);
    vertices[7] = Vec3<float>(upper.x, upper.y, upper.z);

    if (mode == 1) {
        // 3D wireframe: draw each of the 12 edges independently via addLine3D,
        // which gracefully handles individual edge projection failures
        // Bottom face edges
        addLine3D(vertices[0], vertices[1], lineWidth, color);
        addLine3D(vertices[1], vertices[5], lineWidth, color);
        addLine3D(vertices[5], vertices[4], lineWidth, color);
        addLine3D(vertices[4], vertices[0], lineWidth, color);
        // Top face edges
        addLine3D(vertices[2], vertices[3], lineWidth, color);
        addLine3D(vertices[3], vertices[7], lineWidth, color);
        addLine3D(vertices[7], vertices[6], lineWidth, color);
        addLine3D(vertices[6], vertices[2], lineWidth, color);
        // Vertical edges
        addLine3D(vertices[0], vertices[2], lineWidth, color);
        addLine3D(vertices[1], vertices[3], lineWidth, color);
        addLine3D(vertices[4], vertices[6], lineWidth, color);
        addLine3D(vertices[5], vertices[7], lineWidth, color);
    } else if (mode == 2) {
        // 2D outline mode: project vertices and draw convex hull outline
        std::vector<std::tuple<int, Vec2<float>>> screenCords;
        for (int i = 0; i < 8; i++) {
            Vec2<float> screen;
            if (Matrix::WorldToScreen(vertices[i], screen))
                screenCords.emplace_back((int)screenCords.size(), screen);
        }

        if (screenCords.size() < 2)
            return;

        // Find start vertex (leftmost)
        auto it = screenCords.begin();
        std::tuple<int, Vec2<float>> start = *it;
        it++;

        for (; it != screenCords.end(); it++) {
            auto cur = *it;
            if (std::get<1>(cur).x < std::get<1>(start).x) {
                start = cur;
            }
        }

        // Follow outer line (convex hull)
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

            for (auto cur: screenCords) {
                if (std::get<0>(current) == std::get<0>(cur))
                    continue;

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

        for (auto &indice: indices) {
            Vec2<float> curVertex = std::get<1>(screenCords[indice]);

            if (!hasLastVertex) {
                hasLastVertex = true;
                lastVertex = curVertex;
                continue;
            }

            addLine(lastVertex, curVertex, lineWidth, color);
            lastVertex = curVertex;
        }
    }
}

// TODO let choose between outline and full box mode
void DrawUtils::addEntityBox(Actor* ent, float lineWidth, D2D_COLOR_F color) {

    Vec3<float> end = ent->getRenderPositionComponent()->renderPos;

    if(lineWidth == 0) {
        auto *lp = (SDK::clientInstance ? SDK::clientInstance->getLocalPlayer() : nullptr);
        auto *lpRenderPos = (lp ? lp->getRenderPositionComponent() : nullptr);
        if (lpRenderPos) {
            lineWidth = (float) fmax(0.5f, 1 / (float) fmax(1,
                                                             (float) lpRenderPos->renderPos.dist(end)));
        } else {
            lineWidth = 1.0f;
        }
    }

    auto aabb = ent->getAABBShapeComponent()->aabb;

    DrawUtils::addBox(aabb.lower, aabb.upper, lineWidth, 1, color);
}