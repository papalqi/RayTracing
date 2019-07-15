#include "RayCasting.h"



oocd::Vector RayCastingRender::Shader(
	const Vector& orig, const Vector& dir,
	const std::vector<std::unique_ptr<Object>>& objects,
	const std::vector<std::unique_ptr<Light>>& lights,
	uint32_t depth,
	bool test )
{

	Vector hitColor = options->backgroundColor;
	float tnear = kInfinity;
	Vector2D uv;
	uint32_t index = 0;
	Object* hitObject = nullptr;
	if (trace(orig, dir, objects, tnear, index, uv, &hitObject))
	{
		Vector hitPoint = orig + dir * tnear;
		Vector normal; // normal 
		Vector2D st; // st coordinates 
		hitObject->getSurfaceProperties(hitPoint, dir, index, uv, normal, st);
		Vector tmp = hitPoint;

		//环境光、高光
		Vector lightAmt = 0, specularColor = 0;
		float ShadowFact = 0;
		Vector shadowPointOrig = ((dir | normal) < 0) ?
			hitPoint + normal * options->bias :
			hitPoint - normal * options->bias;
		for (uint32_t i = 0; i < lights.size(); ++i)
		{
			//到灯光的方向
			Vector lightDir = lights[i]->position - hitPoint;
			// 距离的平方
			float lightDistance2 = (lightDir | lightDir);
			lightDir = (lightDir).GetSafeNormal();
			float LdotN = std::max(0.f, (lightDir | normal));
			Object* shadowHitObject = nullptr;
			float tNearShadow = kInfinity;
			// 判断是否是在阴影中并且还要是距离，其还不能是最近的
			bool inShadow = trace(shadowPointOrig, lightDir, objects, tNearShadow, index, uv, &shadowHitObject) &&
				tNearShadow * tNearShadow < lightDistance2;
			ShadowFact += inShadow / lights.size();
			//计算
			lightAmt += (1 - inShadow) * lights[i]->intensity * LdotN;
			Vector reflectionDirection = reflect(-lightDir, normal);
			//for debug
			auto Tema = std::max(0.f, ((-reflectionDirection) | dir));
			specularColor += powf(Tema, hitObject->specularExponent) * lights[i]->intensity;
		}
		//phone模型为：环境光+漫反射光+高光
		Color AmbineLight = 0.1 * lightAmt * (1 - ShadowFact);
		Color DiffuseLight = (1 - ShadowFact) * hitObject->evalDiffuseColor(st) * hitObject->Kd;
		Color SpecularLight = specularColor * hitObject->Ks;
		hitColor = AmbineLight + DiffuseLight + specularColor;


	}


	return hitColor;
}