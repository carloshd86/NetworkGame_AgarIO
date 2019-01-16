#pragma once

#include "NetMessage.h"
#include "entity.h"
#include "EntityCircleSerializer.h"

struct NetMessageNewCircle : public NetMessage {

	NetMessageNewCircle() : 
		NetMessage (NetMessageType::NETMSG_NEW_CIRCLE) {}

	NetMessageNewCircle(const EntityCircle& gameCircle) : 
		NetMessage       (NetMessageType::NETMSG_NEW_CIRCLE),
		serializerCircle (gameCircle) {}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = NetMessage::serialize(outBuff);

		outBuff += iBytesSerial;
		iBytesSerial += serializerCircle.serialize(outBuff);

		return iBytesSerial;
	}

	void deserialize(char* inBuff) {
		NetMessage::deserialize(inBuff);

		inBuff += sizeof(NetMessage);
		serializerCircle = *reinterpret_cast<EntityCircleSerializer*>(inBuff);
	}

	EntityCircleSerializer serializerCircle;
};
