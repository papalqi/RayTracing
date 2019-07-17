#include "DistributedRender.h"
#include "ShpereLighting.h"
#include "RayTracingDifine.h"

oocd::Vector DistributedRender::Shader(const Ray p_ray, uint32_t depth, bool test /*= false*/)
{
	return Shader(p_ray,depth, Const_Max_Sample, (1.0 / Const_Max_Sample));
}

oocd::Vector DistributedRender::Shader(const Ray p_ray, uint32_t depth, float p_Sample, float p_SampleRange, bool test /*= false*/)
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
		if (hitObject->IsLight())
		{
			return ((BaseLighting*)hitObject)->intensity;
		}
		hitColor = 0;
		hitPoint = p_ray.origin() + p_ray.direction() * tnear;//hitPoint为光线与最近物体交汇的地方

		
		Color LightColor;
		for (int l = 0; l < lights.size(); ++l)
		{
			BaseLighting* p = lights[l].get();
			{
				BaseLighting* light = p;
				float shadow = 1.0;
				Vector L;
				shadow = GetShadowFact(light, hitPoint, L, p_Sample, p_SampleRange);
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
		//如果有反射的话计算反射
		float refl = hitObject->getMaterial()->getReflection();
		if (refl > 0.0)//反射
		{
			float drefl = hitObject->getMaterial()->getDiffRefl();
			Vector N = hitObject->getNormal(hitPoint);
			Vector R = p_ray.direction() - 2.0 * (p_ray.direction() | N) * N;
			R.Normalize();
			Vector component1 = Vector(R.Z, 0.0, -R.X);
			Vector component2 = R ^ (component1);
			component2.Normalize();
			int num = 5;
			for (int i = 0; i < num; i++)
			{
				float x0, y0;
				//随机取值
				while (1)
				{
					x0 = 2.0 * (double)rand() / RAND_MAX - 1.0;
					y0 = 2.0 * (double)rand() / RAND_MAX - 1.0;
					if ((x0 * x0 + y0 * y0) < 1.0)
						break;
				}
				x0 *= drefl;
				y0 *= drefl;
				Vector newR = R + component1 * x0 + component2 * y0;
				newR.Normalize();
				Color rcol = Shader(Ray(hitPoint + newR * options->bias, newR), depth + 1,p_Sample * 0.25, p_SampleRange * 4.0);
				hitColor += refl * rcol * hitObject->getColor(hitPoint) * (1.0f / (float)num);
			}
		
		}
		//如果有折射的话计算折射
		double refr = hitObject->getMaterial()->getRefraction();
		auto normal = -hitObject->getNormal(hitPoint);
		if (refr > 0.0 && depth < WhittedMaxBound)
		{
			float tmp_Refr_rate = hitObject->getMaterial()->getRefr_Rate();

			float n = tmp_Refr_rate / 1;//介质相对于空气的相对折射率
			Vector V = p_ray.direction();
			float cosi = -(V| normal);
			float sinr2 = (1.0 - (V| normal) * (V| normal)) / (n * n);
			if (0.0 < sinr2 && sinr2 < 1.0)
			{
				float cosr2 = 1.0 - sinr2;
				float sinr = sqrt(sinr2);
				float cosr = sqrt(cosr2);
				Vector T = (V * (1 / n)) + (cosi / n - sqrt(cosr2)) * normal;
				Color rcol = Shader(Ray(hitPoint + T * options->bias, T), depth + 1, p_Sample * 0.25, p_SampleRange * 4.0);


				Color absorbance = hitObject->getColor(hitPoint) * 0.15f/* * -dist*/;
				Color transparency = Color(exp(absorbance.X), exp(absorbance.Y), exp(absorbance.Z));
				hitColor += rcol * transparency;
			}
		}

	}
	return hitColor;
}


float DistributedRender::GetShadowFact(BaseLighting* mlt, Vector position, Vector& p_Dir, float p_Sample, float p_SampleRange)
{
	float ShadowFact = 0.0;


	if (mlt->getType() == Shpere)
	{
		ShpereLighting* light = (ShpereLighting*)mlt;
		Vector O = light->getCentre();
		p_Dir = O - position;
		(p_Dir).Normalize();
		float R = light->getRadius();
		Vector dir = O - position;
		float dist = (dir).Size();
		(dir).Normalize();
		Object* prim;
		//计算到达
		if (FindNearest(Ray(position + dir  , dir), dist, &prim))
			if (prim == mlt)
				ShadowFact += 1.0;
	}
	if (mlt->LightType == LightClass::Rect)
	{

		Vector P = mlt->position;
		Vector size = mlt->Size;
		p_Dir = P + 0.5 * size - position;
		(p_Dir).Normalize();
		//采样个数
		int n = (int)floor(sqrt(p_Sample + 0.5));
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j)
			{
				Vector d = Vector(size.X / n, 0.0, size.Z / n);
				Vector pos = P + Vector(d.X * (i + (double)rand() / RAND_MAX), 0.0, d.Z * (j + (double)rand() / RAND_MAX));
				Vector dir = pos - position;
				float dist = (dir).Size();
				Object *prim;
				(dir).Normalize();
				if (FindNearest(Ray(position + dir /** options->bias*/, dir), dist, &prim))
						ShadowFact += 1.0 / (n * n);
			}
	}
	if (mlt->LightType == point)
	{
		p_Dir = mlt->position - position;
		p_Dir.Normalize();
		position = position + p_Dir;
		float tNearShadow = kInfinity;
		// 判断是否是在阴影中并且还要是距离，其还不能是最近的
		Object* shadowHitObject;
		bool inShadow = FindNearest(Ray(position, p_Dir), tNearShadow, &shadowHitObject);
		ShadowFact += inShadow;
		//计算

	}
	return ShadowFact;
}
