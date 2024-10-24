#pragma once

#include <memory>
#include <variant>
#include <string>

namespace mce {
    class ClientTexture {
    public:
        std::shared_ptr<class data> resourcePointerBlock;
    public:
        virtual ~ClientTexture() = default;

        bool operator==(const ClientTexture& other) const {
            return resourcePointerBlock == other.resourcePointerBlock;
        }

        bool operator!=(const ClientTexture& other) const {
            return resourcePointerBlock != other.resourcePointerBlock;
        }
    };
};

