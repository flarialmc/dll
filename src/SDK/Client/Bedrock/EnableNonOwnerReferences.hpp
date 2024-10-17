#pragma once

namespace Bedrock {
    class EnableNonOwnerReferences {
    public:
        struct ControlBlock {
            EnableNonOwnerReferences* ptr;
        };

        std::shared_ptr<ControlBlock> controlBlock;  // this+0x8

        // prevent constructor by default
        EnableNonOwnerReferences& operator=(EnableNonOwnerReferences const&);

        virtual ~EnableNonOwnerReferences();
    };
}