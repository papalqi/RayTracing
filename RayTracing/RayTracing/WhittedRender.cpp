#include "WhittedRender.h"
#include "RayTracingDifine.h"

oocd::Vector WhittedRender::Shader(const Ray p_ray, uint32_t depth, bool test /*= false*/)
{
	if (depth > options->maxDepth)
	{
		return Vector::ZeroVector;
	}

	Vector hitColor = options->backgroundColor;
	float tnear = kInfinity;
	Vector2D uv;
	uint32_t index = 0;
	Object* hitObject = nullptr;
	Vector hitPoint;

	if (FindNearest(p_ray, tnear, &hitObject))
	{
		hitColor = 0;
		hitPoint = p_ray.origin() + p_ray.direction() * tnear;//hitPoint为光线与最近物体交汇的地方

		Color LightColor;
		for (int l = 0; l < lights.size(); ++l)
		{
			Light* p = lights[l].get();
			{
				Light* light = p;
				float shadow = 1.0;
				Vector L;
				shadow = GetShadowFact(light, hitPoint, L);
				Vector N = hitObject->getNormal(hitPoint);//物体在pi处的法向量
				if (hitObject->getMaterial()->getDiffuse() > 0)//漫反射
				{
					float dot = (L | N);
					if (dot > 0)
					{
						float diff = dot * hitObject->getMaterial()->getDiffuse() * shadow;
						hitColor += diff * light->intensity * hitObject->getColor(hitPoint);
					}
				}
				if (hitObject->getMaterial()->getSpecular() > 0)//高光
				{
					// point light source: sample once for specular highlight
					Vector V = p_ray.direction();//光线方向
					Vector R = L - 2.0 * (L | N) * N;
					float dot = (V | R);
					if (dot > 0)
					{
						float spec = pow(dot, 20) * hitObject->getMaterial()->getSpecular() * shadow;
						hitColor += spec * light->intensity;
					}
				}

			}

		}

		float refl = hitObject->getMaterial()->getReflection();
		if (refl > 0.0)//镜面反射
		{
			float drefl = hitObject->getMaterial()->getDiffRefl();
			Vector N = hitObject->getNormal(hitPoint);
			Vector R = p_ray.direction() - 2.0 * (p_ray.direction() | N) * N;
			R.Normalize();

			if (depth < WhittedMaxBound)//光滑镜面反射
			{

				double dist;

				Color rcol = Shader(Ray(hitPoint + options->bias * R, R), depth + 1);

				hitColor += refl * rcol * hitObject->getColor(hitPoint);
			}
		}
		//计算折射
		double refr = hitObject->getMaterial()->getRefraction();
		auto normal = hitObject->getNormal(hitPoint);
		if (refr > 0.0 && depth < WhittedMaxBound)
		{

			Vector reflectionDirection = (RTMath::reflect(p_ray.direction(), normal)).GetSafeNormal();
			Vector refractionDirection = (RTMath::refract(p_ray.direction(), normal,
				hitObject->m_Material.getRefraction()).GetSafeNormal());
			Vector reflectionRayOrig = ((reflectionDirection | normal) < 0) ?
				hitPoint - normal * options->bias :
				hitPoint + normal * options->bias;
			Vector refractionRayOrig = ((refractionDirection | normal) < 0) ?
				hitPoint - normal * options->bias :
				hitPoint + normal * options->bias;
			Vector reflectionColor = Shader(Ray(reflectionRayOrig, reflectionDirection), depth + 1, 1);
			Vector refractionColor = Shader(Ray(refractionRayOrig, refractionDirection), depth + 1, 1);

			RTMath::fresnel(p_ray.direction(), normal, hitObject->m_Material.m_Refr, hitObject->m_Material.m_Spec);
			hitColor = reflectionColor * hitObject->m_Material.m_Refr + refractionColor * (1 - hitObject->m_Material.m_Refr);

		}

	}



	return hitColor;
}
