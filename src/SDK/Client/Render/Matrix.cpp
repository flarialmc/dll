#include "Matrix.hpp"
#include "../../SDK.hpp"


GLMatrix* Matrix::getMatrixCorrection()
{
    GLMatrix toReturn = GLMatrix();

    for (int i = 0; i < 4; i++) {
        toReturn.matrix[i * 4 + 0] = SDK::clientInstance->Matrix1.matrix[0 + i];
        toReturn.matrix[i * 4 + 1] = SDK::clientInstance->Matrix1.matrix[4 + i];
        toReturn.matrix[i * 4 + 2] = SDK::clientInstance->Matrix1.matrix[8 + i];
        toReturn.matrix[i * 4 + 3] = SDK::clientInstance->Matrix1.matrix[12 + i];
    }

    return &toReturn;
};

float Matrix::transformx(const Vec3<float>& p)
{
    auto matrix = Matrix::getMatrixCorrection()->matrix;
    return p.x * matrix[0] + p.y * matrix[4] + p.z * matrix[8] + matrix[12];

}

float Matrix::transformy(const Vec3<float>& p)
{
    auto matrix = Matrix::getMatrixCorrection()->matrix;
    return p.x * matrix[1] + p.y * matrix[5] + p.z * matrix[9] + matrix[13];
}

float Matrix::transformz(const Vec3<float>& p)
{
    auto matrix = Matrix::getMatrixCorrection()->matrix;
    return p.x * matrix[2] + p.y * matrix[6] + p.z * matrix[10] + matrix[14];
}

bool Matrix::WorldToScreen(Vec3<float> pos, Vec2<float>& screen)
{ // pos = pos 2 w2s, screen = output screen coords

    Vec2<float> displaySize = SDK::clientInstance->guiData->ScreenSize;
    LevelRender* lr = SDK::clientInstance->getLevelRender();
    Vec3<float> origin = lr->getLevelRendererPlayer()->cameraPos;
    Vec2<float> fov = SDK::clientInstance->getFov();

    pos.x -= origin.x;
    pos.y -= origin.y;
    pos.z -= origin.z;

    float x = transformx(pos);
    float y = transformy(pos);
    float z = transformz(pos);

    if (z > 0) return false;

    float mX = (float)displaySize.x / 2.0F;
    float mY = (float)displaySize.y / 2.0F;

    screen.x = mX + (mX * x / -z * fov.x);
    screen.y = mY - (mY * y / -z * fov.y);

    return true;
}