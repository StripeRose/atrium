#pragma once

#include <memory>
#include <span>

namespace RoseGold::Core
{
	class GraphicsBuffer
	{
	public:
		enum class Target { Constant, Index, Vertex };

	public:
		virtual ~GraphicsBuffer() = default;

		virtual std::uint32_t GetCount() const = 0;
		virtual std::uint32_t GetStride() const = 0;
		virtual Target GetTarget() const = 0;

		virtual void* GetNativeBufferPtr() = 0;

		template <typename T>
		void SetData(std::span<T> aDataSpan)
		{
			SetData(aDataSpan.data(), static_cast<std::uint32_t>(aDataSpan.size_bytes()));
		}

		virtual void SetData(const void* aDataPtr, std::uint32_t aDataSize) = 0;
		virtual void SetName(const wchar_t* aName) = 0;
	};
}