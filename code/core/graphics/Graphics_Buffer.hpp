#pragma once

#include <memory>
#include <span>

namespace RoseGold::Core::Graphics
{
	class GraphicsBuffer
	{
	public:
		enum class Target { Index, Vertex };

	public:
		virtual ~GraphicsBuffer() = default;

		template <typename T>
		void SetData(std::span<T> aDataSpan)
		{
			SetData(aDataSpan.data(), static_cast<std::uint32_t>(aDataSpan.size_bytes()));
		}

		virtual void SetData(const void* aDataPtr, std::uint32_t aDataSize) = 0;

		virtual std::uint32_t GetCount() const = 0;
		virtual std::uint32_t GetStride() const = 0;
		virtual Target GetTarget() const = 0;

		virtual void* GetNativeBufferPtr() = 0;
	};
}