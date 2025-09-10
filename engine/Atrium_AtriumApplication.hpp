#pragma once

#include <memory>

namespace Atrium
{
	class AudioAPI;
	class FrameGraphicsContext;
	class GraphicsAPI;
	class InputDeviceAPI;
	class WindowManager;

	class AtriumApplication
	{
	public:
		//--------------------------------------------------
		// * Construction
		//--------------------------------------------------
		#pragma region Construction

		AtriumApplication();
		~AtriumApplication();

		#pragma endregion

		//--------------------------------------------------
		// * Methods
		//--------------------------------------------------
		#pragma region Methods

		/**
		 * @brief Request the application to exit when possible.
		 */
		void Exit();

		/**
		 * @brief Request the application to exit when possible.
		 * @param anExitCode An exit code to use when returning from Run().
		 */
		void Exit(int anExitCode);

		/**
		 * @brief Get the currently running application instance, if any.
		 * @return A pointer to the application, or nullptr if none are running.
		 */
		[[nodiscard]] static AtriumApplication* GetRunningInstance();

		/**
		 * @brief Get the currently active audio handler.
		 */
		[[nodiscard]] AudioAPI& GetAudioHandler() { return *myAudioAPI; }

		/**
		 * @brief Get the currently active graphics handler.
		 */
		[[nodiscard]] GraphicsAPI& GetGraphicsHandler() { return *myGraphicsAPI; }

		/**
		 * @brief Get the currently active input handler.
		 */
		[[nodiscard]] InputDeviceAPI& GetInputHandler() { return *myInputDeviceAPI; }

		/**
		 * @brief Get the currently active window handler.
		 */
		[[nodiscard]] WindowManager& GetWindowHandler() { return *myWindowManager; }

		/**
		 * @brief Start up the engine to run the application.
		 * @return The final exit code of the application.
		 */
		int Run();

		#pragma endregion

	protected:
		/**
		 * @brief Called in between frames if the engine has been requested to close, to allow cancelling this.
		 * @param aShouldExit Whether or not the main loop should exit. Set to false to cancel the exit.
		 */
		virtual void HandleCloseRequest(bool& aShouldExit);

		// Called once per frame.
		virtual void HandleFrameLogic(FrameGraphicsContext& aFrameContext) = 0;

		// Called when the last loop has ended and before the engine is cleaned up.
		virtual void HandleShutdown() = 0;

		/**
		 * @brief Called when engine has been initialized but before the main loop begins.
		 * @return Whether the startup is successful and the main loop can begin.
		 */
		[[nodiscard]]
		virtual bool HandleStartup() = 0;

	private:
		void CreateAPIHandlers();
		void AssertAPIHandlersExist();

		void SetupEngine();
		void RunMainLoop();
		void DoTick();
		void CleanupEngine();

		std::unique_ptr<AudioAPI> myAudioAPI;
		std::unique_ptr<GraphicsAPI> myGraphicsAPI;
		std::unique_ptr<InputDeviceAPI> myInputDeviceAPI;
		std::unique_ptr<WindowManager> myWindowManager;

		std::shared_ptr<FrameGraphicsContext> myFrameGraphics;

		bool myIsRunning;
		bool myHasShutdownBeenRequested;
		int myLastExitCode;
	};
}
