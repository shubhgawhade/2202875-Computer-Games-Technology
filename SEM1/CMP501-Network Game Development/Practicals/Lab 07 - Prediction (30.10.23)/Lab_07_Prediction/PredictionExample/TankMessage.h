#pragma once
/** A position update message. */
struct TankMessage {
	/** The object number of this tank within the game world. */
	int id;

	/** The coordinates of this tank within the game world. */
	float x, y;

	/** The time at which this message was sent.
	(Not the time at which it was received!) */
	float time;
};