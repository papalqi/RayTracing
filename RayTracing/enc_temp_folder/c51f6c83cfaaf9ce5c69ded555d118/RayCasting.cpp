#include "RayCasting.h"



//oocd::Vector RayCastingRender::Shader(
//	const Vector& orig, const Vector& dir,
//	const std::vector<std::unique_ptr<Object>>& objects,
//	const std::vector<std::unique_ptr<Light>>& lights,
//	uint32_t depth,
//	bool test )
//{
//	//���Ƚ���ɫ��Ϊ������ɫ
//	Vector hitColor = options->backgroundColor;
//	//���߳���Ϊ����
//	float tnear = kInfinity;
//	Vector2D uv;
//	uint32_t index = 0;
//	Object* hitObject = nullptr;
//
//	if (trace(orig, dir, objects, tnear, index, uv, &hitObject))
//	{
//		Vector hitPoint = orig + dir * tnear;
//		Vector normal; // normal 
//		Vector2D st; // st coordinates 
//		hitObject->getSurfaceProperties(hitPoint, dir, index, uv, normal, st);
//		Vector tmp = hitPoint;
//
//		//�����⡢�߹�
//		Vector lightAmt = 0, specularColor = 0;
//		float ShadowFact = 0;
//		Vector shadowPointOrig = ((dir | normal) < 0) ?
//			hitPoint + normal * options->bias :
//			hitPoint - normal * options->bias;
//		for (uint32_t i = 0; i < lights.size(); ++i)
//		{
//			//���ƹ�ķ���
//			Vector lightDir = lights[i]->position - hitPoint;
//			// �����ƽ��
//			float lightDistance2 = (lightDir | lightDir);
//			lightDir = (lightDir).GetSafeNormal();
//			float LdotN = std::max(0.f, (lightDir | normal));
//			Object* shadowHitObject = nullptr;
//			float tNearShadow = kInfinity;
//			// �ж��Ƿ�������Ӱ�в��һ�Ҫ�Ǿ��룬�仹�����������
//			bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, uv, &shadowHitObject) &&
//				tNearShadow * tNearShadow < lightDistance2;
//			ShadowFact += inShadow / lights.size();
//			//����
//			lightAmt += (1.0f- inShadow) * lights[i]->intensity * LdotN;
//			Vector reflectionDirection = RTMath::reflect(-lightDir, normal);
//			//for debug
//			auto Tema = std::max(0.f, ((-reflectionDirection) | dir));
//			specularColor += powf(Tema, hitObject->specularExponent) * lights[i]->intensity;
//		}
//		//phoneģ��Ϊ��������+�������+�߹�
//		Color AmbineLight = 0.1 * lightAmt * (1 - ShadowFact);
//		Color DiffuseLight = (1 - ShadowFact) * hitObject->evalDiffuseColor(st) * hitObject->Kd;
//		Color SpecularLight = specularColor * hitObject->Ks;
//		hitColor = AmbineLight + DiffuseLight + specularColor;
//
//
//	}
//
//	return hitColor;
//}

