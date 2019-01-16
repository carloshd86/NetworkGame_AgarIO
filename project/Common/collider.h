#pragma once

#include "collision_funcs.h"
#include <vector>

class Collider {
public:
	enum CollisionType {
		COLLISION_NONE,
		COLLISION_CIRCLE,
		COLLISION_RECT,
		COLLISION_PIXELS
	};

	virtual ~Collider() {}
	virtual bool collides(const Collider& other) const = 0;
	virtual bool collides(const Vec2& circlePos, float circleRadius) const = 0;
	virtual bool collides(const Vec2& rectPos, const Vec2& rectSize) const = 0;
	virtual bool collides(const Vec2& pixelsPos, const Vec2& pixelsSize, const uint8_t* pixels) const = 0;
};

class CircleCollider : public Collider {
public:
	CircleCollider(const Vec2& center, const float& radius) : mCenter(center), mRadius(radius) {}
	virtual bool collides(const Collider& other) const { return other.collides(mCenter, mRadius); }
	virtual bool collides(const Vec2& circlePos, float circleRadius) const { return checkCircleCircle(mCenter, mRadius, circlePos, circleRadius); }
	virtual bool collides(const Vec2& rectPos, const Vec2& rectSize) const { return checkCircleRect(mCenter, mRadius, rectPos, rectSize); }
	virtual bool collides(const Vec2& pixelsPos, const Vec2& pixelsSize, const uint8_t* pixels) const { return checkCirclePixels(mCenter, mRadius, pixelsPos, pixelsSize, pixels); }
private:
	const Vec2&		mCenter;
	const float&	mRadius;
};

class RectCollider : public Collider {
public:
	RectCollider(const Vec2& origin, const Vec2& size) : mOrigin(origin), mSize(size) {}
	virtual bool collides(const Collider& other) const { return other.collides(mOrigin, mSize); }
	virtual bool collides(const Vec2& circlePos, float circleRadius) const { return checkCircleRect(circlePos, circleRadius, mOrigin, mSize); }
	virtual bool collides(const Vec2& rectPos, const Vec2& rectSize) const { return checkRectRect(mOrigin, mSize, rectPos, rectSize); }
	virtual bool collides(const Vec2& pixelsPos, const Vec2& pixelsSize, const uint8_t* pixels) const { return checkPixelsRect(pixelsPos, pixelsSize, pixels, mOrigin, mSize); }
private:
	const Vec2&	mOrigin;
	const Vec2&	mSize;
};

class PixelsCollider : public Collider {
public:
	PixelsCollider(const Vec2& origin, const Vec2& size, const std::vector<uint8_t>& pixels) : mOrigin(origin), mSize(size), mPixels(pixels) {}
	virtual bool collides(const Collider& other) const { return other.collides(mOrigin, mSize, mPixels.data()); }
	virtual bool collides(const Vec2& circlePos, float circleRadius) const { return checkCirclePixels(circlePos, circleRadius, mOrigin, mSize, mPixels.data()); }
	virtual bool collides(const Vec2& rectPos, const Vec2& rectSize) const { return checkPixelsRect(mOrigin, mSize, mPixels.data(), rectPos, rectSize); }
	virtual bool collides(const Vec2& pixelsPos, const Vec2& pixelsSize, const uint8_t* pixels) const { return checkPixelsPixels(mOrigin, mSize, mPixels.data(), pixelsPos, pixelsSize, pixels); }
private:
	const Vec2&					mOrigin;
	const Vec2&					mSize;
	const std::vector<uint8_t>&	mPixels;
};