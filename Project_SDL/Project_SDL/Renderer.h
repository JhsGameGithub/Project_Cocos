#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

class Scene;

namespace RendererHelper
{
	using namespace Microsoft;
	using namespace WRL;

	class Renderer
	{
	private:
		UINT m_4xmsaa_quality;

		ComPtr<IDXGIFactory7> m_dxgi_factory;
		ComPtr<IDXGISwapChain> m_swap_chain;
		ComPtr<ID3D12Device> m_d3d12_device;

		ComPtr<ID3D12Fence> m_fence;
		UINT64 m_current_fence = 0;

		ComPtr<ID3D12CommandQueue> m_command_queue;
		ComPtr<ID3D12CommandAllocator> m_command_allocator;
		ComPtr<ID3D12GraphicsCommandList> m_command_list;

		static const int m_swap_chain_buffer_count = 2;
		int m_current_back_buffer = 0;
		ComPtr<ID3D12Resource> m_swap_chain_buffer[m_swap_chain_buffer_count];
		ComPtr<ID3D12Resource> m_depth_stencil_buffer;

		ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
		ComPtr<ID3D12DescriptorHeap> m_dsv_heap;

		D3D12_VIEWPORT m_screen_viewport;
		D3D12_RECT m_scissor_rect;

		UINT m_rtv_descriptor_size = 0;
		UINT m_dsv_descriptor_size = 0;
		UINT m_cbv_srv_descriptor_size = 0;

		DXGI_FORMAT m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	public:
		void Create_Device();
		void Create_Fecne();
		void Get_Descriptor_Size();
		void Inspect_4XMSAA_Quality();
		void Create_Command_Objects();
		void Create_Swap_Chain();
		void Create_Descriptor_Hepas();
		void Create_Render_Target_View();
		void Create_Depth_Stencil_View();
		void Set_Viewport();
		void Set_Scissor_Rect();
		void Rendering(const Scene* a_scene);
	};

	void Init_Renderer(const int height, const int width, HWND* hwnd, Renderer* renderer);
}