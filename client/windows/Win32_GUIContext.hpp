// Filter "GUI"

#include "Win32_WindowManagement.hpp"

#include <Atrium_GUIContext.hpp>
#include <Atrium_WindowManagement.hpp>

namespace Atrium::Win32
{
	class GUIBackendContext : public Atrium::GUIContext
	{
	public:
		GUIBackendContext(const std::shared_ptr<Atrium::Window>& aWindow);
		~GUIBackendContext() override;

		void MarkFrameStart() override;
		void MarkFrameEnd() override;

	private:
		std::shared_ptr<Window> myWindow;
	};
}
