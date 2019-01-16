#pragma once

#include "NetMessage.h"
#include <vector>
#include <map>
#include "EntitySerializer.h"

struct NetMessageGameSnapshot : public NetMessage {

	NetMessageGameSnapshot() : 
		NetMessage  (NetMessageType::NETMSG_GAME_SNAPSHOT),
		numEntities (0) {}

	NetMessageGameSnapshot(const std::vector<Entity*>& gameEntities) : 
		NetMessage  (NetMessageType::NETMSG_GAME_SNAPSHOT),
		inEntities  (gameEntities),
		numEntities (gameEntities.size()) {}

	NetMessageGameSnapshot(const std::map<int, Entity*>& gameEntities) : 
		NetMessage  (NetMessageType::NETMSG_GAME_SNAPSHOT),
		numEntities (gameEntities.size()) {

		for (auto& pair : gameEntities) {
			inEntities.push_back(pair.second);
		}
	}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = NetMessage::serialize(outBuff);

		outBuff += iBytesSerial;
		*reinterpret_cast<int*>(outBuff) = numEntities;
		iBytesSerial += sizeof(numEntities);

		outBuff += sizeof(numEntities);

		for (auto entity : inEntities) {
			*reinterpret_cast<Entity::E_Type*>(outBuff) = entity->getType();
			iBytesSerial += sizeof(Entity::E_Type);
			outBuff += sizeof(Entity::E_Type);
		}

		for (auto entity : inEntities) {
			size_t serializationSize =  entity->serialize(outBuff);
			iBytesSerial += serializationSize;
			outBuff += serializationSize;
		}

		return iBytesSerial;
	}

	void deserialize(char* inBuff) {
		NetMessage::deserialize(inBuff);
		
		inBuff += sizeof(NetMessage);
		numEntities = *reinterpret_cast<int*>(inBuff);
		
		inBuff += sizeof(numEntities);
		outTypes.clear();
		for (size_t i = 0; i < numEntities; ++i) {
			outTypes.push_back(*reinterpret_cast<Entity::E_Type*>(inBuff));
			inBuff += sizeof(Entity::E_Type);
		}
	}
	
	EntitySerializer* getDeserializedEntity(char* inBuff, int index) {
		EntitySerializer* result = nullptr;
		inBuff += sizeof(NetMessage) + sizeof(numEntities) + (sizeof(Entity::E_Type) * numEntities);
		int i = 0;
		while (i < numEntities && i <= index ) {
			Entity::E_Type entityType = outTypes[i];
			switch (entityType) {
				case Entity::E_Type::Circle: {
					if (i == index) result = reinterpret_cast<EntityCircleSerializer*>(inBuff);
					inBuff += sizeof(EntityCircleSerializer);
					break;
				}
				case Entity::E_Type::Player: {
					if (i == index) result = reinterpret_cast<EntityPlayerSerializer*>(inBuff);
					inBuff += sizeof(EntityPlayerSerializer);
					break;
				}
			}

			i++;
		}

		return result;
	}

	size_t getSerialBufferSize() {
		size_t iBytesSerial = sizeof(NetMessage) + sizeof(numEntities) + (sizeof(Entity::E_Type) * numEntities);
		for (auto entity : inEntities) {
			switch (entity->getType()) {
				case Entity::E_Type::Circle: {
					iBytesSerial += sizeof(EntityCircleSerializer);
					break;
				}
				case Entity::E_Type::Player: {
					iBytesSerial += sizeof(EntityPlayerSerializer);
					break;
				}
			}
		}
		return iBytesSerial;
	}

	std::vector<Entity*> inEntities;
	std::vector<Entity::E_Type> outTypes;
	size_t numEntities;
};
