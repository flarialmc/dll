#include "Matrix.hpp"
#include "../../SDK.hpp"
#include "../../../Client/GUI/D2D.hpp"

glm::mat4 Matrix::getMatrixCorrection(GLMatrix mat) {
    glm::mat4 toReturn;

    for (int i = 0; i < 4; i++) {
        toReturn[i][0] = mat.matrix[0 + i];
        toReturn[i][1] = mat.matrix[4 + i];
        toReturn[i][2] = mat.matrix[8 + i];
        toReturn[i][3] = mat.matrix[12 + i];
    }

    return toReturn;
}

bool Matrix::WorldToScreen(Vec3<float> pos, Vec2<float> &screen) { // pos = pos 2 w2s, screen = output screen coords

    Vec2<float> displaySize = SDK::clientInstance->guiData->ScreenSize;
    Vec3<float> origin = MC::Transform.origin;

    pos = pos.sub(origin);

    glm::mat4x4 matx = Matrix::getMatrixCorrection(MC::Transform.modelView);

    glm::vec4 transformedPos = matx * glm::vec4(pos.x, pos.y, pos.z, 1.0f);

    if (transformedPos.z > 0) return false;

    float mX = displaySize.x / 2.0f;
    float mY = displaySize.y / 2.0f;

    screen.x = mX + (mX * transformedPos.x / -transformedPos.z * MC::Transform.fov.x);
    screen.y = mY - (mY * transformedPos.y / -transformedPos.z * MC::Transform.fov.y);

    return true;
}