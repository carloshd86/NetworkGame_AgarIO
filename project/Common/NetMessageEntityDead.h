#pragma once

#include "NetMessage.h"

struct NetMessageEntityDead : public NetMessage {

	NetMessageEntityDead() : NetMessage(NetMessageType::NETMSG_ENTITY_DEAD) {}
	NetMessageEntityDead(int id) : 
		NetMessage (NetMessageType::NETMSG_ENTITY_DEAD),
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

