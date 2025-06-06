// Filter "Audio"

#pragma once

#include "Core_AudioAPI.hpp"

namespace Atrium::Core
{
	/**
	 * @brief Empty audio handler implementation which acts like functional API but does nothing.
	 */
	class NullAudioHandler final : public AudioAPI
	{
	public:
		std::shared_ptr<AudioClip> CreateClipFromDisk(const std::filesystem::path&, const AudioClip::LoadType) { return nullptr; }
	};
}