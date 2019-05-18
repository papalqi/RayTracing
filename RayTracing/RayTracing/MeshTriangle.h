#pragma once
#include "CoreMinimal.h"
#include "Object.h"

bool rayTriangleIntersect(
	const Vector &v0, const Vector &v1, const Vector &v2,
	const Vector &orig, const Vector &dir,
	float &tnear, float &u, float &v)
{
	Vector edge1 = v1 - v0;
	Vector edge2 = v2 - v0;
	Vector pvec = (dir^edge2);
	float det = (edge1 | pvec);
	if (det == 0 || det < 0) return false;

	Vector tvec = orig - v0;
	u = (tvec | pvec);
	if (u < 0 || u > det) return false;

	Vector qvec = (tvec^ edge1);
	v = (dir | qvec);
	if (v < 0 || u + v > det) return false;

	float invDet = 1 / det;

	tnear = (edge2 | qvec) * invDet;
	u *= invDet;
	v *= invDet;

	return true;
}



class MeshTriangle : public Object
{
public:
	MeshTriangle(
		const Vector *verts,
		const uint32_t *vertsIndex,
		const uint32_t &numTris,
		const Vector2D *st)
	{
		uint32_t maxIndex = 0;
		for (uint32_t i = 0; i < numTris * 3; ++i)
			if (vertsIndex[i] > maxIndex) maxIndex = vertsIndex[i];
		maxIndex += 1;
		vertices = std::unique_ptr<Vector[]>(new Vector[maxIndex]);
		memcpy(vertices.get(), verts, sizeof(Vector) * maxIndex);
		vertexIndex = std::unique_ptr<uint32_t[]>(new uint32_t[numTris * 3]);
		memcpy(vertexIndex.get(), vertsIndex, sizeof(uint32_t) * numTris * 3);
		numTriangles = numTris;
		stCoordinates = std::unique_ptr<Vector2D[]>(new Vector2D[maxIndex]);
		memcpy(stCoordinates.get(), st, sizeof(Vector2D) * maxIndex);
	}

	bool intersect(const Vector &orig, const Vector &dir, float &tnear, uint32_t &index, Vector2D &uv) const
	{
		bool intersect = false;
		for (uint32_t k = 0; k < numTriangles; ++k) {
			const Vector & v0 = vertices[vertexIndex[k * 3]];
			const Vector & v1 = vertices[vertexIndex[k * 3 + 1]];
			const Vector & v2 = vertices[vertexIndex[k * 3 + 2]];
			float t, u, v;
			if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear) {
				tnear = t;
				uv.X = u;
				uv.Y = v;
				index = k;
				intersect |= true;
			}
		}

		return intersect;
	}

	void getSurfaceProperties(const Vector &P, const Vector &I, const uint32_t &index, const Vector2D &uv, Vector &N, Vector2D &st) const
	{
		const Vector &v0 = vertices[vertexIndex[index * 3]];
		const Vector &v1 = vertices[vertexIndex[index * 3 + 1]];
		const Vector &v2 = vertices[vertexIndex[index * 3 + 2]];
		Vector e0 = (v1 - v0).GetSafeNormal();
		Vector e1 = (v2 - v1).GetSafeNormal();
		N = ((e0^e1)).GetSafeNormal();;
		const Vector2D &st0 = stCoordinates[vertexIndex[index * 3]];
		const Vector2D &st1 = stCoordinates[vertexIndex[index * 3 + 1]];
		const Vector2D &st2 = stCoordinates[vertexIndex[index * 3 + 2]];
		st = st0 * (1 - uv.X - uv.Y) + st1 * uv.X + st2 * uv.Y;
	}

	Vector evalDiffuseColor(const Vector2D &st) const
	{
		float scale = 5;
		float pattern = (fmodf(st.X * scale, 1) > 0.5) ^ (fmodf(st.Y * scale, 1) > 0.5);
		return mix(Vector(0.815, 0.235, 0.031), Vector(0.937, 0.937, 0.231), pattern);
	}

	std::unique_ptr<Vector[]> vertices;
	uint32_t numTriangles;
	std::unique_ptr<uint32_t[]> vertexIndex;
	std::unique_ptr<Vector2D[]> stCoordinates;
};