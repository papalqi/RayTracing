#pragma once
#include "material.h"

material::material():
m_Color(Color(0.2f, 0.2f, 0.2f)),
m_Refl(0), m_Diff(0.2f), m_Spec(0.8f), m_Refr_Rate(1.5f)
{

}
