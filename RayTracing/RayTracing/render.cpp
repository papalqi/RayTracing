#include "render.h"
#include "RayTracingDifine.h"
#include "ShpereLighting.h"
#include "RectLighting.h"
#include <opencv2\opencv.hpp>
using namespace  cv;
Mat colorim(RTHeight, RTWidth, CV_8UC3);


void Render::SetSphere(Vector position, float rad, material::Refl_t mtype, Vector ks, Vector ka, Vector kd, float shiness, string name)
{
	auto s=std::shared_ptr<Object>(new Sphere(position, rad));
	s->m_Material.BRDFType = mtype;
	s->m_Material.ks = ks;
	s->m_Material.ka = ka;
	s->m_Material.kd = kd;
	s->m_Material.shiness = shiness;
	s->m_Material.emission = 0;
	s->setName(name);
	objects.push_back(s);
}

void Render::SetPlane(Vector position, Vector  normal, Vector DX, Vector DY,float rad, material::Refl_t mtype, Vector ks, Vector ka, Vector kd, float shiness, string name)
{
	auto s = std::shared_ptr<Object>(new PlaneObj(position, normal,DX,DY));
	s->m_Material.BRDFType = mtype;
	s->m_Material.ks = ks;
	s->m_Material.ka = ka;
	s->m_Material.kd = kd;
	s->m_Material.shiness = shiness;
	s->m_Material.emission = 0;
	s->setName(name);
	objects.push_back(s);
}

bool Render::FindNearest(const Ray p_ray, float& tNear, Object** hitObject)
{

	bool isfind = false;
	int in_out = 0;
	tNear = kInfinity;
	float fasfa = kInfinity;
	for (int s = 0; s < objects.size(); s++)
	{
		Object* pr = objects[s].get();
	
		if ( pr->intersect(p_ray, tNear))
		{
			if (tNear < fasfa)
			{
				isfind = true;
				fasfa = tNear;
				*hitObject = pr;
			}
		
		
		}
	}

	if (tNear > kInfinity - 1)//没有相加的物体返回0
		return false;
	else //相交了的话反映是内部还是外部
		return isfind;


}
float Render::GetShadowFact(BaseLighting* mlt, Vector position, Vector& p_Dir, float p_Sample, float p_SampleRange)
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
		if (FindNearest(Ray(position , dir), dist, &prim))
			if (prim == mlt)
				ShadowFact += 1.0;
	}
	if (mlt->LightType == LightClass::Rect)
	{
		RectLighting* light = (RectLighting*)mlt;
		Vector P = light->position;
		Vector size{ light->dx.Size() ,light->dy.Size() ,0} ;
		p_Dir = P + 0.5 * size - position;
		(p_Dir).Normalize();
		//采样个数
		//int n = (int)floor(sqrt(p_Sample + 0.5));


		int n = (int)floor(sqrt(p_Sample + 0.5));
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j)
			{
				Vector d = Vector(size.X / n, 0.0, size.Z/ n);
				Vector pos = P + Vector(d.X * (i + (double)rand() / RAND_MAX), 0.0, d.Z * (j + (double)rand() / RAND_MAX));
				Vector dir = pos - position;
				float dist = (dir).Size();
				(dir).Normalize();
				Object* a;
				if (FindNearest(Ray(position + dir * EPS, dir), dist, &a))
					if (a == light)
						ShadowFact += 1.0 / (n * n);
			}



		//for (int i = 0; i < lightSamples; i++)
		//{
		//	double sx = (double)rand() / RAND_MAX;
		//	double sy = (double)rand() / RAND_MAX * (1.0 - sx);

	

		//	auto  lightOrigin = (light->position /*+ light->dx / 2 + light->dy / 2*/) + sx*light-> dx + sy*light-> dy ;
		//	auto  r = lightOrigin - position;
		//	float rr = (r).Size();
		//	Ray shadowRay = Ray(position-0.1*r.GetSafeNormal(), r);
		//	Object* ShadowObj;
		//	float aa;
		//	if (FindNearest(shadowRay, aa, &ShadowObj))
		//		if (ShadowObj == mlt)
		//			ShadowFact += 1.0 / (lightSamples);
		//	//ShadowFact += 1.0 / (lightSamples);
		//	//{
		//	//	if (aa <= rr)
		//	//	{
		//	//		ShadowFact += 1.0 / (lightSamples);

		//	//	}
		//	//	/*if (ShadowObj == mlt)
		//	//	{
		//	//		ShadowFact += 1.0 / (lightSamples);

		//	//	}
		//	//	else
		//	//	{
		//	//		ShadowFact += 0;
		//	//	}*/
		//	//}


		//
		//}


		//for (int i = 0; i < n; ++i)
		//	for (int j = 0; j < n; ++j)
		//	{
		//		Vector d = Vector(size.X / n, size.Y / n, size.Z / n);
		//		Vector pos = P + Vector(d.X * (i + (double)rand() / RAND_MAX), 0.0, d.Z * (j + (double)rand() / RAND_MAX));
		//		Vector dir = pos - position;
		//		float dist = (dir).Size();
		//		Object* prim;
		//		(dir).Normalize();
		//		if (FindNearest(Ray(position/* + 0.1* dir*/ /** options->bias*/, dir), dist, &prim))
		//			if (prim == mlt)
		//				ShadowFact += 1.0 / (n * n);
		//			else
		//			{
		//				ShadowFact += 0;
		//			}
		//	}
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

