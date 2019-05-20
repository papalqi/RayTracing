
#include "CoreMinimal.h"
#include "Camera.h"
#include"Object.h"
#include"Shpere.h"
#include"material.h"
#include "ObjectList.h"

#include "Light.h"
#include <cstdio> 
#include <cstdlib> 
#include <memory> 
#include <vector> 
#include <utility> 
#include <cstdint> 
#include <iostream> 
#include <fstream> 
#include <cmath> 
#include "MeshTriangle.h"

const float kInfinity = std::numeric_limits<float>::max();

struct Options
{
	uint32_t width;
	uint32_t height;
	float fov;
	float imageAspectRatio;
	uint8_t maxDepth;
	Vector backgroundColor;
	float bias;
};




bool trace(
	const Vector &orig, const Vector &dir,
	const std::vector<std::unique_ptr<Object>> &objects,
	float &tNear, uint32_t &index, Vector2D &uv, Object **hitObject)
{
	*hitObject = nullptr;
	for (uint32_t k = 0; k < objects.size(); ++k) {
		float tNearK = kInfinity;
		uint32_t indexK;
		Vector2D uvK;
		if (objects[k]->intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear) {
			*hitObject = objects[k].get();
			tNear = tNearK;
			index = indexK;
			uv = uvK;
		}
	}

	return (*hitObject != nullptr);
}
Vector castRay(
	const Vector &orig, const Vector &dir,
	const std::vector<std::unique_ptr<Object>> &objects,
	const std::vector<std::unique_ptr<Light>> &lights,
	const Options &options,
	uint32_t depth,
	bool test = false)
{
	if (depth > options.maxDepth) {
		return options.backgroundColor;
	}

	Vector hitColor = options.backgroundColor;
	float tnear = kInfinity;
	Vector2D uv;
	uint32_t index = 0;
	Object *hitObject = nullptr;
	if (trace(orig, dir, objects, tnear, index, uv, &hitObject)) {
		Vector hitPoint = orig + dir * tnear;
		Vector N; // normal 
		Vector2D st; // st coordinates 
		hitObject->getSurfaceProperties(hitPoint, dir, index, uv, N, st);
		Vector tmp = hitPoint;
		switch (hitObject->materialType) {
		case REFLECTION_AND_REFRACTION:
		{
			Vector reflectionDirection = (reflect(dir, N)).GetSafeNormal();
			Vector refractionDirection = (refract(dir, N, hitObject->ior)).GetSafeNormal();
			Vector reflectionRayOrig = ((reflectionDirection| N) < 0) ?
				hitPoint - N * options.bias :
				hitPoint + N * options.bias;
			Vector refractionRayOrig = ((refractionDirection| N) < 0) ?
				hitPoint - N * options.bias :
				hitPoint + N * options.bias;
			Vector reflectionColor = castRay(reflectionRayOrig, reflectionDirection, objects, lights, options, depth + 1, 1);
			Vector refractionColor = castRay(refractionRayOrig, refractionDirection, objects, lights, options, depth + 1, 1);
			float kr;
			fresnel(dir, N, hitObject->ior, kr);
			hitColor = reflectionColor * kr + refractionColor * (1 - kr);
			break;
		}
		case REFLECTION:
		{
			float kr;
			fresnel(dir, N, hitObject->ior, kr);
			Vector reflectionDirection = reflect(dir, N);
			Vector reflectionRayOrig = ((reflectionDirection|N) < 0) ?
				hitPoint + N * options.bias :
				hitPoint - N * options.bias;
			hitColor = castRay(reflectionRayOrig, reflectionDirection, objects, lights, options, depth + 1) * kr;
			break;
		}
		default:
		{  Vector lightAmt = 0, specularColor = 0;
		Vector shadowPointOrig = ((dir| N) < 0) ?
			hitPoint + N * options.bias :
			hitPoint - N * options.bias;        for (uint32_t i = 0; i < lights.size(); ++i) {
			Vector lightDir = lights[i]->position - hitPoint;
			// square of the distance between hitPoint and the light
			float lightDistance2 = (lightDir| lightDir);
			lightDir = (lightDir).GetSafeNormal();
			float LdotN = std::max(0.f, (lightDir| N));
			Object *shadowHitObject = nullptr;
			float tNearShadow = kInfinity;
			// is the point in shadow, and is the nearest occluding object closer to the object than the light itself?
			bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, uv, &shadowHitObject) &&
				tNearShadow * tNearShadow < lightDistance2;
			lightAmt += (1 - inShadow) * lights[i]->intensity * LdotN;
			Vector reflectionDirection = reflect(-lightDir, N);
			specularColor += powf(std::max(0.f, -(reflectionDirection|dir)), hitObject->specularExponent) * lights[i]->intensity;
		}
		hitColor = lightAmt * hitObject->evalDiffuseColor(st) * hitObject->Kd + specularColor * hitObject->Ks;
		break;
		}
		}
	}

	return hitColor;
}


