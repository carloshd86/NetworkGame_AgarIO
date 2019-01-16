#pragma once

#include "entity.h"

struct EntitySerializer {

	EntitySerializer() {}
	EntitySerializer(Entity::E_Type type) : type(type) {}
	EntitySerializer(const Entity& entity) : 
		type (entity.getType()),
		id   (entity.getId()),
		posX (entity.getPosX()),
		posY (entity.getPosY()) {}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = 0;
		*reinterpret_cast<Entity::E_Type*>(outBuff) = type;
		iBytesSerial += sizeof(type);

		outBuff += sizeof(type);
		*reinterpret_cast<float*>(outBuff) = posX;
		iBytesSerial += sizeof(posX);

		outBuff += sizeof(posX);
		*reinterpret_cast<float*>(outBuff) = posY;
		iBytesSerial += sizeof(posY);

		outBuff += sizeof(posY);
		*reinterpret_cast<int*>(outBuff) = id;
		iBytesSerial += sizeof(id);

		return iBytesSerial;
	}

	void deserialize(char* inBuff) {
		type = *reinterpret_cast<Entity::E_Type*>(inBuff);

		inBuff += sizeof(type);
		posX = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(posX);
		posY = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(posY);
		id = *reinterpret_cast<int*>(inBuff);
	}

	Entity::E_Type type;
	float posX;
	float posY;
	int id;
};
