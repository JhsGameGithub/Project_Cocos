#include <iostream>
#include <assert.h>

#include "Renderer.h"
#include "Scene.h"

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

void RendererHelper::Renderer::Inspect_4XMSAA_Quality()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms_quality_levels;
	ms_quality_levels.Format = m_back_buffer_format;
	ms_quality_levels.SampleCount = 4;
	ms_quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	ms_quality_levels.NumQualityLevels = 0;
	ThrowIfFailed(m_d3d12_device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &ms_quality_levels, sizeof(ms_quality_levels)));

	m_4xmsaa_quality = ms_quality_levels.NumQualityLevels;
	assert(m_4xmsaa_quality > 0 && "Unexpected MSAA quality level");
}

void RendererHelper::Renderer::Create_Command_Objects()
{
	D3D12_COMMAND_QUEUE_DESC queue_desc = {};
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_d3d12_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_command_queue)));

	ThrowIfFailed(m_d3d12_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_command_allocator.GetAddressOf())));

	ThrowIfFailed(m_d3d12_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_command_allocator.Get(), nullptr, IID_PPV_ARGS(m_command_list.GetAddressOf())));

	m_command_list->Close();
}

void RendererHelper::Renderer::Create_Swap_Chain(const int height,const int width, HWND* hwnd)
{
	// 기존 교환 사슬 해제
	m_swap_chain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_back_buffer_format;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = m_4xmsaa_quality - 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = *hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(m_dxgi_factory->CreateSwapChain(m_command_queue.Get(), &sd, m_swap_chain.GetAddressOf()));
}

void RendererHelper::Renderer::Create_Descriptor_Hepas()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
	rtv_heap_desc.NumDescriptors = m_swap_chain_buffer_count;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtv_heap_desc.NodeMask = 0;
	ThrowIfFailed(m_d3d12_device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(m_rtv_heap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsv_heap_desc.NodeMask = 0;
	ThrowIfFailed(m_d3d12_device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(m_dsv_heap.GetAddressOf())));
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererHelper::Renderer::Current_Back_Buffer_View() const
{
	//이 생성자는 주어진 오프셋에 해당하는 후면 버퍼
	//RTV의 핸들을 돌려준다.
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_rtv_heap->GetCPUDescriptorHandleForHeapStart(),
		m_current_back_buffer,
		m_rtv_descriptor_size
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE RendererHelper::Renderer::Depth_Stencil_View() const
{
	return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
}

void RendererHelper::Renderer::Create_Render_Target_View()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i < m_swap_chain_buffer_count; i++)
	{
		//교환 사슬의 i번째 버퍼를 얻는다.
		ThrowIfFailed(m_swap_chain->GetBuffer(i, IID_PPV_ARGS(&m_swap_chain_buffer[i])));
		//그 버퍼에 대한 RTV를 생성한다.
		m_d3d12_device->CreateRenderTargetView(m_swap_chain_buffer[i].Get(), nullptr, rtv_heap_handle);
		//힙의 다음 항목으로 넘어간다.
		rtv_heap_handle.Offset(1, m_rtv_descriptor_size);
	}
}

void RendererHelper::Renderer::Create_Depth_Stencil_View(const int height, const int width)
{
	D3D12_RESOURCE_DESC depth_stencil_desc;
	depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depth_stencil_desc.Alignment = 0;
	depth_stencil_desc.Width = width;
	depth_stencil_desc.Height = height;
	depth_stencil_desc.DepthOrArraySize = 1;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.Format = m_depth_stencil_format;
	depth_stencil_desc.SampleDesc.Count = 4;
	depth_stencil_desc.SampleDesc.Quality = m_4xmsaa_quality - 1;
	depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE opt_clear;
	opt_clear.Format = m_depth_stencil_format;
	opt_clear.DepthStencil.Depth = 1.0f;
	opt_clear.DepthStencil.Stencil = 0;
	ThrowIfFailed(m_d3d12_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depth_stencil_desc,
		D3D12_RESOURCE_STATE_COMMON,
		&opt_clear,
		IID_PPV_ARGS(m_depth_stencil_buffer.GetAddressOf())
	));

	// 전체 자원이 밉맵 수준 0에 대한 서술자를
	// 해당 자원의 픽셀 형식을 적용해서 생성한다.
	m_d3d12_device->CreateDepthStencilView(m_depth_stencil_buffer.Get(), nullptr, Depth_Stencil_View());

	// 자원을 초기 상태에서 깊이 버퍼로 사용할 수 있는 상태로 전이
	m_command_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(m_depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE)
	);
}

