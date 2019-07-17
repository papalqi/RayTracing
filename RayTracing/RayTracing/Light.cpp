#include "Light.h"



BaseLighting::BaseLighting(const Vector& p, const Vector& i) : position(p), intensity(i)
{
	this->m_Material.setColor(i);
	intensity = i;
	LightType = point;
	bLight = true;
}

BaseLighting::BaseLighting()
{
	bLight = true;

	position = 0;
	LightType = point;
	intensity = Vector(1, 1, 1);
	this->m_Material.setColor({ 1, 1, 1 });
}
