#include <iostream>
#include "Renderer.h"

void RendererHelper::Renderer::Create_Device()
{
#if defined(DEBUG) || defined(_DEBUG)
	{
		//D3D12  디버그층을 활성화
		ComPtr<ID3D12Debug> debug_controller;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
		debug_controller->EnableDebugLayer();
	}
#endif
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgi_factory)));

	// 하드웨어 어댑터를 나타내는 장치 생성
	HRESULT hardware_result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12_device));

	// 실패했다면, WARP 어댑터를 나타내는 장치를 생성
	if (FAILED(hardware_result))
	{
		ComPtr<IDXGIAdapter> p_warp_adapter;
		ThrowIfFailed(m_dxgi_factory->EnumWarpAdapter(IID_PPV_ARGS(&p_warp_adapter)));

		ThrowIfFailed(D3D12CreateDevice(p_warp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12_device)));
	}
}

void RendererHelper::Renderer::Create_Fecne()
{
	ThrowIfFailed(m_d3d12_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
}

void RendererHelper::Renderer::Get_Descriptor_Size()
{
	m_rtv_descriptor_size = m_d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsv_descriptor_size = m_d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbv_srv_descriptor_size = m_d3d12_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}