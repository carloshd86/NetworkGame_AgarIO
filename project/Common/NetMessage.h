#pragma once

struct NetMessage {
	enum NetMessageType {
		NETMSG_UNKNOWN,
		NETMSG_NEW_CIRCLE,
		NETMSG_NEW_PLAYER,
		NETMSG_PLAYER_ID,
		NETMSG_ENTITY_DEAD,
		NETMSG_GAME_INITIAL_SNAPSHOT,
		NETMSG_GAME_SNAPSHOT,
		NETMSG_CHANGE_POS
	};

	NetMessage() : Type(NETMSG_UNKNOWN) {}
	NetMessage(NetMessageType Type) : Type(Type) {}

	NetMessageType Type;

	size_t serialize(char* outBuff) {
		*reinterpret_cast<NetMessageType*>(outBuff) = Type;
		return sizeof(NetMessage);
	}

	void deserialize(char* inBuff) {
		Type = *reinterpret_cast<NetMessageType*>(inBuff);
	}
};