#include "RayCasting.h"

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
			BaseLighting* p = lights[l].get();
			{
				BaseLighting* light = p;
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