float Render::GetShadowFact(BaseLighting* mlt, Vector position, Vector& p_Dir)
{
	float ShadowFact = 0.0;
	//只接受点光源
	if (mlt->LightType == point)
	{
		p_Dir = mlt->position - position;
		p_Dir.Normalize();
		position = position +  p_Dir;
		float tNearShadow = kInfinity;
		// 判断是否是在阴影中并且还要是距离，其还不能是最近的
		Object *shadowHitObject;
		bool inShadow = FindNearest(Ray(position, p_Dir), tNearShadow, &shadowHitObject);
		ShadowFact += inShadow;
		//计算
	}
	return ShadowFact;
}

void Render::Rendering()
{
	namedWindow("Lu", WINDOW_NORMAL);
	//设置输出大小
	Camera c = Camera();
	Const_Max_Sample = SAMPLES;
	
	for (int y = 0; y < RTHeight; y++)
	{
		for (int x = 0; x < RTWidth; x++)
		{	
			
			Color col(0, 0, 0);
			float bias = 1.f/ MSAA;

			for(int i=0;i!=MSAA;i++)
				for (int j = 0; j != MSAA; j++)
			{
				Vector dir = c.getDir(x+ i* bias, y+ j*bias);
				(dir).Normalize();
				Ray r(c.getEye(), dir);

				col+= Shader(r, 0)/ (MSAA*MSAA);
			
				
			}
			int red = (int)(col.X * 256);
			int green = (int)(col.Y * 256);
			int blue = (int)(col.Z * 256);
			if (red > 255) red = 255;
			if (green > 255) green = 255;
			if (blue > 255) blue = 255;
			colorim.at<Vec3b>(RTHeight - y - 1, x) = Vec3b(blue, green, red);
		}
		cv::imshow("test", colorim);
		cv::waitKey(1);

	}
	waitKey(0);
}



