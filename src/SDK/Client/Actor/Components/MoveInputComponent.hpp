struct MoveInputComponent {

    BUILD_ACCESS(this, bool, forward, 0x0A);
    BUILD_ACCESS(this, bool, backward, 0x0B);
    BUILD_ACCESS(this, bool, left, 0x0C);
    BUILD_ACCESS(this, bool, right, 0x0D);

    BUILD_ACCESS(this, bool, sneaking, 0x20);
    BUILD_ACCESS(this, bool, jumping, 0x26);
    BUILD_ACCESS(this, bool, sprinting, 0x27);

};