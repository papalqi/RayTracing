#include "WhittedRender.h"
#include "RayTracingDifine.h"


// Vector WhittedRender::Shader(const Vector& orig, const Vector& dir, const std::vector<std::unique_ptr<Object>>& objects,
//	const std::vector<std::unique_ptr<Light>>& lights, uint32_t depth, bool test)
//{
//	if (depth > options->maxDepth)
//	{
//		return options->backgroundColor;
//	}
//
//	Vector hitColor = options->backgroundColor;
//	float tnear = kInfinity;
//	Vector2D uv;
//	uint32_t index = 0;
//	Object* hitObject = nullptr;
//	if (trace(orig, dir, objects, tnear, index, uv, &hitObject))
//	{
//		Vector hitPoint = orig + dir * tnear;
//		Vector normal; // normal 
//		Vector2D st; // st coordinates 
//		hitObject->getSurfaceProperties(hitPoint, dir, index, uv, normal, st);
//		Vector tmp = hitPoint;
//		switch (hitObject->materialType)
//		{
//		case REFLECTION_AND_REFRACTION:
//		{
//			Vector reflectionDirection = (RTMath::reflect(dir, normal)).GetSafeNormal();
//			Vector refractionDirection = (RTMath::refract(dir, normal, hitObject->ior)).GetSafeNormal();
//			Vector reflectionRayOrig = ((reflectionDirection | normal) < 0) ?
//				hitPoint - normal * options->bias :
//				hitPoint + normal * options->bias;
//			Vector refractionRayOrig = ((refractionDirection | normal) < 0) ?
//				hitPoint - normal * options->bias :
//				hitPoint + normal * options->bias;
//			Vector reflectionColor = Shader(reflectionRayOrig, reflectionDirection, objects, lights, depth + 1, 1);
//			Vector refractionColor = Shader(refractionRayOrig, refractionDirection, objects, lights, depth + 1, 1);
//			float kr;
//			RTMath::fresnel(dir, normal, hitObject->ior, kr);
//			hitColor = reflectionColor * kr + refractionColor * (1 - kr);
//			break;
//		}
//		case REFLECTION:
//		{
//			float kr;
//			RTMath::fresnel(dir, normal, hitObject->ior, kr);
//			Vector reflectionDirection = RTMath::reflect(dir, normal);
//			Vector reflectionRayOrig = ((reflectionDirection | normal) < 0) ?
//				hitPoint + normal * options->bias :
//				hitPoint - normal * options->bias;
//			hitColor = Shader(reflectionRayOrig, reflectionDirection, objects, lights, depth + 1) * kr;
//			break;
//		}
//		default:
//		{
//			//�����⡢�߹�
//			Vector lightAmt = 0, specularColor = 0;
//			float ShadowFact = 0;
//			Vector shadowPointOrig = ((dir | normal) < 0) ?
//				hitPoint + normal * options->bias :
//				hitPoint - normal * options->bias;
//			for (uint32_t i = 0; i < lights.size(); ++i)
//			{
//				//���ƹ�ķ���
//				Vector lightDir = lights[i]->position - hitPoint;
//				// �����ƽ��
//				float lightDistance2 = (lightDir | lightDir);
//				lightDir = (lightDir).GetSafeNormal();
//				float LdotN = std::max(0.f, (lightDir | normal));
//				Object* shadowHitObject = nullptr;
//				float tNearShadow = kInfinity;
//				// �ж��Ƿ�������Ӱ�в��һ�Ҫ�Ǿ��룬�仹�����������
//				bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, uv, &shadowHitObject) &&
//					tNearShadow * tNearShadow < lightDistance2;
//				ShadowFact += inShadow / lights.size();
//				//����
//				lightAmt += (1.f- inShadow) * lights[i]->intensity * LdotN;
//				Vector reflectionDirection = RTMath::reflect(-lightDir, normal);
//				//for debug
//				auto Tema = std::max(0.f, ((-reflectionDirection) | dir));
//				specularColor += powf(Tema, hitObject->specularExponent) * lights[i]->intensity;
//			}
//			//phoneģ��Ϊ��������+�������+�߹�
//			Color AmbineLight = 0.1f * lightAmt * (1 - ShadowFact);
//			Color DiffuseLight = (1 - ShadowFact) * hitObject->evalDiffuseColor(st) * hitObject->Kd;
//			Color SpecularLight = specularColor * hitObject->Ks;
//			hitColor = AmbineLight + DiffuseLight + specularColor;
//			break;
//		}
//		}
//	}
//
//	return hitColor;
//}

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
		hitPoint = p_ray.origin() + p_ray.direction() * tnear;//hitPointΪ������������彻��ĵط�

		Color LightColor;
		for (int l = 0; l < lights.size(); ++l)
		{
			Light* p = lights[l].get();
			{
				Light* light = p;
				float shadow = 1.0;
				Vector L;
				shadow = GetShadowFact(light, hitPoint, L);
				Vector N = hitObject->getNormal(hitPoint);//������pi���ķ�����
				if (hitObject->getMaterial()->getDiffuse() > 0)//������
				{
					float dot = (L | N);
					if (dot > 0)
					{
						float diff = dot * hitObject->getMaterial()->getDiffuse() * shadow;
						hitColor += diff * light->intensity * hitObject->getColor(hitPoint);
					}
				}
				if (hitObject->getMaterial()->getSpecular() > 0)//�߹�
				{
					// point light source: sample once for specular highlight
					Vector V = p_ray.direction();//���߷���
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
		if (refl > 0.0)//���淴��
		{
			float drefl = hitObject->getMaterial()->getDiffRefl();
			Vector N = hitObject->getNormal(hitPoint);
			Vector R = p_ray.direction() - 2.0 * (p_ray.direction() | N) * N;
			R.Normalize();

			if (depth < WhittedMaxBound)//�⻬���淴��
			{

				double dist;

				Color rcol = Shader(Ray(hitPoint + options->bias * R, R), depth + 1);

				hitColor += refl * rcol * hitObject->getColor(hitPoint);
			}
		}
		//��������
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
	
	


		/*	Color rcol(0, 0, 0);
			double dist;
			Vector N;
			N = -hitObject->getNormal(hitPoint);*/
		/*	if (result < 0)
			{
				N = -1.0 * N;
			}*/
		//double tmp_Refr_rate = hitObject->getMaterial()->getRefr_Rate();
		//if (result == INPRIM) tmp_Refr_rate = 1.0;
		//double n = tmp_Refr_rate / p_Refr_Rate;//��������ڿ��������������
		//Vector V = p_ray.direction();
		//double cosi = -(V| N);
		//double sinr2 = (1.0 - DOT(V, N) * DOT(V, N)) / (n * n);
		//if (0.0 < sinr2 && sinr2 < 1.0)
		//{
		//	double cosr2 = 1.0 - sinr2;
		//	double sinr = sqrt(sinr2);
		//	double cosr = sqrt(cosr2);
		//	vector3 T = (V * (1 / n)) + (cosi / n - sqrt(cosr2)) * N;

		//	Shader(Ray(hitPoint + T * 0.1, T), rcol, depth + 1, tmp_Refr_rate, dist, p_Sample, p_SampleRange);

		//	Color absorbance = hitObject->getColor(pi) * 0.15 * -dist;
		//	Color transparency = Color(exp(absorbance.r), exp(absorbance.g), exp(absorbance.b));
		//	hitColor += rcol * transparency;
		//}
	}



	return hitColor;
}
