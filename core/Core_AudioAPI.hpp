// Filter "Audio"

#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

namespace Atrium::Core
{
	/**
	 * @brief A playable audio clip.
	 */
	class AudioClip
	{
	public:

		//--------------------------------------------------
		// * Types
		//--------------------------------------------------
		#pragma region Types

		enum class LoadState
		{
			Unloaded,
			Loading,
			Loaded,
			Error
		};

		enum class LoadType
		{
			// Audio data is decompressed when loaded and is stored uncompressed.
			DecompressOnLoad,

			// Audio is kept compressed in memory and is uncompressed at mix.
			DecompressAtMix,
			
			// Stream audio from disk.
			Streaming
		};

		#pragma endregion

		//--------------------------------------------------
		// * Methods
		//--------------------------------------------------
		#pragma region Methods

		virtual LoadType GetLoadType() const = 0;

		virtual LoadState GetLoadState() const = 0;

		/**
		 * @brief Start playing the clip from its current position.
		 */
		virtual void Play() = 0;

		/**
		 * @brief Stop playing the clip, without affecting the playhead.
		 */
		virtual void Stop() = 0;

		#pragma endregion
	};

	/**
	 * @brief Interface for accessing and using audio devices.
	 */
	class AudioAPI
	{
	public:
		virtual ~AudioAPI() = default;

		/**
		 * @brief Set up an audio data stream to a particular file on disk.
		 * @param aPath A file path to stream from.
		 * @param aLoadType A behaviour for how to load the audio data.
		 * @return An instance of an AudioStream for the given path.
		 */
		virtual std::shared_ptr<AudioClip> CreateClipFromDisk(const std::filesystem::path& aPath, const AudioClip::LoadType aLoadType = AudioClip::LoadType::DecompressOnLoad) = 0;
	};
}