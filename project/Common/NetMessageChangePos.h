#pragma once

#include "NetMessage.h"

struct NetMessageChangePos : public NetMessage {

	NetMessageChangePos() : NetMessage(NetMessageType::NETMSG_CHANGE_POS) {}
	NetMessageChangePos(int id, float targetPosX, float targetPosY) : 
		NetMessage (NetMessageType::NETMSG_CHANGE_POS),
		id         (id),
		targetPosX (targetPosX),
		targetPosY (targetPosY) {}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = NetMessage::serialize(outBuff);

		outBuff += iBytesSerial;
		*reinterpret_cast<float*>(outBuff) = targetPosX;
		iBytesSerial += sizeof(targetPosX);

		outBuff += sizeof(targetPosX);
		*reinterpret_cast<float*>(outBuff) = targetPosY;
		iBytesSerial += sizeof(targetPosY);

		outBuff += sizeof(targetPosY);
		*reinterpret_cast<int*>(outBuff) = id;
		iBytesSerial += sizeof(id);

		return iBytesSerial;
	}

	void deserialize(char* inBuff) {
		NetMessage::deserialize(inBuff);

		inBuff += sizeof(NetMessage);
		targetPosX = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(targetPosX);
		targetPosY = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(targetPosY);
		id = *reinterpret_cast<int*>(inBuff);
	}

	float targetPosX;
	float targetPosY;

	int id;
};