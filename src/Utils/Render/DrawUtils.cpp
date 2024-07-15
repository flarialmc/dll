#include <iostream>
#include "DrawUtils.hpp"

#include "../../Client/GUI/Engine/Engine.hpp"
#include "../../SDK/SDK.hpp"

GuiData *guiData;
Matrix viewMatrix;
Matrix projMatrix;
MCCColor *colorHolder;
Vec2<float> screenSize;
Vec3<float> origin;

void DrawUtils::setCtx(MinecraftUIRenderContext *ctx, GuiData *gui) {
    guiData = gui;
    viewMatrix = SDK::clientInstance->camera.viewMatrix.correct();
    projMatrix = SDK::clientInstance->getProjectionMatrix();
    screenSize.x = gui->ScreenSizeScaled.x;
    screenSize.y = gui->ScreenSizeScaled.y;

    if (SDK::clientInstance->getLevelRender() != nullptr)
        origin = SDK::clientInstance->getLevelRender()->getOrigin();
}

void DrawUtils::addLine(Vec2<float> start, Vec2<float> end, float lineWidth, D2D_COLOR_F color) {

    if (start.x < 0 || start.x > SDK::clientInstance->guiData->ScreenSize.x || start.y < 0 ||
        start.y > SDK::clientInstance->guiData->ScreenSize.y) {
        return;
    }

    if (end.x < 0 || end.x > SDK::clientInstance->guiData->ScreenSize.x || end.y < 0 ||
        end.y > SDK::clientInstance->guiData->ScreenSize.y) {
        return;
    }

    D2D::context->DrawLine(D2D1::Point2F(start.x, start.y), D2D1::Point2F(end.x, end.y), FlarialGUI::getBrush(color).get(),
                           lineWidth);

}

void DrawUtils::addBox(Vec3<float> lower, Vec3<float> upper, float lineWidth, int mode, D2D_COLOR_F color) {
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

    /*
    Vec3<float> start;
    Vec3<float> end;
    viewMatrix.WorldToScreen(Vec3<float>(0, -60, 0), start);
    viewMatrix.WorldToScreen(Vec3<float>(15, -60, 0), end);
    std::cout << start.x << std::endl;
    D2D::context->DrawLine(D2D1::Point2F(start.x, start.y), D2D1::Point2F(end.x, end.y), FlarialGUI::getBrush(color), lineWidth);
    */

    if (mode == 1 || mode == 2) {
        // Convert the vertices to screen coordinates
        std::vector<std::tuple<int, Vec2<float>>> screenCords;
        bool real = false;
        for (int i = 0; i < 8; i++) {
            Vec2<float> screen;

            if (Matrix::WorldToScreen(vertices[i], screen))
                screenCords.emplace_back(mode == 2 ? (int) screenCords.size() : i, screen);
            else real = true;
        }

        if (real) return;
        // Return if there are less than two points to draw lines between
        if (screenCords.size() < 2)
            return;

        switch (mode) {
            case 1: {
                // Draw lines between all pairs of vertices
                for (auto from : screenCords) {
                    auto fromOrig = vertices[std::get<0>(from)];
                }
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

                    for (auto cur: screenCords) {
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
    }
}

// TODO let choose between outline and full box mode
void DrawUtils::addEntityBox(Actor* ent, float lineWidth, D2D_COLOR_F color) {

    Vec3<float> end = ent->getRenderPositionComponent()->renderPos;

    if(lineWidth == 0)
        lineWidth = (float) fmax(0.5f, 1 / (float) fmax(1,
                                                         (float) SDK::clientInstance->getLocalPlayer()->getRenderPositionComponent()->renderPos.dist(
                                                                 end)));

    auto aabb = ent->getaabb()->aabb;

    DrawUtils::addBox(aabb.lower, aabb.upper, lineWidth, 1, color);
}