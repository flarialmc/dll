#pragma once

class MoveInputHandler {
	char pad_0000[0x5F];
public:
	bool forward;
	bool backward;
	bool left;
	bool right;
private:
	virtual ~MoveInputHandler(void);
public:
	virtual auto tick(class IPlayerMovementProxy*) -> void;
	virtual auto render(float) -> void;
	virtual auto setKey(int, bool) -> void;
	virtual auto clearInputState(void) -> void;
	virtual auto clearMovementState(void) -> void;
	virtual auto allowPicking(float, float) -> void;
	virtual auto setJumping(bool) -> void;
	virtual auto setAutoJumpingInWater(bool) -> void;
	virtual auto isChangeHeight(void) -> bool;
	virtual auto setSneakDown(bool) -> void;
	virtual auto isPlayerMoving(void) -> bool;
	virtual auto extractInputComponent(void) -> class MoveInputComponent;
	virtual auto applyInputComponent(class MoveInputComponent*) -> void;
	virtual auto fillInputPacket(class PlayerAuthInputPacket*) -> void;
};