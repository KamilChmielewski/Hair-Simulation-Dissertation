#include "Appearance.h"
#include <iostream>

Appearance::Appearance(Geometry geometry, Material material, ID3D11DeviceContext * pImmediateContext) : _geometry(geometry), _material(material), _pImmediateContext(pImmediateContext)
{
}

Appearance::~Appearance()
{
}

void Appearance::Draw()
{
	_pImmediateContext->IASetVertexBuffers(0, 1, &_geometry.vertexBuffer, &_geometry.vertexBufferStride, &_geometry.vertexBufferOffset);
	_pImmediateContext->IASetIndexBuffer(_geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	_pImmediateContext->DrawIndexed(_geometry.numberOfIndices, 0, 0);
}

void Appearance::DrawLines()
{
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	_pImmediateContext->IASetVertexBuffers(0, 1, &_geometry.vertexBuffer, &_geometry.vertexBufferStride, &_geometry.vertexBufferOffset);
	_pImmediateContext->IASetIndexBuffer(_geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	_pImmediateContext->DrawIndexed(_geometry.numberOfIndices, 0, 0);
}

void Appearance::DrawObj()
{
	if (_model == nullptr)
	{
		return;
	}
	static UINT stride = sizeof(Vertex);
	static UINT offset = 0;

	_pImmediateContext->IASetVertexBuffers(0, 1, &_model->_pVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_model->_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_pImmediateContext->DrawIndexed(_model->Indices.size(), 0, 0);
}

void Appearance::DrawHairStrand()
{

}
