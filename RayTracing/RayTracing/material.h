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
	inline void setColor(Color& p_Color) { m_Color = p_Color; m_Tex = NULL; m_TexRatio = m_TexRatioDao = 1.0; }
	inline Color getColor() { return m_Color; }
	inline void setSpecular(double p_Spec) { m_Spec = p_Spec; }//�߹�
	inline void setDiffuse(double p_Diff) { m_Diff = p_Diff; }
	inline void setReflection(double p_Refl) { m_Refl = p_Refl; }
	inline void setDiffRefl(double p_DiffRefl) { m_DiffRefl = p_DiffRefl; }
	inline void setRefraction(double p_Refr) { m_Refr = p_Refr; }
	inline void setRefr_Rate(double p_Refr_Rate) { m_Refr_Rate = p_Refr_Rate; }
	inline double getSpecular() { return m_Spec; }//�߹�
	inline double getDiffuse() { return m_Diff; }//������
	inline double getReflection() { return m_Refl; }
	inline double getDiffRefl() { return m_DiffRefl; }
	inline double getRefraction() { return m_Refr; }
	inline double getRefr_Rate() { return m_Refr_Rate; }

	inline void setTexure(Texture* p_Tex) { m_Tex = p_Tex; }
	inline Texture* getTexure() { return m_Tex; }
	inline void setTexRatio(double p_TexRatio) { m_TexRatio = p_TexRatio; m_TexRatioDao = 1.0 / p_TexRatio; }
	inline double getTexRatio() { return m_TexRatio; }
	inline double getTexRatioDao() { return m_TexRatioDao; }
	//virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const ;
private:
	Color m_Color;//������ɫ
	double m_Refl;//����ϵ��
	double m_DiffRefl;//�����淴��ϵ��
	double m_Diff;//������ϵ��
	double m_Spec;//�߹�
	double m_Refr;//͸����
	double m_Refr_Rate;//������
	Texture* m_Tex;
	double m_TexRatio;//һ������ķŴ���
	double m_TexRatioDao;//һ������ķŴ����ĵ���
};




