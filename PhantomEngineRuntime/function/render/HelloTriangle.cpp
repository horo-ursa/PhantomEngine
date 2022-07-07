#include "stdafx.h"
#include "HelloTriangle.h"
#include <d3dcompiler.h>
#include "vertexformat.h"
#include "d3dx12.h"

#pragma comment (lib, "d3d12.lib")


using namespace wrl;
using namespace DX;

namespace Phantom 
{
	helloTriangle::helloTriangle() {

	}

	helloTriangle::~helloTriangle() {

	}

	void helloTriangle::GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
	{
		*ppAdapter = nullptr;
		for (UINT adapterIndex = 0; ; ++adapterIndex)
		{
			IDXGIAdapter1* pAdapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter))
			{
				// No more adapters to enumerate.
				break;
			}

			// Check to see if the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				*ppAdapter = pAdapter;
				return;
			}
			pAdapter->Release();
		}
	}

	void helloTriangle::LoadPipeline(HWND hWnd, float width, float height)
	{
#if defined(_DEBUG)
		wrl::ComPtr<ID3D12Debug> debugger;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugger))))
		{
			debugger->EnableDebugLayer();
		}
#endif

		wrl::ComPtr<IDXGIFactory4> factory;
		DX::ThrowIfFailed(CreateDXGIFactory2(
#if defined(_DEBUG)
			DXGI_CREATE_FACTORY_DEBUG,
#else
			0,
#endif
			IID_PPV_ARGS(&factory)
		));

		wrl::ComPtr<IDXGIAdapter1> hardwardAdaptor;
		GetHardwareAdapter(factory.Get(), &hardwardAdaptor);

		DX::ThrowIfFailed(
			D3D12CreateDevice(
				hardwardAdaptor.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&mDevice)
			)
		);

		D3D12_COMMAND_QUEUE_DESC queueDesc;
		ZeroMemory(&queueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		DX::ThrowIfFailed(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

		{
			DXGI_SWAP_CHAIN_DESC1 dscd;
			ZeroMemory(&dscd, sizeof(DXGI_SWAP_CHAIN_DESC1));
			dscd.Width = 0;
			dscd.Height = 0;
			dscd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dscd.Stereo = FALSE;
			dscd.SampleDesc.Count = 1;
			dscd.SampleDesc.Quality = 0;
			dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			dscd.BufferCount = 2;
			dscd.Scaling = DXGI_SCALING_STRETCH;
			dscd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			dscd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			wrl::ComPtr<IDXGISwapChain1> swapChain;
			DX::ThrowIfFailed(
				factory->CreateSwapChainForHwnd(
					mCommandQueue.Get(),
					hWnd,
					&dscd,
					NULL,
					NULL,
					&swapChain
				)
			);
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
			ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
			rtvHeapDesc.NumDescriptors = 2;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			DX::ThrowIfFailed(
				mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mDescriptorHeap))
			);
			m_rtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		//rtvHandle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT n = 0; n < 2; n++) {
			DX::ThrowIfFailed(mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n])));
			mDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}

		DX::ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mAllocator)));
	}

	void helloTriangle::LoadAssets()
	{
		// create empty root signature
		{
			D3D12_ROOT_SIGNATURE_DESC rootDesc;
			ZeroMemory(&rootDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
			rootDesc.NumParameters = 0;
			rootDesc.pParameters = nullptr;
			rootDesc.NumStaticSamplers = 0;
			rootDesc.pStaticSamplers = nullptr;
			rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			wrl::ComPtr<ID3DBlob> signature;
			wrl::ComPtr<ID3DBlob> error;
			DX::ThrowIfFailed(
				D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)
			);
			DX::ThrowIfFailed(
				mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mSignature))
			);
		}

		//create pso, which includes compiling and loading shaders
		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> pixelShader;

		#if defined(_DEBUG)
			// Enable better shader debugging with the graphics debugging tools.
			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		#else
			UINT compileFlags = 0;
		#endif
			ThrowIfFailed(
				D3DCompileFromFile(L"PhantomEngineRuntime\function\render\shaders\simple.hlsl", nullptr, nullptr,
					"VS", "vs_5_0", compileFlags, 0, &vertexShader, nullptr)
			);

			ThrowIfFailed(
				D3DCompileFromFile(L"PhantomEngineRuntime\function\render\shaders\simple.hlsl", nullptr, nullptr,
					"PS", "ps_5_0", compileFlags, 0, &pixelShader, nullptr)
			);

			// Define the vertex input layout.
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosColor, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			//ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
			psoDesc.InputLayout = { inputElementDescs, sizeof(inputElementDescs) };
			psoDesc.pRootSignature = mSignature.Get();
			psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
			psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.StencilEnable = FALSE;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));
		}

		// Create the command list.
		ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			mAllocator.Get(), mPipelineState.Get(), IID_PPV_ARGS(&mCommandList)));
		// Command lists are created in the recording state, but there is nothing
		// to record yet. The main loop expects it to be closed, so close it now.
		ThrowIfFailed(mCommandList->Close());

		// Create the vertex buffer.
		{
			// Define the geometry for a triangle.
			VertexPosColor vert[] =
			{
				{ Vector3(0.0f, 0.5f, 0.0f), Color4(1.0f, 0.0f, 0.0f, 1.0f) },
				{ Vector3(0.45f, -0.5f, 0.0f), Color4(0.0f, 1.0f, 0.0f, 1.0f) },
				{ Vector3(-0.45f, -0.5f, 0.0f), Color4(0.0f, 0.0f, 1.0f, 1.0f) }
			};

			const UINT vertexBufferSize = sizeof(vert);

			// Note: using upload heaps to transfer static data like vert buffers is not 
			// recommended. Every time the GPU needs it, the upload heap will be marshalled 
			// over. Please read up on Default Heap usage. An upload heap is used here for 
			// code simplicity and because there are very few verts to actually transfer.
			CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
			auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
			ThrowIfFailed(mDevice->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_vertexBuffer)));

			// Copy the triangle data to the vertex buffer.
			UINT8* pVertexDataBegin;
			CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
			memcpy(pVertexDataBegin, vert, sizeof(vert));
			m_vertexBuffer->Unmap(0, nullptr);

			// Initialize the vertex buffer view.
			m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = sizeof(VertexPosColor);
			m_vertexBufferView.SizeInBytes = vertexBufferSize;
		}

		// Create synchronization objects and wait until assets have been uploaded to the GPU.
		{
			ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
			m_fenceValue = 1;

			// Create an event handle to use for frame synchronization.
			m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fenceEvent == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}

			// Wait for the command list to execute; we are reusing the same command 
			// list in our main loop but for now, we just want to wait for setup to 
			// complete before continuing.
			WaitForPreviousFrame();

		}
	}

	void helloTriangle::WaitForPreviousFrame()
	{
		const UINT fenceCount = m_fenceValue;
		ThrowIfFailed(mCommandQueue->Signal(m_fence.Get(), fenceCount));
		m_fenceValue++;

		if (m_fence->GetCompletedValue() < fenceCount) {
			
			ThrowIfFailed(m_fence->SetEventOnCompletion(fenceCount, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
		m_frameIndex = mSwapChain->GetCurrentBackBufferIndex();
	}

	void helloTriangle::PopulateCommandList()
	{
		mAllocator->Reset();
		mCommandList->Reset(mAllocator.Get(), mPipelineState.Get());

		mCommandList->SetGraphicsRootSignature(mSignature.Get());
		mCommandList->RSSetViewports(1, &m_viewport);
		mCommandList->RSSetScissorRects(1, &m_scissorRect);

		// Indicate that the back buffer will be used as a render target
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[m_frameIndex].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		mCommandList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_frameIndex, m_rtvDescriptorSize);
		mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		mCommandList->DrawInstanced(3, 1, 0, 0);

		// Indicate that the back buffer will now be used to present.
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[m_frameIndex].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		mCommandList->ResourceBarrier(1, &barrier);

		ThrowIfFailed(mCommandList->Close());
	}

	void helloTriangle::onInit(HWND hWnd, float width, float height) {
		LoadPipeline(hWnd, width, height);
		LoadAssets();
	}

	void helloTriangle::onUpdate()
	{

	}

	void helloTriangle::onRender()
	{
		PopulateCommandList();

		ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
		mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		mSwapChain->Present(1, 0);
		WaitForPreviousFrame();
	}

	void helloTriangle::onDestroy()
	{
		WaitForPreviousFrame();
		CloseHandle(m_fenceEvent);
	}
}