#pragma once

class Option {
public:
    BUILD_ACCESS(this, std::string*, translateName, 0x158);
};

class Options {
public:
    BUILD_ACCESS(this, bool, value, 0x10);
    BUILD_ACCESS(this, bool, value1, 0x70);
};