#include "entityplayer.h"
#include "EntityPlayerSerializer.h"

struct NetMessageNewPlayer : public NetMessage {

	NetMessageNewPlayer() : 
		NetMessage (NetMessageType::NETMSG_NEW_PLAYER) {}

	NetMessageNewPlayer(const EntityPlayer& gamePlayer) : 
		NetMessage       (NetMessageType::NETMSG_NEW_PLAYER),
		serializedPlayer (gamePlayer) {}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = NetMessage::serialize(outBuff);

		outBuff += iBytesSerial;
		iBytesSerial += serializedPlayer.serialize(outBuff);

		return iBytesSerial;
	}

	void deserialize(char* inBuff) {
		NetMessage::deserialize(inBuff);

		inBuff += sizeof(NetMessage);
		serializedPlayer = *reinterpret_cast<EntityPlayerSerializer*>(inBuff);
	}

	EntityPlayerSerializer serializedPlayer;
};
