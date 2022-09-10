#pragma once
#include <Windows.h>

namespace Helper
{
	namespace renderer
	{

	}

	namespace mesh
	{

	}


	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}
}