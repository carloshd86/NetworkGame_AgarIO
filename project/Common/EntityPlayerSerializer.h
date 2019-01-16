#pragma once

#include "EntitySerializer.h"
#include "entityplayer.h"

struct EntityPlayerSerializer : public EntitySerializer {

	EntityPlayerSerializer() : EntitySerializer(Entity::E_Type::Player) {}
	EntityPlayerSerializer(const EntityPlayer& entityPlayer) :
		EntitySerializer(entityPlayer),
		radius (entityPlayer.getRadius()),
		speed  (entityPlayer.getSpeed()),
		red    (entityPlayer.getRed()),
		green  (entityPlayer.getGreen()),
		blue   (entityPlayer.getBlue()) {}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = EntitySerializer::serialize(outBuff);

		outBuff += sizeof(EntitySerializer);
		*reinterpret_cast<float*>(outBuff) = radius;
		iBytesSerial += sizeof(radius);

		outBuff += sizeof(radius);
		*reinterpret_cast<float*>(outBuff) = speed;
		iBytesSerial += sizeof(speed);

		outBuff += sizeof(speed);
		*reinterpret_cast<float*>(outBuff) = red;
		iBytesSerial += sizeof(red);

		outBuff += sizeof(red);
		*reinterpret_cast<float*>(outBuff) = green;
		iBytesSerial += sizeof(green);

		outBuff += sizeof(green);
		*reinterpret_cast<float*>(outBuff) = blue;
		iBytesSerial += sizeof(blue);

		return iBytesSerial;
	}

	void deserialize(char* inBuff) {
		EntitySerializer::deserialize(inBuff);
		
		inBuff += sizeof(EntitySerializer);
		radius = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(radius);
		speed = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(speed);
		red = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(red);
		green = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(green);
		blue = *reinterpret_cast<float*>(inBuff);
	}

	float radius;
	float speed;
	float red;
	float green;
	float blue;
};
