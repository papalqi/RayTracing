#include "render.h"
#include "RayTracingDifine.h"
#include <opencv2\opencv.hpp>
using namespace  cv;
Mat colorim(RTHeight, RTWidth, CV_8UC3);
bool Render::trace(const Vector &orig, const Vector &dir, const std::vector<std::unique_ptr<Object>> &objects,
	float &tNear, uint32_t &index, Vector2D &uv, Object **hitObject)
{
	*hitObject = nullptr;
	for (uint32_t k = 0; k < objects.size(); ++k) {
		float tNearK = kInfinity;
		uint32_t indexK;
		Vector2D uvK;
		if (objects[k]->intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear)
		{
			*hitObject = objects[k].get();
			tNear = tNearK;
			index = indexK;
			uv = uvK;
		}
	}
	return (*hitObject != nullptr);
}

bool Render::FindNearest(const Ray p_ray, float& tNear, Object** hitObject)
{

	bool is_light = false;
	int in_out = 0;
	tNear = kInfinity;
	for (int s = 0; s < objects.size(); s++)
	{
		Object* pr = objects[s].get();
		int res;
		if (res = pr->intersect(p_ray, tNear))
		{
			*hitObject = pr;
			in_out = res;
		}
	}

	if (tNear > kInfinity - 1)//没有相加的物体返回0
		return 0;
	else //相交了的话反映是内部还是外部
		return in_out;
	//*hitObject = nullptr;
	//bool IsFind=false;
	//for (uint32_t k = 0; k < objects.size(); ++k) 
	//{
	//	float tNearK = kInfinity;
	//	uint32_t indexK;
	//	Vector2D uvK;
	//	if (objects[k]->intersect(p_ray, tNearK) && tNearK < tNear)
	//	{
	//		*hitObject = objects[k].get();
	//		tNear = tNearK;
	//		IsFind = true;
	//	}
	//}
	//return IsFind;

}


