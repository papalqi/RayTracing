#pragma once
#include "Object.h"
class ObjectList : public Object 
{
public:
	ObjectList();
	ObjectList(Object **l, int n);
	virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec) const;
	Object **list;
	int list_size;
};


