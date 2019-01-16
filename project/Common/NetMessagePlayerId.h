#pragma once

#include "NetMessage.h"

struct NetMessagePlayerId : public NetMessage {

	NetMessagePlayerId() : NetMessage(NetMessageType::NETMSG_PLAYER_ID) {}
	NetMessagePlayerId(int id) : 
		NetMessage (NetMessageType::NETMSG_PLAYER_ID),
		id         (id) {}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = NetMessage::serialize(outBuff);

		outBuff += iBytesSerial;
		*reinterpret_cast<int*>(outBuff) = id;
		iBytesSerial += sizeof(id);

		return iBytesSerial;
	}

	void deserialize(char* inBuff) {
		NetMessage::deserialize(inBuff);

		inBuff += sizeof(NetMessage);
		id = *reinterpret_cast<int*>(inBuff);
	}

	int id;
};

