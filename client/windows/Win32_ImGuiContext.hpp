// Filter "ImGui"

#include "Win32_WindowManagement.hpp"

#include <Core_ImGuiContext.hpp>
#include <Core_WindowManagement.hpp>

namespace Atrium::Win32
{
	class ImGuiContext : public Core::ImGuiContext
	{
	public:
		ImGuiContext(const std::shared_ptr<Core::Window>& aWindow);
		~ImGuiContext() override;

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		std::shared_ptr<Window> myWindow;
	};
}
