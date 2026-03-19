//
// Created by Leslie on January 12, 2026.
//

#pragma once

namespace Bedrock::Safety
{
    struct RedactableString
    {
        std::string mUnredacted;
        std::optional<std::string> mRedacted;
    };
}