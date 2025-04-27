// Filter "Graphics"

#pragma once

#include <rose-common/Enum.hpp>

#include <memory>
#include <span>

namespace Atrium::Core
{
	/**
	 * @brief A section of memory on the graphics card usable by shader programs.
	 */
	class GraphicsBuffer
	{
	public:
		/**
		 * @brief Specifies which usage targets a GraphicsBuffer can have.
		 */
		enum class Target
		{
			None = 0,

			// GraphicsBuffer to be usable as a Constant buffer.
			Constant = 1 << 0,

			// GraphicsBuffer to be usable as an Index buffer.
			Index = 1 << 1,

			// GraphicsBuffer to be usable as a Vertex buffer.
			Vertex = 1 << 2
		};

	public:
		virtual ~GraphicsBuffer() = default;

		//--------------------------------------------------
		// * Methods
		//--------------------------------------------------
		#pragma region Methods

		/**
		 * @brief Get the amount of individual elements the GraphicsBuffer was created to hold.
		 * 
		 * @return The amount of elements.
		 */
		virtual std::uint32_t GetCount() const = 0;

		/**
		 * @brief Get the size of each individual element the GraphicsBuffer was created to hold.
		 * 
		 * @return The size in bytes.
		 */
		virtual std::uint32_t GetStride() const = 0;

		/**
		 * @brief Get a pointer to the native buffer in use by the Graphics API.
		 *        The type depends on the underlying API used.
		 *        For DirectX 11 it is a ID3D11Resource.
		 *        For DirectX 12 it is a ID3D12Resource.
		 * 
		 * @return The pointer to the underlying graphics API buffer.
		 */
		virtual void* GetNativeBufferPtr() = 0;

		/**
		 * @brief Copy data from a section of T elements into the buffer.
		 * 
		 * @tparam T Type of the elements to be filled.
		 * @param aDataSpan Span pointing at a set of elements to copy.
		 * @param aDestinationOffset Offset in the destination to start copying the data into.
		 */
		template <typename T>
		void SetData(std::span<T> aDataSpan, std::size_t aDestinationOffset = 0)
		{
			SetData(aDataSpan.data(), static_cast<std::uint32_t>(aDataSpan.size_bytes()), sizeof(T) * aDestinationOffset);
		}

		/**
		 * @brief Copy a section of data starting at a specified pointer.
		 * 
		 * @param aDataPtr Pointer where the source data begins.
		 * @param aDataSize The size of the source data.
		 * @param aDestinationOffset Offset in the destination to start copying the data into.
		 */
		virtual void SetData(const void* aDataPtr, std::uint32_t aDataSize, std::size_t aDestinationOffset = 0u) = 0;

		/**
		 * @brief Set the name of the buffer, to help with debugging.
		 * 
		 * @param aName Name to use.
		 */
		virtual void SetName(const wchar_t* aName) = 0;

		#pragma endregion
	};
}

ENUM_FLAGS(Atrium::Core::GraphicsBuffer::Target);