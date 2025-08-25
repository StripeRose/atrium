// Filter "GUI"

#include "Win32_WindowManagement.hpp"

#include <Core_GUIContext.hpp>
#include <Core_WindowManagement.hpp>

namespace Atrium::Win32
{
	class GUIBackendContext : public Core::GUIContext
	{
	public:
		GUIBackendContext(const std::shared_ptr<Core::Window>& aWindow);
		~GUIBackendContext() override;

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		std::shared_ptr<Window> myWindow;
	};
}
