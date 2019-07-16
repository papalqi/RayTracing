#include "Texture.h"
#include <stdio.h>

Texture::Texture(Color* p_Bitmap, int p_Width, int p_Height) :
	m_Bitmap(p_Bitmap), m_Width(p_Width), m_Height(p_Height)
{
}

Texture::Texture(char* p_File)
{
	FILE* f = fopen(p_File, "rb");
	if (f)
	{
		unsigned char buffer[20];
		fread(buffer, 1, 20, f);
		m_Width = *(buffer + 12) + 256 * *(buffer + 13);
		m_Height = *(buffer + 14) + 256 * *(buffer + 15);
		fclose(f);
		f = fopen(p_File, "rb");
		unsigned char* t = new unsigned char[m_Width * m_Height * 3 + 1024];
		fread(t, 1, m_Width * m_Height * 3 + 1024, f);
		fclose(f);
		m_Bitmap = new Color[m_Width * m_Height];
		double rec = 1.0f / 256;
		for (int size = m_Width * m_Height, i = 0; i < size; i++)
			m_Bitmap[i] = Color(t[i * 3 + 20] * rec, t[i * 3 + 19] * rec, t[i * 3 + 18] * rec);
		delete t;
	}
}
Color Texture::getUVColor(double p_u, double p_v)//��Ӧ��uvƽ������꣬��һ����0-1֮��
{
	double tmp_u = p_u + 204800.0;
	double tmp_v = p_v + 204800.0;
	assert(tmp_u > 0);
	assert(tmp_v > 0);
	double u_W = tmp_u * m_Width;
	double v_H = tmp_v * m_Height;
	int u1 = (int)u_W % m_Width, u2 = (u1 + 1) % m_Width;
	int v1 = (int)v_H % m_Height, v2 = (v1 + 1) % m_Height;
	double deltp_u = u_W - floor(u_W);
	double deltp_v = v_H - floor(v_H);
	double w11 = (1 - deltp_u) * (1 - deltp_v);
	double w12 = (1 - deltp_u) * deltp_v;
	double w21 = deltp_u * (1 - deltp_v);
	double w22 = deltp_u * deltp_v;
	Color c11 = m_Bitmap[u1 + v1 * m_Width];
	Color c21 = m_Bitmap[u2 + v1 * m_Width];
	Color c12 = m_Bitmap[u1 + v2 * m_Width];
	Color c22 = m_Bitmap[u2 + v2 * m_Width];
	return w11 * c11 + w12 * c12 + w21 * c21 + w22 * c22;
}