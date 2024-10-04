#pragma once

#include <stack>

#include "Matrix.hpp"

class MatrixStack {
public:
    std::stack<Matrix> stack;
private:
    char filling[0x10];
public:
    bool isDirty;

    void push();
    void pop();

    Matrix& top();
};