float Render::GetShadowFact(Light* mlt, Vector position, Vector& p_Dir)
{
	float ShadowFact = 0.0;
	//Light* prim = 0;
	//if (mlt->LightType== Shpere)
	//{
	//	Sphere* light = (Sphere*)p_Light;
	//	Vector O = light->getCentre();
	//	p_Dir = O - p_pi;
	//	NORMALIZE(p_Dir);
	//	double R = light->getRadius();
	//	Vector dir = O - p_pi;
	//	double dist = LENGTH(dir);
	//	NORMALIZE(dir);
	//	if (FindNearest(Ray(p_pi + dir * EPS, dir), dist, prim))
	//		if (prim == p_Light)
	//			shade += 1.0;
	//}
	//else if (mlt->LightType == Rect)
	//{
	//	shade = 0.0;
	//	Box* light = (Box*)p_Light;
	//	Vector P = light->getPos();
	//	Vector size = light->getSize();
	//	p_Dir = P + 0.5 * size - p_pi;
	//	NORMALIZE(p_Dir);
	//	int n = (int)floor(sqrt(p_Sample + 0.5));
	//	for (int i = 0; i < n; ++i)
	//		for (int j = 0; j < n; ++j)
	//		{
	//			Vector d = Vector(size.x / n, 0.0, size.z / n);
	//			Vector pos = P + Vector(d.x * (i + (double)rand() / RAND_MAX), 0.0, d.z * (j + (double)rand() / RAND_MAX));
	//			Vector dir = pos - p_pi;
	//			double dist = LENGTH(dir);
	//			NORMALIZE(dir);
	//			if (FindNearest(Ray(p_pi + dir * EPS, dir), dist, prim))
	//				if (prim == p_Light)
	//					ShadowFact += 1.0 / (n * n);
	//		}
	//}
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

void Render::Rendering(  const std::vector<std::unique_ptr<Object>> &objects, const std::vector<std::unique_ptr<Light>> &lights)
{

	//设置输出大小
	Camera c = Camera();
	for (int y = 0; y < RTHeight; y++)
	{
		for (int x = 0; x < RTWidth; x++)
		{
			Color col(0, 0, 0);
			Vector dir = c.getDir(x, y);
			(dir).Normalize();
			Ray r(c.getEye(), dir);
		
			col = Shader(r, MaxBound);
			int red = (int)(col.X * 256);
			int green = (int)(col.Y * 256);
			int blue = (int)(col.Z * 256);
			if (red > 255) red = 255;
			if (green > 255) green = 255;
			if (blue > 255) blue = 255;
			colorim.at<Vec3b>(RTHeight - y - 1, x) = Vec3b(blue, green, red);
			//printf("rendering %dth row...\n", y + 1);
		

		
			//cv::waitKey(100000);
			/*if (col.X > 1) col.X = 1;
			if (col.Y > 1) col.X = 1;
			if (col.Z> 1) col.X = 1;
			*(pix++) = col;*/

		}
	

	}

	namedWindow("Lu", WINDOW_NORMAL);
	cv::imshow("test", colorim);
	waitKey(0);


}



//这里主要计算的是是否有
Color Render::GetDiffuseColor(material m_material, Light* m_light, Vector normal, Vector position,Object * obj)
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


Color Render::GeSpecularColor(material m_material, Light* m_light, Vector normal, Vector position,Object* obj, Vector Dir)
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

void Render::OutputImage(string paths, string name, Vector * framebuffer)
{
	std::ofstream ofs;
	ofs.open(paths+ name);
	ofs << "P6\n" << options->width << " " << options->height << "\n255\n";
	for (uint32_t i = 0; i < options->height * options->width; ++i)
	{
		char r = (char)(255 * RTMath::clamp(0, 1, framebuffer[i].X));
		char g = (char)(255 * RTMath::clamp(0, 1, framebuffer[i].Y));
		char b = (char)(255 * RTMath::clamp(0, 1, framebuffer[i].Z));
		ofs << g << b << r;
	}

	ofs.close();
}

void Render::InitScene()
{

	/*Sphere* sph1 = new Sphere(Vector(-1, 0, -12), 2);
	sph1->materialType = DIFFUSE_AND_GLOSSY;
	sph1->diffuseColor = Vector(0.6, 0.7, 0.8);
	Sphere* sph2 = new Sphere(Vector(0.5, -0.5, -8), 1.5);
	sph2->ior = 1.5;
	sph2->materialType = REFLECTION_AND_REFRACTION;

	objects.push_back(std::unique_ptr<Sphere>(sph1));
	objects.push_back(std::unique_ptr<Sphere>(sph2));
	Vector verts[4] = { {-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16} };
	uint32_t vertIndex[6] = { 0, 1, 3, 1, 2, 3 };
	Vector2D st[4] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
	MeshTriangle* mesh = new MeshTriangle(verts, vertIndex, 2, st);
	mesh->materialType = DIFFUSE_AND_GLOSSY;

	objects.push_back(std::unique_ptr<MeshTriangle>(mesh));

*/
	lights.push_back(std::unique_ptr<Light>(new Light(Vector(0,1,0), 1)));
	lights[0]->LightType = point;
	//lights.push_back(std::unique_ptr<Light>(new Light(Vector(0, -1, 0), 1)));
	//lights[1]->LightType = point;

	objects.resize(9);

	// ground plane
	objects[0] = std::unique_ptr<Object>(new PlaneObj( Vector( 0, 1, 0 ), 4.4f ));
	objects[0]->setName( "plane" );
	objects[0]->getMaterial()->setReflection( 0.6f );
	objects[0]->getMaterial()->setSpecular(0.8f);
	objects[0]->getMaterial()->setRefraction( 0.0f );
	objects[0]->getMaterial()->setDiffuse( 1.0f );
	objects[0]->getMaterial()->setColor( Color( 0.3f, 0.3f, 0.3f ) );

	objects[1] = std::unique_ptr<Object>(new PlaneObj( Vector( 0, 0, 0 ), 6.0f ));
	objects[1]->setName( "backplane2" );
	objects[1]->getMaterial()->setSpecular(0.8f);
	objects[1]->getMaterial()->setReflection( 0.0f );
	objects[1]->getMaterial()->setRefraction( 0.0f );
	objects[1]->getMaterial()->setDiffuse( 1.0f );
	objects[1]->getMaterial()->setColor( Color( 0.0f, 0.2f, 0.0f ) );

	// small sphere
	objects[2] = std::unique_ptr<Object>(new Sphere( Vector( -5, -2.7f, 9 ), 1.5 ));
	objects[2]->setName( "small sphere" );
	objects[2]->getMaterial()->setReflection( 0.8f );
	objects[2]->getMaterial()->setRefraction( 0.0f );
	objects[2]->getMaterial()->setRefr_Rate( 1.3f );
	objects[2]->getMaterial()->setDiffuse( 0.1f );
	objects[2]->getMaterial()->setColor( Color( 0.7f, 0.7f, 1.0f ) );
	objects[2]->getMaterial()->setDiffRefl( 0.1f );
	
	// third sphere
	objects[3] = std::unique_ptr<Object>(new Sphere( Vector( 4, -3.2f, 7 ), 1 ));
	objects[3]->setName( "middle sphere" );
	objects[3]->getMaterial()->setReflection( 0.1f );
	objects[3]->getMaterial()->setRefraction( 0.9f );
	objects[3]->getMaterial()->setRefr_Rate( 1.5f );
	objects[3]->getMaterial()->setDiffuse( 0.1f );
	objects[3]->getMaterial()->setSpecular( 0.2f );
	objects[3]->getMaterial()->setColor( Color( 0.7f, 0.7f, 1.0f ) );
	objects[3]->getMaterial()->setDiffRefl( 0.1f );


	// back plane
	objects[4] = std::unique_ptr<Object>(new PlaneObj(Vector(0, 0, -1), 12.4f));
	objects[4]->setName("backplane");
	objects[4]->getMaterial()->setSpecular(0.4f);
	objects[4]->getMaterial()->setReflection(0.0f);
	objects[4]->getMaterial()->setRefraction(0.0f);
	objects[4]->getMaterial()->setDiffuse(1.0f);
	objects[4]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));
	//objects[4]->getMaterial()->setTexure(new Texture("textures/marble.tga"));
	objects[4]->getMaterial()->setTexRatio(3.0);
	
	objects[5] = std::unique_ptr<Object>(new PlaneObj(Vector(0, -1, 0), 5.2f));
	objects[5]->setName("ceiling");
	objects[5]->getMaterial()->setSpecular(0.8f);
	objects[5]->getMaterial()->setReflection(0.0f);
	objects[5]->getMaterial()->setRefraction(0.0f);
	objects[5]->getMaterial()->setDiffuse(1.0f);
	objects[5]->getMaterial()->setColor(Color(0.4f, 0.3f, 0.3f));

	objects[6] = std::unique_ptr<Object>(new PlaneObj(Vector(1, 0, 0), 7.0f));
	objects[6]->setName("right plane");
	objects[6]->getMaterial()->setSpecular(0.0f);
	objects[6]->getMaterial()->setReflection(0.0f);
	objects[6]->getMaterial()->setRefraction(0.0f);
	objects[6]->getMaterial()->setDiffuse(0.5f);
	objects[6]->getMaterial()->setColor(Color(0.2f, 0.0f, 0.0f));
	//objects[6]->getMaterial()->setTexure(new Texture("textures/wood.tga"));
	objects[6]->getMaterial()->setTexRatio(3.0);

	objects[7] = std::unique_ptr<Object>(new PlaneObj( Vector( -1, 0, 0), 7.0f));
	objects[7]->setName( "left plane" );
	objects[7]->getMaterial()->setSpecular(0.0f);
	objects[7]->getMaterial()->setReflection( 0.0f );
	objects[7]->getMaterial()->setRefraction( 0.0f );
	objects[7]->getMaterial()->setDiffuse( 0.8f );
	//object7[8]->getMaterial()->setColor( Color( 0.0f, 0.0f, 0.2f ) );
	objects[7]->getMaterial()->setColor((1.0 / 256)*Color(241.0f, 158.0f, 194.0f ));

	objects[8] = std::unique_ptr<Object>(new Sphere( Vector( -5.8, -3.0f, 5 ), 1.2 ));
	objects[8]->setName( "small sphere" );
	objects[8]->getMaterial()->setReflection( 0.0f );
	objects[8]->getMaterial()->setRefraction( 0.0f );
	objects[8]->getMaterial()->setRefr_Rate( 1.3f );
	objects[8]->getMaterial()->setDiffuse( 0.9f );
	objects[8]->getMaterial()->setColor( Color( 0.7f, 0.7f, 1.0f ) );
	//object8[9]->getMaterial()->setDiffRefl( 0.3f );
	//objects[8]->getMaterial()->setTexure(new Texture( "textures/marble.tga" ));





}
