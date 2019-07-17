#include "MeshTriangle.h"



bool Triangle::Intersect(Ray& p_Ray, double& p_Dist)
{
	Vector dir = p_Ray.direction();
	Vector ori = p_Ray.origin();
	double distance = -((ori - vertex[0]) | N) / (dir | N);
	if ((distance < 0.01) || (distance > 1e6)) return false;
	Vector interPoint = ori + distance * dir;
	double u, v;

	Vector diru = vertex[1] - vertex[0];
	(diru).Normalize();
	Vector Hu1 = vertex[0] + diru * (diru | (vertex[2] - vertex[0]));
	Vector Hu2 = vertex[0] + diru * (diru | (interPoint - vertex[0]));
	u = (interPoint - Hu2).X / (vertex[2] - Hu1).X;
	if (u < -0.01) return false;

	Vector dirv = vertex[2] - vertex[0];
	(dirv).Normalize();
	Vector Hv1 = vertex[0] + dirv * (dirv | (vertex[1] - vertex[0]));
	Vector Hv2 = vertex[0] + dirv * (dirv | (interPoint - vertex[0]));
	v = (interPoint - Hv2).X / (vertex[1] - Hv1).X;
	if (v < -0.01) return false;

	if (u + v > 1.0 + 0.01) return false;
	if (p_Dist > distance) p_Dist = distance;
	else return false;
	return true;
}
