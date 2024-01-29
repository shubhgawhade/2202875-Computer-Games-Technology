#pragma once

#include <queue>
#include "TankMessage.h"

//This is a network simulation and doesn't actually do any networking!
class NetworkSimulator
{
public:
	
	//Create a Network simulator with the designated Send Rate and Latency
	NetworkSimulator(float sendRate, float latency);
	~NetworkSimulator();


	/** Receive the next message from the network. Non-blocking.
	Returns true if a message was received, false if there are no more messages to process. */
	bool ReceiveMessage(TankMessage &result);

	float Time() { return m_Time; }
	void Update(float dt);

	void SetLatency(float);
	void SetSendRate(float);

	void Reset();

	int m_MyID = -1;

private:
	float m_Time, m_SentTime;
	std::queue<TankMessage> m_MessageQueue;
	float m_SentX, m_SentY; //The last sent position
	float m_SentVX;
	float m_SentVY;
	float m_Latency = 0.1f;	
	float m_SendRate = 0.5f;
};

