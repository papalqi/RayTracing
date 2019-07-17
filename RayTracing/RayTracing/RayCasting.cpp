#include "RayCasting.h"

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
			BaseLighting* p = lights[l].get();
			{
				BaseLighting* light = p;
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

	}
	return hitColor;
}