oocd::Vector RayCastingRender::Shader(const Ray p_ray, uint32_t depth, bool test /*= false*/)
{
	//����raycasting ����Ҫbound

	//���Ƚ���ɫ��Ϊ������ɫ
	Vector hitColor = options->backgroundColor;
	//���߳���Ϊ����
	float tnear = kInfinity;
	Object* hitObject = nullptr;

	if (FindNearest(p_ray, tnear, &hitObject))
	{
		hitColor = 0;
		Vector hitPoint = p_ray.origin() + p_ray.direction() * tnear;//hitPointΪ������������彻��ĵط�

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
					float dot = (L| N);
					if (dot > 0)
					{
						float diff = dot * hitObject->getMaterial()->getDiffuse() * shadow;
						hitColor +=  diff * light->intensity * hitObject->getColor(hitPoint);
					}
				}
				if (hitObject->getMaterial()->getSpecular() > 0)//�߹�
				{
					// point light source: sample once for specular highlight
					Vector V = p_ray.direction();//���߷���
					Vector R = L - 2.0 * (L| N) * N;
					float dot = (V|R);
					if (dot > 0)
					{
						float spec = pow(dot, 20) * hitObject->getMaterial()->getSpecular() * shadow;
						hitColor += spec * light->intensity;
					}
				}
				
			}
		
		}
		

	/*	float refl = hitObject->getMaterial()->getReflection();*/
		//if (refl > 0.0)//���淴��
		//{
		//	float drefl = hitObject->getMaterial()->getDiffRefl();
		//	Vector N = hitObject->getNormal(hitPoint);
		//	Vector R = p_ray.direction() - 2.0 * (p_ray.direction()| N) * N;
		//	if ((drefl > 0) && (depth < 2))//�ֲھ��淴��
		//	{
		//		Vector component1 = Vector(R.Z, 0.0, -R.X);
		//		Vector component2 = R^(component1);
		//		(component2).Normalize();


		//		hitColor += refl  * hitObject->getColor(hitPoint);
		//		int num = 20;
		//		for (int i = 0; i < num; i++)
		//		{
		//			/*	float x0, y0;
		//				while (1)
		//				{
		//					x0 = 2.0 * (float)rand() / RAND_MAX - 1.0;
		//					y0 = 2.0 * (float)rand() / RAND_MAX - 1.0;
		//					if ((x0 * x0 + y0 * y0) < 1.0)
		//						break;
		//				}
		//				x0 *= drefl;
		//				y0 *= drefl;
		//				Vector newR = R + x0*component1 + y0*component2 ;
		//				(newR).Normalize();
		//				float dist;
		//				Color rcol(0, 0, 0);
		//				Runtracer(Ray(hitPoint + newR * EPS, newR), rcol, depth + 1, p_Refr_Rate, dist, p_Sample, p_SampleRange);*/

		//		}
		//	}

		//}


	}
	return hitColor;

	/*for (int l = 0; l < lights.size(); ++l)
	{
		Light* p = lights[l].get();

	}


*/
	//{
	//	hitColor = 0;
	//	Vector hitPoint = p_ray.origin() + p_ray.direction() * tnear;
	//	Vector normal= hitObject->getNormal(hitPoint); // normal 
	//	Vector2D st; // st coordinates 
	//
	//	Vector tmp = hitPoint;

	//	//�����⡢�߹�
	//	Vector lightAmt = 0, specularColor = 0;
	//	float ShadowFact = 0;
	//	Vector shadowPointOrig = ((p_ray.direction() | normal) < 0) ?
	//		hitPoint + normal * options->bias :
	//		hitPoint - normal * options->bias;
	//	for (uint32_t i = 0; i < lights.size(); ++i)
	//	{
	//		//���ƹ�ķ���
	//		Vector lightDir = lights[i]->position - hitPoint;
	//		// �����ƽ��
	//		float lightDistance2 = (lightDir | lightDir);
	//		lightDir = (lightDir).GetSafeNormal();

	//		float LdotN = std::max(0.f, (lightDir | normal));
	//		Object* shadowHitObject = nullptr;
	//		float tNearShadow = kInfinity;
	//		// �ж��Ƿ�������Ӱ�в��һ�Ҫ�Ǿ��룬�仹�����������
	//		
	//		bool inShadow = FindNearest(p_ray, tNearShadow, &shadowHitObject) &&
	//			tNearShadow * tNearShadow < lightDistance2;
	//		//��Ӱ����
	//		ShadowFact += inShadow / lights.size();
	//		//����
	//		//lightAmt += (1.0f - inShadow) * lights[i]->intensity * LdotN;
	//		hitColor += GetDiffuseColor(*hitObject->getMaterial(), lights[i].get(), normal, hitPoint, hitObject);
	//		hitColor +=   GeSpecularColor(*hitObject->getMaterial(), lights[i].get(), normal, hitPoint, hitObject, p_ray.direction());
	//	}
		//phoneģ��Ϊ��������+�������+�߹�
	/*	Color AmbineLight = 0.1 * lightAmt * (1 - ShadowFact);
		Color DiffuseLight = (1 - ShadowFact) * hitObject->evalDiffuseColor(st) * hitObject->Kd;
		Color SpecularLight = specularColor * hitObject->Ks;
		hitColor = AmbineLight + DiffuseLight + specularColor;*/


	


}
