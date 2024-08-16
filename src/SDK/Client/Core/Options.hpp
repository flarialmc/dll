#pragma once

#include "../../../Client/Client.hpp"

class OptionInfo {
public:
    BUILD_ACCESS(this, std::string*, TranslateName, GET_OFFSET("OptionInfo::TranslateName"));
};

class Option {
public:
    //BUILD_ACCESS(this, OptionInfo*, OptionInformation, GET_OFFSET("Option::optionInformation")); // max value?
    BUILD_ACCESS(this, bool, value, GET_OFFSET("Option::value")); // current value?
    BUILD_ACCESS(this, bool, value1, GET_OFFSET("Option::value1")); // max value?
};

class Options {
private:
    static inline uintptr_t** optionsBaseEntry;
    static inline std::unordered_map<UINT32, Option*> options;
    static inline bool initialized = false;

    static void initVsync(){
        auto vsyncsetting = Client::settings.getSettingByName<bool>("vsync");
        auto vsyncOption = Options::getOption("vsync");

        if(vsyncOption == nullptr) return;

        if (vsyncsetting->value && vsyncOption->getvalue())
            vsyncOption->setvalue(false);
        else if (!vsyncsetting->value && !vsyncOption->getvalue())
            vsyncOption->setvalue(true);
    }
public:
    static void initialize(const uintptr_t optionsEntryPtr){
        optionsBaseEntry = (uintptr_t **) optionsEntryPtr;
        initialized = true;
        initVsync();
    };

    static bool isInitialized() {
        return initialized;
    };

    static Option* findOption(const std::string& optionName) {
        if(!isInitialized()) return nullptr;
        for (uint16_t i = 4; i < 450; i++) {

            if (optionsBaseEntry[i] == nullptr) continue;
            OptionInfo* info = *(OptionInfo**)((uintptr_t)optionsBaseEntry[i] + GET_OFFSET("Option::optionInformation"));
            if (info == nullptr) continue;

            auto* optionsPtr = (Option*)optionsBaseEntry[i];
            auto translateName = *(std::string*)((uintptr_t)info + GET_OFFSET("OptionInfo::TranslateName"));

            if(translateName.empty()) continue;

//            if (optionsBaseEntry[i] == nullptr) continue;
//
//            auto *optionsPtr = (Option *) optionsBaseEntry[i];
//
//            auto *optionData = optionsPtr->getOptionInformation();
//
//            if (optionData == nullptr) continue;
//
//            auto translateName = optionData->getTranslateName();
//
//            if (translateName == nullptr) continue;

            //Logger::debug(translateName);
            if (optionName == translateName) {
                return optionsPtr;
            }
        }
        return nullptr;
    };
    static Option* getOption(const std::string& optionName) {
        if(!isInitialized()) return nullptr;
        std::hash<std::string> optionNameHash;

        UINT32 key = optionNameHash(optionName);

        auto it = options.find(key);
        if (it != options.end()) {
            return it->second;
        } else {
            auto option = findOption("options." + optionName);
            if(option == nullptr) return option; // do not save option ptr if its null
            options[key] = option;
            return option;
        }
    }
};

