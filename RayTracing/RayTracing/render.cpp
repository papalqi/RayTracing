#include "render.h"



bool Render::trace(const Vector &orig, const Vector &dir, const std::vector<std::unique_ptr<Object>> &objects, float &tNear, uint32_t &index, Vector2D &uv, Object **hitObject)
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



void Render::Rendering(  const std::vector<std::unique_ptr<Object>> &objects, const std::vector<std::unique_ptr<Light>> &lights)
{
	//设置输出大小
	Vector *framebuffer = new Vector[options->width * options->height];
	Vector *pix = framebuffer;
	float scale = tan(deg2rad(options->fov * 0.5));
	float imageAspectRatio = options->width / (float)options->height;
	Vector orig(0);
	for (uint32_t j = 0; j < options->height; ++j)
	{
		for (uint32_t i = 0; i < options->width; ++i)
		{
			//我们要把所有的i，j分配到NDC坐标当中
			float x = (2 * (i + 0.5) / (float)options->width - 1) * imageAspectRatio * scale;
			float y = (1 - 2 * (j + 0.5) / (float)options->height) * scale;
			Vector dir = (Vector(x, y, -1)).GetSafeNormal();
			*(pix++) = Shader(orig, dir, objects, lights, 0);
		}
	}
	OutputImage("./","out.ppm", framebuffer);
	delete[] framebuffer;
}




void Render::OutputImage(string paths, string name, Vector * framebuffer)
{
	std::ofstream ofs;
	ofs.open(paths+ name);
	ofs << "P6\n" << options->width << " " << options->height << "\n255\n";
	for (uint32_t i = 0; i < options->height * options->width; ++i)
	{
		char r = (char)(255 * clamp(0, 1, framebuffer[i].X));
		char g = (char)(255 * clamp(0, 1, framebuffer[i].Y));
		char b = (char)(255 * clamp(0, 1, framebuffer[i].Z));
		ofs << g << b << r;
	}

	ofs.close();
}

void Render::InitScene()
{

	Sphere* sph1 = new Sphere(Vector(-1, 0, -12), 2);
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

	lights.push_back(std::unique_ptr<Light>(new Light(Vector(-20, 70, 20), 0.5)));
	lights.push_back(std::unique_ptr<Light>(new Light(Vector(30, 50, -12), 1)));

}
