#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

#include "Helper.h"

class Scene;

class Renderer
{
	template<typename T>
	using RendPtr = Microsoft::WRL::ComPtr<T>;

private:
	UINT m_4xmsaa_quality;

	RendPtr<IDXGIFactory4> m_dxgi_factory;
	RendPtr<IDXGISwapChain> m_swap_chain;
	RendPtr<ID3D12Device> m_d3d12_device;

	RendPtr<ID3D12Fence> m_fence;
	UINT64 m_current_fence = 0;

	RendPtr<ID3D12CommandQueue> m_command_queue;
	RendPtr<ID3D12CommandAllocator> m_command_allocator;
	RendPtr<ID3D12GraphicsCommandList> m_command_list;

	static const int m_swap_chain_buffer_count = 2;
	int m_current_back_buffer = 0;
	RendPtr<ID3D12Resource> m_swap_chain_buffer[m_swap_chain_buffer_count];
	RendPtr<ID3D12Resource> m_depth_stencil_buffer;

	RendPtr<ID3D12DescriptorHeap> m_rtv_heap;
	RendPtr<ID3D12DescriptorHeap> m_dsv_heap;

	D3D12_VIEWPORT m_screen_viewport;
	D3D12_RECT m_scissor_rect;

	UINT m_rtv_descriptor_size = 0;
	UINT m_dsv_descriptor_size = 0;
	UINT m_cbv_srv_descriptor_size = 0;

	DXGI_FORMAT m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;

public:
	//��ġ ����
	void Create_Device();
	//��Ÿ�� ����
	void Create_Fecne();
	//������ ũ�� ���
	void Get_Descriptor_Size();
	//4X MSAA ǰ�� ���� ���� ����
	void Inspect_4XMSAA_Quality();
	//��� ��⿭�� ��� ��� ����
	void Create_Command_Objects();
	//��ȯ �罽�� ������ ����
	void Create_Swap_Chain(const int height, const int width, HWND* hwnd);
	//������ �� ����
	void Create_Descriptor_Hepas();
	//���� ��� �� ����
	void Create_Render_Target_View();
	//����, ���ٽ� ���ۿ� �� ����
	void Create_Depth_Stencil_View(const int height, const int width);
	//����Ʈ ����
	void Set_Viewport(const int height, const int width);
	//���� ���簢�� ����
	void Set_Scissor_Rect(const int height, const int width);

	D3D12_CPU_DESCRIPTOR_HANDLE Current_Back_Buffer_View() const;

	D3D12_CPU_DESCRIPTOR_HANDLE Depth_Stencil_View() const;

	//ȭ�鿡 �׸���
	void Rendering(Scene* a_scene);
};

namespace Helper
{
	namespace renderer
	{
		void Init_Renderer(const int height, const int width, HWND* hwnd, Renderer* renderer);
	}
}