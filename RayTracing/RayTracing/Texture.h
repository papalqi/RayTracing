#pragma once
//ÌùÍ¼
#include "CoreMinimal.h"


class Texture
{
public:
	Texture(Color* p_Bitmap, int p_Width, int p_Height);
	Texture(char* p_File);
	inline Color* getBitmap() { return m_Bitmap; }
	Color getUVColor(double p_U, double p_V);
	inline int getWidth() { return m_Width; }
	inline int getHeight() { return m_Height; }
private:
	Color* m_Bitmap;
	int m_Width, m_Height;
};