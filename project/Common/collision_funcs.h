#pragma once

#include "vec2.h"
#include <algorithm>
#include <cstdint>

inline bool valInRange(float val, float minVal, float maxVal) {
	return val >= minVal && val <= maxVal;
}

inline float clamp(float val, float minVal, float maxVal) {
	return (std::min)((std::max)(val, minVal), maxVal);
}

inline Vec2 closestPointInRect(const Vec2& point, const Vec2& rectPos, const Vec2& rectSize) {
	Vec2 rectEnd = rectPos + rectSize;
	return Vec2(
		clamp(point.x, rectPos.x, rectEnd.x),
		clamp(point.y, rectPos.y, rectEnd.y)
	);
}

inline void getOverlap(const Vec2& rectPos1, const Vec2& rectSize1, const Vec2& rectPos2, const Vec2& rectSize2, Vec2& outPos, Vec2& outSize) {
	Vec2 rectEnd1 = rectPos1 + rectSize1;
	Vec2 rectEnd2 = rectPos2 + rectSize2;
	outPos.x = valInRange(rectPos1.x, rectPos2.x, rectEnd2.x) ? rectPos1.x : rectPos2.x;
	outPos.y = valInRange(rectPos1.y, rectPos2.y, rectEnd2.y) ? rectPos1.y : rectPos2.y;
	outSize.x = valInRange(rectEnd1.x, rectPos2.x, rectEnd2.x) ? rectEnd1.x - outPos.x : rectEnd2.x - outPos.x;
	outSize.y = valInRange(rectEnd1.y, rectPos2.y, rectEnd2.y) ? rectEnd1.y - outPos.y : rectEnd2.y - outPos.y;
}

inline bool checkCircleCircle(const Vec2& pos1, float radius1, const Vec2& pos2, float radius2) {
	float sumRad = radius1 + radius2;
	float sumSq = sumRad * sumRad;
	return (pos1 - pos2).squareLength() <= sumSq;
}

inline bool checkCircleRect(const Vec2& circlePos, float circleRadius, const Vec2& rectPos, const Vec2& rectSize) {
	Vec2 closestPoint = closestPointInRect(circlePos, rectPos, rectSize);
	return (closestPoint - circlePos).squareLength() <= circleRadius * circleRadius;
}

inline bool checkRectRect(const Vec2& rectPos1, const Vec2& rectSize1, const Vec2& rectPos2, const Vec2& rectSize2) {
	Vec2 rectEnd1 = rectPos1 + rectSize1;
	Vec2 rectEnd2 = rectPos2 + rectSize2;
	bool xoverlap = valInRange(rectPos1.x, rectPos2.x, rectEnd2.x) || valInRange(rectPos2.x, rectPos1.x, rectEnd1.x);
	bool yoverlap = valInRange(rectPos1.y, rectPos2.y, rectEnd2.y) || valInRange(rectPos2.y, rectPos1.y, rectEnd1.y);
	return xoverlap && yoverlap;
}

inline bool checkCirclePixels(const Vec2& circlePos, float circleRadius, const Vec2& pixelsPos, const Vec2& pixelsSize, const uint8_t* pixels) {
	Vec2 circleOrigin = circlePos - circleRadius;
	Vec2 circleSize(circleRadius * 2, circleRadius * 2);
	if (!checkRectRect(pixelsPos, pixelsSize, circleOrigin, circleSize)) return false;
	Vec2 overlapPos, overlapSize;
	getOverlap(pixelsPos, pixelsSize, circleOrigin, circleSize, overlapPos, overlapSize);
	Vec2 pixelsOrigin = overlapPos - pixelsPos;
	for (size_t py = 0; py < static_cast<size_t>(overlapSize.y); ++py) {
		for (size_t px = 0; px < static_cast<size_t>(overlapSize.x); ++px) {
			size_t xpos = static_cast<size_t>(pixelsOrigin.x) + px;
			size_t ypos = static_cast<size_t>(pixelsOrigin.y) + py;
			if (pixels[(ypos * static_cast<size_t>(pixelsSize.x) + xpos) * 4 + 3] >= 128 && (pixelsPos + Vec2(static_cast<float>(xpos), static_cast<float>(ypos)) - circlePos).squareLength() <= circleRadius*circleRadius ) {
				return true;
			}
		}
	}
	return false;
}

inline bool checkPixelsPixels(const Vec2& pixelsPos1, const Vec2& pixelsSize1, const uint8_t* pixels1, const Vec2& pixelsPos2, const Vec2& pixelsSize2, const uint8_t* pixels2) {
	if (!checkRectRect(pixelsPos1, pixelsSize1, pixelsPos2, pixelsSize2)) return false;
	Vec2 overlapPos, overlapSize;
	getOverlap(pixelsPos1, pixelsSize1, pixelsPos2, pixelsSize2, overlapPos, overlapSize);
	Vec2 pixelsOrigin1 = overlapPos - pixelsPos1;
	Vec2 pixelsOrigin2 = overlapPos - pixelsPos2;
	for (size_t py = 0; py < static_cast<size_t>(overlapSize.y); ++py ) {
		for (size_t px = 0; px < static_cast<size_t>(overlapSize.x); ++px ) {
			size_t xpos1 = static_cast<size_t>(pixelsOrigin1.x) + px;
			size_t ypos1 = static_cast<size_t>(pixelsOrigin1.y) + py;
			size_t xpos2 = static_cast<size_t>(pixelsOrigin2.x) + px;
			size_t ypos2 = static_cast<size_t>(pixelsOrigin2.y) + py;
			if ( pixels1[(ypos1 * static_cast<size_t>(pixelsSize1.x) + xpos1)*4 + 3] >= 128 && pixels2[(ypos2 * static_cast<size_t>(pixelsSize2.x) + xpos2) * 4 + 3] >= 128 ) {
				return true;
			}
		}
	}
	return false;
}

inline bool checkPixelsRect(const Vec2& pixelsPos, const Vec2& pixelsSize, const uint8_t* pixels, const Vec2& rectPos, const Vec2& rectSize) {
	if (!checkRectRect(pixelsPos, pixelsSize, rectPos, rectSize)) return false;
	Vec2 overlapPos, overlapSize;
	getOverlap(pixelsPos, pixelsSize, rectPos, rectSize, overlapPos, overlapSize);
	Vec2 pixelsOrigin = overlapPos - pixelsPos;
	for (size_t py = 0; py < static_cast<size_t>(overlapSize.y); ++py) {
		for (size_t px = 0; px < static_cast<size_t>(overlapSize.x); ++px) {
			size_t xpos = static_cast<size_t>(pixelsOrigin.x) + px;
			size_t ypos = static_cast<size_t>(pixelsOrigin.y) + py;
			if (pixels[(ypos * static_cast<size_t>(pixelsSize.x) + xpos) * 4 + 3] >= 128) {
				return true;
			}
		}
	}
	return false;
}