void RendererHelper::Renderer::Set_Viewport(const int height, const int width)
{
	m_screen_viewport.TopLeftX = 0.0f;
	m_screen_viewport.TopLeftY = 0.0f;
	m_screen_viewport.Width = static_cast<float>(width);
	m_screen_viewport.Height = static_cast<float>(height);
	m_screen_viewport.MinDepth = 0.0f;
	m_screen_viewport.MaxDepth = 1.0f;

	m_command_list->RSSetViewports(1, &m_screen_viewport);
}

void RendererHelper::Renderer::Set_Scissor_Rect(const int height, const int width)
{
	m_scissor_rect = { 0,0,width,height };
	m_command_list->RSSetScissorRects(1, &m_scissor_rect);
}

void RendererHelper::Init_Renderer(const int height, const int width, HWND* hwnd, Renderer* renderer)
{
	// 1.장치생성
	renderer->Create_Device();
	// 2.울타리 생성과 서술자 크기 얻기
	renderer->Create_Fecne();
	renderer->Get_Descriptor_Size();
	// 3.4X MSAA 품질 수준 지원 점검
	renderer->Inspect_4XMSAA_Quality();
	// 4.명령 대기열과 명령 목록 생성
	renderer->Create_Command_Objects();
	// 5.교환 사슬의 서술과 생성
	renderer->Create_Swap_Chain(height, width, hwnd);
	// 6.서술자 힙 생성
	renderer->Create_Descriptor_Hepas();
	// 7.렌더 대상 뷰(RTV) 생성
	renderer->Create_Render_Target_View();
	// 8.깊이, 스텐실 버퍼와 뷰 생성
	renderer->Create_Depth_Stencil_View(height, width);
	// 9.뷰포트 설정
	renderer->Set_Viewport(height, width);
	// 10.가위 직사각형 설정
	renderer->Set_Scissor_Rect(height, width);
}

void RendererHelper::Renderer::Rendering(Scene* a_scene)
{
	a_scene->Render();
}


Microsoft::WRL::ComPtr<ID3D12Resource> RendererHelper::Renderer::Create_Default_Buffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, const void* init_data, UINT64 byte_size, ComPtr<ID3D12Resource>& upload_buffer)
{
	using namespace Microsoft;
	using namespace WRL;

	ComPtr<ID3D12Resource> default_buffer;
	// 실제 기본 버퍼 자원을 생성
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(default_buffer.GetAddressOf())
	));

	// CPU 메모리의 자료를 기본 버퍼에 복사하려면
	// 임시 업로드 힙을 만들어야 함
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(upload_buffer.GetAddressOf())
	));

	// 기본 버퍼에 복사할 자료를 서술
	D3D12_SUBRESOURCE_DATA sub_resource_data = {};
	sub_resource_data.pData = init_data;
	sub_resource_data.RowPitch = byte_size;
	sub_resource_data.SlicePitch = sub_resource_data.RowPitch;

	//기본 버퍼 자원으로의 자료 복사를 요청
	// 개략적으로 말하자면, 보조 함수 UpdateSubresources는 CPU 메모리를
	// 임시 업로드 힙에 복사하고, ID3D12CommandList::CopySubresourceRegion을
	//이용해서 임시 업로드 힙의 자료를 mBuffer에 복사
	cmd_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST
		));

	UpdateSubresources<1>(
		cmd_list,
		default_buffer.Get(),
		upload_buffer.Get(),
		0,
		0,
		1,
		&sub_resource_data);

	cmd_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ
		));

	// 주의 : 위의 함수 호출 이후에도 upload_buffer를 계속
	// 유지해야 한다. 실제로 복사를 수행하는 명령 목록이
	// 아직 실행되지 않았기 때문이다.
	// 복사가 완료되었음이 확실해진 후에 호출자가 upload_buffer를
	// 해제하면 된다.

	return default_buffer;
}