Color Render::GetDirectColor(Ray p_ray, BaseLighting* light, Object* hitObject, Vector hitPoint, Vector normal)
{

	Color hitColor = 0;
	material mal = hitObject->m_Material;
	Vector ks = mal.ks;
	Vector kd = mal.kd;
	Vector ka = mal.ka;
	Vector emission = light->m_Material. emission;
	Vector shadowDir= light->position - hitPoint;
	Object* ShadowObject=nullptr;
	float dist;
	Vector s = shadowDir.GetSafeNormal();
	//如果最近的这个东西是灯本身
	//FindNearest(Ray(hitPoint + 0.01 * s, s), dist, &ShadowObject);
	//if (ShadowObject == nullptr)
	//	ShadowObject = light;

	//if (ShadowObject->getName()!=light->getName()) return hitColor;
	if (light->LightType == point)
	{
		

			if (kd != Vector::ZeroVector)
			{
				float mDots = (s| normal);
				if (mDots > 0.0) hitColor += mDots * kd * emission / PI;
			}
			if (ks != Vector::ZeroVector)
			{
				Vector v = -p_ray.direction();
				Vector h = (s + v).GetSafeNormal();
				float mDotH = (h |normal);
				if (mDotH > 0.0)

				{
					hitColor += pow(mDotH, mal.shiness) * ks
						* emission
						* (mal.shiness + 1) / (2 * PI);
				}
			
			}
	}
	else if (light->LightType == LightClass::Rect)
	{
		RectLighting* recl = (RectLighting*)light;
		float LightSampleRate =1.f/ lightSamples;
		for (int i = 0; i < lightSamples; i++)
		{
			float sx = (double)rand() / RAND_MAX;
			float sy = (double)rand() / RAND_MAX * (1.0 - sx);
			Vector lightOrigin = recl->position + sx*recl-> dx + sy*recl->dy  ;
			Vector r = lightOrigin - hitPoint;
			float rr = (r).Size();
			Ray shadowRay = Ray(hitPoint, r);

			if (FindNearest(shadowRay, dist, &ShadowObject))
			{

			/*	if (ShadowObject->getName() != light->getName())
				{
					return hitColor;
				}*/


				s = r.GetSafeNormal();
				double cosThetaIn = Math::Max((normal | s), 0.0f);
				double cosThetaOut = Math::Max(((-s) | normal), 0.0f);
				double geoFactor = cosThetaIn * cosThetaOut / (rr * rr);

				Vector intensity = geoFactor * recl->area * emission * LightSampleRate;
				//Vector intensity{ 1,1,1 };
				//Vec3 intensity =  emission * rate;

				if (kd != Vector::ZeroVector)
				{
					//calculate the diffuse color
					float mDots = (s | normal);
					if (mDots > 0.0) hitColor += mDots * kd
						* intensity / PI;
				}

				if (ks != Vector::ZeroVector)
				{
					//calculate the specular color
					Vector v = -p_ray.direction();
					Vector h = (s + v).GetSafeNormal();
					float mDotH = (h | normal);
					if (mDotH > 0.0) hitColor += pow(mDotH, mal.shiness) * ks
						* intensity
						* (mal.shiness + 1) / (2 * PI);
				}
					//	ShadowObject = light;
			}
			//shadowRay.tMax = rr;
			
		}


	}
	return hitColor;


	//float shadow = 1.0;
	//Vector L;
	//shadow = GetShadowFact(light, hitPoint, L, SAMPLES, SAMPLES);
	//Vector N = hitObject->getNormal(hitPoint);//物体在pi处的法向量
	//if (hitObject->getMaterial()->getDiffuse() > 0)//漫反射
	//{
	//	float dot = (L | N);
	//	if (dot > 0)
	//	{
	//		float diff = dot * hitObject->getMaterial()->getDiffuse() * shadow;
	//		hitColor += diff * light->intensity * hitObject->getColor(hitPoint);
	//	}
	//}
	//if (hitObject->getMaterial()->getSpecular() > 0)//高光
	//{
	//	// point light source: sample once for specular highlight
	//	Vector V = p_ray.direction().GetSafeNormal();//光线方向
	//	Vector R = (L - 2.0 * (L | N) * N).GetSafeNormal();
	//	float dot = (V | R);
	//	if (dot > 0)
	//	{
	//		float spec = pow(dot, 20) * hitObject->getMaterial()->getSpecular() * shadow;
	//		hitColor += spec * light->intensity;
	//	}
	//}

}

//这里主要计算的是是否有
Color Render::GetDiffuseColor(material m_material, BaseLighting* m_light, Vector normal, Vector position,Object * obj)
{
	;
	Color DiffuseColor(0,0,0);
	if (m_light->LightType == point)
	{
		auto LightDir = position - m_light->position;
		LightDir.Normalize();
		float Dot = normal | LightDir;
		if (Dot > 0)
		{
			float diff = Dot * m_material.getDiffuse() ;
			DiffuseColor += diff * m_light->intensity * obj->getColor(position);
		}
	}
	return DiffuseColor;
}


Color Render::GeSpecularColor(material m_material, BaseLighting* m_light, Vector normal, Vector position,Object* obj, Vector Dir)
{
	Color SpecularColor(0, 0, 0);

	if (m_light->LightType == point)
	{
		Vector V = Dir;//光线方向
		Vector L = position - m_light->position;
		L.Normalize();
		Vector R = L - 2.0 * (L| normal) * normal;
		float dot = (V| R);
		if (dot > 0)
		{
			float spec = pow(dot, 20) * obj->getMaterial()->getSpecular() ;
			SpecularColor += spec * m_light->intensity;
		}
	}
	return SpecularColor;
}



