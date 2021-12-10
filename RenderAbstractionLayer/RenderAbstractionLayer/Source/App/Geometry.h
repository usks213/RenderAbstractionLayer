/*****************************************************************//**
 * \file   Geometry.h
 * \brief  ÉWÉIÉÅÉgÉä
 * 
 * \author USAMI KOSHI
 * \date   2021/10/06
 *********************************************************************/
#ifndef _GEOMETRY_
#define _GEOMETRY_

#include <Renderer/Core/Core_Mesh.h>

class Geometry
{
public:
	static void Quad(core::CoreMesh& out);
	static void Plane(core::CoreMesh& out, int split = 10, float size = 1.0f, float texSize = 0.1f);
	static void Cube(core::CoreMesh& out);
	static void Sphere(core::CoreMesh& out, int nSplit, float fSize, float fTexSize);
	static void SkyDome(core::CoreMesh& out, int nSegment, float fTexSplit);

private:
	static bool CalcTangent(core::CoreMesh& mesh);
	static bool CalcIndex(std::uint32_t* pOut, core::CoreMesh& mesh, std::uint32_t face);

};

#endif // !_GEOMETRY_
