#include "NetworkSimulator.h"

#include <iostream>


NetworkSimulator::NetworkSimulator(float sendRate, float latency) :
	m_Time(0),
	m_SentTime(0),
	m_SentX(128),
	m_SentY(128),
	m_SentVX(15),
	m_SentVY(10),
	m_SendRate(sendRate),
	m_Latency(latency)
{
}


NetworkSimulator::~NetworkSimulator()
{
}



bool NetworkSimulator::ReceiveMessage(TankMessage & result)
{
	if (m_MessageQueue.empty()) {
		return false;
	}
	else {
		result = m_MessageQueue.front();
		m_MessageQueue.pop();
		return true;
	}
}

void NetworkSimulator::Update(float dt)
{
	m_Time += dt;

	// Simulate messages being sent from a remote host every "period",
	// delivered to this host after "latency".
	while (m_SentTime + m_Latency < m_Time) {
		m_MessageQueue.push({ 1, m_SentX, m_SentY, m_SentTime });

		m_SentTime += m_SendRate;

		// Change the path of the tank after a while.
		if (m_SentTime >= 12.77f) {
			m_SentVX = 25.0f;
			m_SentVY = -10.0f;
		}

		m_SentX += (m_SentVX * m_SendRate);
		m_SentY += (m_SentVY * m_SendRate);
	}

}

void NetworkSimulator::SetLatency(float latency)
{
	m_Latency = latency;
}

void NetworkSimulator::SetSendRate(float sendRate)
{
	m_SendRate = sendRate;
}

void NetworkSimulator::Reset() {
	m_Time = 0.0f;
	m_SentTime = 0.0f;
	m_SentX = 0.0f;
	m_SentY = 60.0f;
	m_SentVX= 30;
	m_SentVY= 20;
}