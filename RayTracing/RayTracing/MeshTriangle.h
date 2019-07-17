#pragma once
#include "CoreMinimal.h"
#include "Object.h"
#include "Color.h"
inline
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



class Triangle : public Object
{
public:
	Triangle(Vector A, Vector B, Vector C)
	{
		vertex[0] = A; vertex[1] = B; vertex[2] = C;
		
		N = -(B - A)^(C - B);
		N.Normalize();
		bLight = false;
	}
	double GetMinCoord(int coord) {
		double x0 = vertex[0][coord];
		double x1 = vertex[1][coord];
		double x2 = vertex[2][coord];
		if (x0 < x1)
			return (x0 < x2) ? x0 : x2;
		return (x1 < x2) ? x1 : x2;
	}
	double GetMaxCoord(int coord) {
		double x0 = vertex[0][coord];
		double x1 = vertex[1][coord];
		double x2 = vertex[2][coord];
		if (x0 > x1)
			return (x0 > x2) ? x0 : x2;
		return (x1 > x2) ? x1 : x2;
	}
	int getType() { return TRIANGLE; }
	bool Intersect(Ray& p_Ray, double& p_Dist);
	//bool H_IntersectBox(BoundingBox&);
	inline Vector getNormal(Vector& p_Pos) { return N; }
	inline Vector getNormal() { return N; }
	//Color getColor(Vector& p_Pos);
	//BoundingBox getBoundingBox();
protected:
	Vector vertex[3];
	Vector N;
};

class ObjTriangle : public Triangle
{
public:
	ObjTriangle(Vector A, Vector B, Vector C) :Triangle(A, B, C) {}
	int vertexIndex[3];
	int id;
	Object* obj;
	Vector getNormal(Vector& p_Pos);
	inline Vector getNormal() { return N; }
};
//class Mesh
//{
//public:
//	Mesh() {}
//	Vector m_move;
//	double m_alpha;
//	vector<ObjTriangle*> TriangleVec;
//	vector<vector<int> > TriangleInfo;
//
//	inline void setInfo(Vector p_move, double p_alpha)
//	{
//		m_move = p_move;
//		m_alpha = p_alpha;
//	}
//	void readin(std::string, Object**, int&, std::vector<TriangleTree*>&);
//	void setMaterial(material*);
//};