#pragma once
#pragma once
#include "ConstantBuffers.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

namespace ModelLoader
{
	Mesh* Load(char* path, ID3D11Device* _pd3dDevice);
};
