#include "WhittedRender.h"



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
//			//环境光、高光
//			Vector lightAmt = 0, specularColor = 0;
//			float ShadowFact = 0;
//			Vector shadowPointOrig = ((dir | normal) < 0) ?
//				hitPoint + normal * options->bias :
//				hitPoint - normal * options->bias;
//			for (uint32_t i = 0; i < lights.size(); ++i)
//			{
//				//到灯光的方向
//				Vector lightDir = lights[i]->position - hitPoint;
//				// 距离的平方
//				float lightDistance2 = (lightDir | lightDir);
//				lightDir = (lightDir).GetSafeNormal();
//				float LdotN = std::max(0.f, (lightDir | normal));
//				Object* shadowHitObject = nullptr;
//				float tNearShadow = kInfinity;
//				// 判断是否是在阴影中并且还要是距离，其还不能是最近的
//				bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, uv, &shadowHitObject) &&
//					tNearShadow * tNearShadow < lightDistance2;
//				ShadowFact += inShadow / lights.size();
//				//计算
//				lightAmt += (1.f- inShadow) * lights[i]->intensity * LdotN;
//				Vector reflectionDirection = RTMath::reflect(-lightDir, normal);
//				//for debug
//				auto Tema = std::max(0.f, ((-reflectionDirection) | dir));
//				specularColor += powf(Tema, hitObject->specularExponent) * lights[i]->intensity;
//			}
//			//phone模型为：环境光+漫反射光+高光
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