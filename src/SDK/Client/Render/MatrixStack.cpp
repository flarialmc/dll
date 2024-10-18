#include "MatrixStack.hpp"

void MatrixStack::push() {
    this->isDirty = true;

    this->stack.push(this->stack.top());
}

void MatrixStack::pop() {
    this->isDirty = true;
    this->stack.pop();
}

Matrix& MatrixStack::top() {
    return this->stack.top();
}
