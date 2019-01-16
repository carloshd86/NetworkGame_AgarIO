#pragma once

#include "EntitySerializer.h"
#include "entitycircle.h"

struct EntityCircleSerializer : public EntitySerializer {

	EntityCircleSerializer() : EntitySerializer(Entity::E_Type::Circle) {}
	EntityCircleSerializer(const EntityCircle& entityCircle) :
		EntitySerializer(entityCircle),
		radius (entityCircle.getRadius()),
		red    (entityCircle.getRed()),
		green  (entityCircle.getGreen()),
		blue   (entityCircle.getBlue()) {}

	size_t serialize(char* outBuff) {
		size_t iBytesSerial = EntitySerializer::serialize(outBuff);

		outBuff += sizeof(EntitySerializer);
		*reinterpret_cast<float*>(outBuff) = radius;
		iBytesSerial += sizeof(radius);

		outBuff += sizeof(radius);
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
		red = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(red);
		green = *reinterpret_cast<float*>(inBuff);

		inBuff += sizeof(green);
		blue = *reinterpret_cast<float*>(inBuff);
	}

	float radius;
	float red;
	float green;
	float blue;
};