void render(
	const Options &options,
	const std::vector<std::unique_ptr<Object>> &objects,
	const std::vector<std::unique_ptr<Light>> &lights)
{
	Vector *framebuffer = new Vector[options.width * options.height];
	Vector *pix = framebuffer;
	float scale = tan(deg2rad(options.fov * 0.5));
	float imageAspectRatio = options.width / (float)options.height;
	Vector orig(0);
	for (uint32_t j = 0; j < options.height; ++j) {
		for (uint32_t i = 0; i < options.width; ++i) {
			// generate primary ray direction
			float x = (2 * (i + 0.5) / (float)options.width - 1) * imageAspectRatio * scale;
			float y = (1 - 2 * (j + 0.5) / (float)options.height) * scale;
			Vector dir = (Vector(x, y, -1)).GetSafeNormal();
			*(pix++) = castRay(orig, dir, objects, lights, options, 0);
		}
	}

	// save framebuffer to file
	std::ofstream ofs;
	ofs.open("./out.ppm");
	ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
	for (uint32_t i = 0; i < options.height * options.width; ++i) {
		char r = (char)(255 * clamp(0, 1, framebuffer[i].X));
		char g = (char)(255 * clamp(0, 1, framebuffer[i].Y));
		char b = (char)(255 * clamp(0, 1, framebuffer[i].Z));
		ofs << r << g << b;
	}

	ofs.close();

	delete[] framebuffer;
}


int main()
{
	// creating the scene (adding objects and lights)
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Light>> lights;

	Sphere *sph1 = new Sphere(Vector(-1, 0, -12), 2);
	sph1->materialType = DIFFUSE_AND_GLOSSY;
	sph1->diffuseColor = Vector(0.6, 0.7, 0.8);
	Sphere *sph2 = new Sphere(Vector(0.5, -0.5, -8), 1.5);
	sph2->ior = 1.5;
	sph2->materialType = REFLECTION_AND_REFRACTION;

	objects.push_back(std::unique_ptr<Sphere>(sph1));
	objects.push_back(std::unique_ptr<Sphere>(sph2));

	Vector verts[4] = { {-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16} };
	uint32_t vertIndex[6] = { 0, 1, 3, 1, 2, 3 };
	Vector2D st[4] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
	MeshTriangle *mesh = new MeshTriangle(verts, vertIndex, 2, st);
	mesh->materialType = DIFFUSE_AND_GLOSSY;

	objects.push_back(std::unique_ptr<MeshTriangle>(mesh));

	lights.push_back(std::unique_ptr<Light>(new Light(Vector(-20, 70, 20), 0.5)));
	lights.push_back(std::unique_ptr<Light>(new Light(Vector(30, 50, -12), 1)));

	// setting up options
	Options options;
	options.width = 640;
	options.height = 480;
	options.fov = 90;
	options.backgroundColor = Vector(0.235294, 0.67451, 0.843137);
	options.maxDepth = 5;
	options.bias = 0.00001;

	// finally, render
	render(options, objects, lights);

	return 0;
}
