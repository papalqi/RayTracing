#include "Camera.h"



FCamera::FCamera()
{
	lookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
	perspective(60, 1.0, 0.01, 2000.0);
}

Ray FCamera::getRay(double x, double y)
{
	Vector direction = view_z + (x - 0.5f) * view_x
		+ (y - 0.5) * view_y;
	return Ray(origin, direction);
}

void FCamera::lookAt(double ex, double ey, double ez, double lx, double ly, double lz, double upx, double upy, double upz)
{
	origin = Vector(ex, ey, ez);
	direction = Vector(lx - ex, ly - ey, lz - ez);
	up = Vector(upx, upy, upz);

	right = ((direction ^ up)).GetSafeNormal();
	up = ((right ^ direction)).GetSafeNormal();
}

void FCamera::perspective(double fov, double aspect, double near, double far)
{
	nearPlane = near;
	farPlane = far;

	height = 2 * nearPlane * tan(fov / 2 * PI / 180);
	width = height * aspect;

	calcViewPort(fov, width, height);
}

void FCamera::calcViewPort(double fov, double width, double height)
{
	this->fov = fov;
	this->width = width;
	this->height = height;

	double aspect = height / width;

	view_x = 2 * tan(fov * PI / 360) * right;
	view_y = 2 * tan(fov * aspect * PI / 360) * up;
	view_z = direction;
}
