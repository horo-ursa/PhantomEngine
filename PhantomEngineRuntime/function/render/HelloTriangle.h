#pragma once



namespace Phantom
{
	class helloTriangle
	{
	public:
		helloTriangle();
		~helloTriangle();

		void onInit(HWND hWnd, float width, float height);
		void onUpdate();
		void onRender();
		void onDestroy();

	private:
		void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter);
		void LoadPipeline(HWND hWnd, float width, float height);
		void LoadAssets();
		void WaitForPreviousFrame();
		void PopulateCommandList();

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
		UINT m_rtvDescriptorSize;
		wrl::ComPtr<ID3D12Device> mDevice;
		wrl::ComPtr<IDXGISwapChain3> mSwapChain;
		wrl::ComPtr<ID3D12CommandQueue> mCommandQueue;
		wrl::ComPtr<ID3D12GraphicsCommandList> mCommandList;
		wrl::ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
		wrl::ComPtr<ID3D12Resource> mRenderTargets[2];
		wrl::ComPtr<ID3D12CommandAllocator> mAllocator;
		wrl::ComPtr<ID3D12RootSignature> mSignature;
		wrl::ComPtr<ID3D12PipelineState> mPipelineState;

		wrl::ComPtr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

		// Synchronization objects.
		UINT m_frameIndex;
		HANDLE m_fenceEvent;
		wrl::ComPtr<ID3D12Fence> m_fence;
		UINT64 m_fenceValue;
	};
}