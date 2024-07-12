#include "Matrix.hpp"
#include "../../SDK.hpp"

glm::mat4 Matrix::getMatrixCorrection() {
    glm::mat4 toReturn;

    for (int i = 0; i < 4; i++) {
        toReturn[i][0] = SDK::clientInstance->Matrix1.matrix[0 + i];
        toReturn[i][1] = SDK::clientInstance->Matrix1.matrix[4 + i];
        toReturn[i][2] = SDK::clientInstance->Matrix1.matrix[8 + i];
        toReturn[i][3] = SDK::clientInstance->Matrix1.matrix[12 + i];
    }

    return toReturn;
}

bool Matrix::WorldToScreen(Vec3<float> pos, Vec2<float> &screen) { // pos = pos 2 w2s, screen = output screen coords

    Vec2<float> displaySize = SDK::clientInstance->guiData->ScreenSize;
    LevelRender *lr = SDK::clientInstance->getLevelRender();
    Vec3<float> origin = lr->getLevelRendererPlayer()->cameraPos;

    pos = pos.sub(origin);

    glm::mat4x4 matx = getMatrixCorrection();

    glm::vec4 transformedPos = matx * glm::vec4(pos.x, pos.y, pos.z, 1.0f);

    if (transformedPos.z > 0) return false;

    float mX = displaySize.x / 2.0f;
    float mY = displaySize.y / 2.0f;

    screen.x = mX + (mX * transformedPos.x / -transformedPos.z * SDK::clientInstance->getFovX());
    screen.y = mY - (mY * transformedPos.y / -transformedPos.z * SDK::clientInstance->getFovY());

    return true;
}