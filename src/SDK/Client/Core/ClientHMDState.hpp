#pragma once

#include "../Render/Matrix.hpp"
#include "../../../Utils/Utils.hpp"

class ClientHMDState {
public:
    BUILD_ACCESS(this, Matrix, lastLevelProjMatrix, GET_OFFSET("ClientHMDState::lastLevelProjMatrix"));
};