void Render::InitScene()
{

	SetSphere(Vector(0, -1, 0), 0.5f, material::DIFF, { 0.26,0.18,0.06 }, { 1,0,0 }, { .70,0.27,0.08 }, 12.8, "sh1");

	//SetSphere(Vector(-5, -2.7f, 9), 1.5, material::DIFF, { 0.26,0.18,0.06 }, { 1,0,0 }, { .70,0.27,0.08 }, 12.8);
	//SetSphere(Vector(4, -3.2f, 7), 1, material::DIFF, { 0.26,0.18,0.06 }, { 1,0,0 }, { .70,0.27,0.08 }, 12.8);

	//SetPlane(Vector((0, 0, -1)), 12.4f, material::DIFF, { 0.26,0.18,0.06 }, { 1,0,0 }, { .70,0.27,0.08 }, 12.8);
	//SetPlane(Vector((0, -1, 0)), 5.2f, material::DIFF, { 0.26,0.18,0.06 }, { 1,0,0 }, { .70,0.27,0.08 }, 12.8);
	//SetPlane(Vector((1, 0, 0)), 7.f, material::DIFF, { 0.26,0.18,0.06 }, { 1,0,0 }, { .70,0.27,0.08 }, 12.8);
	//SetPlane(Vector((-1, 0, 0)), 7.f, material::DIFF, { 0.26,0.18,0.06 }, { 1,0,0 }, { .70,0.27,0.08 }, 12.8);



	// ground plane
	//objects[0] = std::shared_ptr<Object>(new PlaneObj( Vector( 0, 1, 0 ), 4.4f ));
	//objects[0]->setName( "plane" );
	//objects[0]->getMaterial()->setReflection( 0.6f );
	//objects[0]->getMaterial()->setSpecular(0.8f);
	//objects[0]->getMaterial()->setRefraction( 0.0f );
	//objects[0]->getMaterial()->setDiffuse( 1.0f );
	//objects[0]->getMaterial()->setColor( Color( 0, 0.3f, 0.3f ) );
	//objects[0]->getMaterial()->BRDFType = material::SPEC;

	//objects[1] = std::shared_ptr<Object>(new PlaneObj( Vector( 0, 0, 0 ), 6.0f ));
	//objects[1]->setName( "backplane2" );
	//objects[1]->getMaterial()->setSpecular(0.8f);
	//objects[1]->getMaterial()->setReflection( 0.0f );
	//objects[1]->getMaterial()->setRefraction( 0.0f );
	//objects[1]->getMaterial()->setDiffuse( 1.0f );
	//objects[1]->getMaterial()->setColor( Color( 0.0f, 0.2f, 0.0f ) );
	//objects[1]->getMaterial()->BRDFType = material::SPEC;
	// small sphere
	//objects[2] = std::shared_ptr<Object>(new Sphere( Vector( -5, -2.7f, 9 ), 1.5 ));
	//objects[2]->setName( "small sphere" );
	//objects[2]->getMaterial()->setReflection( 0.8f );
	//objects[2]->getMaterial()->setRefraction( 0.0f );
	//objects[2]->getMaterial()->setRefr_Rate( 1.3f );
	//objects[2]->getMaterial()->setDiffuse( 0.1f );
	//objects[2]->getMaterial()->setColor( Color( 0.7f, 0.7f, 1.0f ) );
	//objects[2]->getMaterial()->setDiffRefl( 0.1f );
	//objects[2]->getMaterial()->BRDFType = material::SPEC;
	//// third sphere
	//objects[3] = std::shared_ptr<Object>(new Sphere( Vector( 4, -3.2f, 7 ), 1 ));
	//objects[3]->setName( "middle sphere" );
	//objects[3]->getMaterial()->setReflection( 0.1f );
	//objects[3]->getMaterial()->setRefraction( 0.9f );
	//objects[3]->getMaterial()->setRefr_Rate( 1.5f );
	//objects[3]->getMaterial()->setDiffuse( 0.1f );
	//objects[3]->getMaterial()->setSpecular( 0.2f );
	//objects[3]->getMaterial()->setColor( Color( 0.7f, 0.7f, 1.0f ) );
	//objects[3]->getMaterial()->setDiffRefl( 0.1f );
	
	
	// back plane
	//objects[4] = std::shared_ptr<Object>(new PlaneObj(Vector(0, 0, -1), 12.4f));
	//objects[4]->setName("backplane");
	//objects[4]->getMaterial()->setSpecular(0.4f);
	//objects[4]->getMaterial()->setReflection(0.0f);
	//objects[4]->getMaterial()->setRefraction(0.0f);
	//objects[4]->getMaterial()->setDiffuse(1.0f);
	//objects[4]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	////objects[4]->getMaterial()->setTexure(new Texture("textures/marble.tga"));
	//objects[4]->getMaterial()->setTexRatio(3.0);
	//objects[3]->getMaterial()->BRDFType = material::SPEC;
	//objects[5] = std::shared_ptr<Object>(new PlaneObj(Vector(0, -1, 0), 5.2f));
	//objects[5]->setName("ceiling");
	//objects[5]->getMaterial()->setSpecular(0.8f);
	//objects[5]->getMaterial()->setReflection(0.0f);
	//objects[5]->getMaterial()->setRefraction(0.0f);
	//objects[5]->getMaterial()->setDiffuse(1.0f);
	//objects[5]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	//objects[3]->getMaterial()->BRDFType = material::DIFF;
	//objects[6] = std::shared_ptr<Object>(new PlaneObj(Vector(1, 0, 0), 7.0f));
	//objects[6]->setName("right plane");
	//objects[6]->getMaterial()->setSpecular(0.0f);
	//objects[6]->getMaterial()->setReflection(0.0f);
	//objects[6]->getMaterial()->setRefraction(0.0f);
	//objects[6]->getMaterial()->setDiffuse(0.5f);
	//objects[6]->getMaterial()->setColor(Color(0.2f, 0.0f, 0.0f));
	////objects[6]->getMaterial()->setTexure(new Texture("textures/wood.tga"));
	//objects[6]->getMaterial()->setTexRatio(3.0);
	//objects[6]->getMaterial()->BRDFType = material::REFR;
	//objects[7] = std::shared_ptr<Object>(new PlaneObj( Vector( -1, 0, 0), 7.0f));
	//objects[7]->setName( "left plane" );
	//objects[7]->getMaterial()->setSpecular(0.0f);
	//objects[7]->getMaterial()->setReflection( 0.0f );
	//objects[7]->getMaterial()->setRefraction( 0.0f );
	//objects[7]->getMaterial()->setDiffuse( 0.8f );
	////object7[8]->getMaterial()->setColor( Color( 0.0f, 0.0f, 0.2f ) );
	//objects[7]->getMaterial()->setColor((1.0 / 256)*Color(241.0f, 158.0f, 194.0f ));
	//objects[7]->getMaterial()->BRDFType = material::REFR;
	//objects[8] = std::shared_ptr<Object>(new Sphere( Vector( -5.8, -3.0f, 5 ), 1.2 ));
	//objects[8]->setName( "small sphere" );
	//objects[8]->getMaterial()->setReflection( 0.0f );
	//objects[8]->getMaterial()->setRefraction( 0.0f );
	//objects[8]->getMaterial()->setRefr_Rate( 1.3f );
	//objects[8]->getMaterial()->setDiffuse( 0.9f );
	//objects[8]->getMaterial()->setColor( Color( 0.7f, 0.7f, 1.0f ) );
	//objects[8]->getMaterial()->BRDFType = material::REFR;
	//object8[9]->getMaterial()->setDiffRefl( 0.3f );
	//objects[8]->getMaterial()->setTexure(new Texture( "textures/marble.tga" ));
	AddLight(LightClass::Rect, { 1, 0, 0 }, { 1,1,1 }, 1, "rectl1");

	

	
}

void Render::AddLight(LightClass Ltype, Vector p, Color cor, float size, string name)
{
	if (Ltype == LightClass::point)
	{
		auto tem = std::shared_ptr<BaseLighting>(new BaseLighting(p, cor));
		tem->m_Material.emission = cor;
		tem->setName(name);
		tem->LightType = LightClass::point;
		objects.push_back(tem);
		
		lights.push_back(tem);
	}
	else if (Ltype == LightClass::Rect)
	{
		auto tem = std::shared_ptr<BaseLighting>(new RectLighting(p, { 0,-1,0 }, { 5,0,0 }, { 0,0,5}, cor));
		tem->m_Material.emission = cor;
		tem->setName(name);
		tem->LightType = LightClass::Rect;
	
		objects.push_back(tem);
		lights.push_back(tem);
	}
	else if(Ltype== LightClass::Shpere)
	{
		auto tem = std::shared_ptr<BaseLighting>(new BaseLighting(p, cor));
		tem->m_Material.emission = cor;
		tem->setName(name);
		tem->LightType = LightClass::Shpere;
		objects.push_back(tem);
		lights.push_back(tem);
	}

}
