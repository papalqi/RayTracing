#pragma once
#include "CoreMinimal.h"
#include "Random.h"
#include "Ray.h"
class Ray;


Vector random_in_unit_disk() {
	Vector p;
	do {
		p = 2.0*Vector(drand48(), drand48(), 0) - Vector(1, 1, 0);
	} while ((p| p) >= 1.0);
	return p;
}


struct Camera
{
	// 原点
	Vector mPosition;
	// orthonormal frame so that -w is the viewing direction. 
	Vector u, v, w;
	// image plane distance / focal length
	double distance;
	// 最后图像的长和宽
	double width, height;


	Camera(Vector lookfrom, Vector lookat, Vector vup, float vfov, float aspect, float aperture, float focus_dist) { // vfov is top to bottom in degrees
		lens_radius = aperture / 2;
		float theta = vfov * PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		mPosition = lookfrom;
		w = (lookfrom - lookat).GetSafeNormal();
		u = ((vup^ w)).GetSafeNormal();
		v = (w^u).GetSafeNormal();
		lower_left_corner = mPosition - half_width * focus_dist*u - half_height * focus_dist*v - focus_dist * w;
		horizontal = 2 * half_width*focus_dist*u;
		vertical = 2 * half_height*focus_dist*v;
	}
	Ray get_ray(float s, float t)
	{
		Vector rd = lens_radius * random_in_unit_disk();
		Vector offset = u * rd.X + v * rd.Y;
		return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}
	Vector origin;
	Vector lower_left_corner;
	Vector horizontal;
	Vector vertical;
	
	float lens_radius;
};