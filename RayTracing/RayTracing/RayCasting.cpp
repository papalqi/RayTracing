#include "RayCasting.h"



//oocd::Vector RayCastingRender::Shader(
//	const Vector& orig, const Vector& dir,
//	const std::vector<std::unique_ptr<Object>>& objects,
//	const std::vector<std::unique_ptr<Light>>& lights,
//	uint32_t depth,
//	bool test )
//{
//	//首先将颜色搞为背景颜色
//	Vector hitColor = options->backgroundColor;
//	//光线长度为无穷
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
//		//环境光、高光
//		Vector lightAmt = 0, specularColor = 0;
//		float ShadowFact = 0;
//		Vector shadowPointOrig = ((dir | normal) < 0) ?
//			hitPoint + normal * options->bias :
//			hitPoint - normal * options->bias;
//		for (uint32_t i = 0; i < lights.size(); ++i)
//		{
//			//到灯光的方向
//			Vector lightDir = lights[i]->position - hitPoint;
//			// 距离的平方
//			float lightDistance2 = (lightDir | lightDir);
//			lightDir = (lightDir).GetSafeNormal();
//			float LdotN = std::max(0.f, (lightDir | normal));
//			Object* shadowHitObject = nullptr;
//			float tNearShadow = kInfinity;
//			// 判断是否是在阴影中并且还要是距离，其还不能是最近的
//			bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, uv, &shadowHitObject) &&
//				tNearShadow * tNearShadow < lightDistance2;
//			ShadowFact += inShadow / lights.size();
//			//计算
//			lightAmt += (1.0f- inShadow) * lights[i]->intensity * LdotN;
//			Vector reflectionDirection = RTMath::reflect(-lightDir, normal);
//			//for debug
//			auto Tema = std::max(0.f, ((-reflectionDirection) | dir));
//			specularColor += powf(Tema, hitObject->specularExponent) * lights[i]->intensity;
//		}
//		//phone模型为：环境光+漫反射光+高光
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
	//由于raycasting 不需要bound

	//首先将颜色搞为背景颜色
	Vector hitColor = options->backgroundColor;
	//光线长度为无穷
	float tnear = kInfinity;
	Object* hitObject = nullptr;

	if (FindNearest(p_ray, tnear, &hitObject))
	{
		hitColor = 0;
		Vector hitPoint = p_ray.origin() + p_ray.direction() * tnear;//hitPoint为光线与最近物体交汇的地方

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
					float dot = (L| N);
					if (dot > 0)
					{
						float diff = dot * hitObject->getMaterial()->getDiffuse() * shadow;
						hitColor +=  diff * light->intensity * hitObject->getColor(hitPoint);
					}
				}
				if (hitObject->getMaterial()->getSpecular() > 0)//高光
				{
					// point light source: sample once for specular highlight
					Vector V = p_ray.direction();//光线方向
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

	}
	return hitColor;
}
