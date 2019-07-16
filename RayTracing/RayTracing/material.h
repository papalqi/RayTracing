#pragma once
#include "CoreMinimal.h"
#include "MaterialMath.h"
#include "Random.h"
#include "Ray.h"

#include "Texture.h"


class material 
{
public:
	material();
	inline void setColor(Color p_Color) { m_Color = p_Color; m_Tex = NULL; m_TexRatio = m_TexRatioDao = 1.0; }
	inline Color getColor() { return m_Color; }
	inline void setSpecular(float p_Spec) { m_Spec = p_Spec; }//高光
	inline void setDiffuse(float p_Diff) { m_Diff = p_Diff; }
	inline void setReflection(float p_Refl) { m_Refl = p_Refl; }
	inline void setDiffRefl(float p_DiffRefl) { m_DiffRefl = p_DiffRefl; }
	inline void setRefraction(float p_Refr) { m_Refr = p_Refr; }
	inline void setRefr_Rate(float p_Refr_Rate) { m_Refr_Rate = p_Refr_Rate; }
	inline float getSpecular() { return m_Spec; }//高光
	inline float getDiffuse() { return m_Diff; }//漫反射
	inline float getReflection() { return m_Refl; }
	inline float getDiffRefl() { return m_DiffRefl; }
	inline float getRefraction() { return m_Refr; }
	inline float getRefr_Rate() { return m_Refr_Rate; }
	
	inline void setTexure(Texture* p_Tex) { m_Tex = p_Tex; }
	inline Texture* getTexure() { return m_Tex; }
	inline void setTexRatio(float p_TexRatio) { m_TexRatio = p_TexRatio; m_TexRatioDao = 1.0 / p_TexRatio; }
	inline float getTexRatio() { return m_TexRatio; }
	inline float getTexRatioDao() { return m_TexRatioDao; }
	//virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const ;
	public:
	Color m_Color=0;//材质颜色
	float m_Refl = 0;//反射系数
	float m_DiffRefl = 0;//漫镜面反射系数
	float m_Diff = 0;//漫反射系数
	float m_Spec = 0;//高光
	float m_Refr = 0;//透射率
	float m_Refr_Rate = 0;//折射率
	Texture* m_Tex=nullptr;
	float m_TexRatio = 0;//一块纹理的放大倍数
	float m_TexRatioDao = 0;//一块纹理的放大倍数的倒数
};




