#include "render.h"



bool Render::trace(const Vector &orig, const Vector &dir, const std::vector<std::unique_ptr<Object>> &objects, float &tNear, uint32_t &index, Vector2D &uv, Object **hitObject)
{
	*hitObject = nullptr;
	for (uint32_t k = 0; k < objects.size(); ++k) {
		float tNearK = kInfinity;
		uint32_t indexK;
		Vector2D uvK;
		if (objects[k]->intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear)
		{
			*hitObject = objects[k].get();
			tNear = tNearK;
			index = indexK;
			uv = uvK;
		}
	}

	return (*hitObject != nullptr);
}

inline Vector Render::castRay(const Vector & orig, const Vector & dir, const std::vector<std::unique_ptr<Object>>& objects, const std::vector<std::unique_ptr<Light>>& lights, const Options & options, uint32_t depth, bool test)
{
	if (depth > options.maxDepth)
	{
		return options.backgroundColor;
	}

	Vector hitColor = options.backgroundColor;
	float tnear = kInfinity;
	Vector2D uv;
	uint32_t index = 0;
	Object *hitObject = nullptr;
	if (trace(orig, dir, objects, tnear, index, uv, &hitObject))
	{
		Vector hitPoint = orig + dir * tnear;
		Vector normal; // normal 
		Vector2D st; // st coordinates 
		hitObject->getSurfaceProperties(hitPoint, dir, index, uv, normal, st);
		Vector tmp = hitPoint;
		switch (hitObject->materialType)
		{
		case REFLECTION_AND_REFRACTION:
		{
			Vector reflectionDirection = (reflect(dir, normal)).GetSafeNormal();
			Vector refractionDirection = (refract(dir, normal, hitObject->ior)).GetSafeNormal();
			Vector reflectionRayOrig = ((reflectionDirection | normal) < 0) ?
				hitPoint - normal * options.bias :
				hitPoint + normal * options.bias;
			Vector refractionRayOrig = ((refractionDirection | normal) < 0) ?
				hitPoint - normal * options.bias :
				hitPoint + normal * options.bias;
			Vector reflectionColor = castRay(reflectionRayOrig, reflectionDirection, objects, lights, options, depth + 1, 1);
			Vector refractionColor = castRay(refractionRayOrig, refractionDirection, objects, lights, options, depth + 1, 1);
			float kr;
			fresnel(dir, normal, hitObject->ior, kr);
			hitColor = reflectionColor * kr + refractionColor * (1 - kr);
			break;
		}
		case REFLECTION:
		{
			float kr;
			fresnel(dir, normal, hitObject->ior, kr);
			Vector reflectionDirection = reflect(dir, normal);
			Vector reflectionRayOrig = ((reflectionDirection | normal) < 0) ?
				hitPoint + normal * options.bias :
				hitPoint - normal * options.bias;
			hitColor = castRay(reflectionRayOrig, reflectionDirection, objects, lights, options, depth + 1) * kr;
			break;
		}
		default:
		{
			//环境光、高光
			Vector lightAmt = 0, specularColor = 0;
			Vector shadowPointOrig = ((dir | normal) < 0) ?
				hitPoint + normal * options.bias :
				hitPoint - normal * options.bias;
			for (uint32_t i = 0; i < lights.size(); ++i)
			{
				//到灯光的方向
				Vector lightDir = lights[i]->position - hitPoint;
				// 距离的平方
				float lightDistance2 = (lightDir | lightDir);
				lightDir = (lightDir).GetSafeNormal();
				float LdotN = std::max(0.f, (lightDir | normal));
				Object *shadowHitObject = nullptr;
				float tNearShadow = kInfinity;
				// 判断是否是在阴影中并且还要是距离，其还不能是最近的
				bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, uv, &shadowHitObject) &&
					tNearShadow * tNearShadow < lightDistance2;
				lightAmt += (1 - inShadow) * lights[i]->intensity * LdotN;
				Vector reflectionDirection = reflect(-lightDir, normal);
				//for debug
				auto Tema = std::max(0.f, ((-reflectionDirection) | dir));
				specularColor += powf(Tema, hitObject->specularExponent) * lights[i]->intensity;
			}
			//phone模型为：环境光+漫反射光+高光
			Color AmbineLight = 0 * lightAmt;
			Color DiffuseLight = hitObject->evalDiffuseColor(st) * hitObject->Kd;
			Color SpecularLight = specularColor * hitObject->Ks;
			hitColor = AmbineLight + DiffuseLight + specularColor;
			break;
		}
		}
	}

	return hitColor;
}

void Render::rendering(const Options &options, const std::vector<std::unique_ptr<Object>> &objects, const std::vector<std::unique_ptr<Light>> &lights)
{
	Vector *framebuffer = new Vector[options.width * options.height];
	Vector *pix = framebuffer;
	float scale = tan(deg2rad(options.fov * 0.5));
	float imageAspectRatio = options.width / (float)options.height;
	Vector orig(0);
	for (uint32_t j = 0; j < options.height; ++j)
	{
		for (uint32_t i = 0; i < options.width; ++i)
		{
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
	for (uint32_t i = 0; i < options.height * options.width; ++i)
	{
		char r = (char)(255 * clamp(0, 1, framebuffer[i].X));
		char g = (char)(255 * clamp(0, 1, framebuffer[i].Y));
		char b = (char)(255 * clamp(0, 1, framebuffer[i].Z));
		ofs << g << b << r;
	}

	ofs.close();

	delete[] framebuffer;
}
