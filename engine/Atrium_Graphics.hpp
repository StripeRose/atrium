// Filter "Graphics"

#include "Atrium_RenderTarget.hpp"

namespace Atrium
{
	class Graphics
	{
	public:
		/**
		 * @brief Clear a render texture's color to a specific value.
		 *        If the chosen target has no color buffer, the call does nothing.
		 *
		 * @param aTarget Target to clear the color buffer of.
		 * @param aClearColor Color value to clear to.
		 */
		static void ClearColor(const RenderTarget& aTarget, ColorT<float> aClearColor);

		/**
		 * @brief Clear a render texture's depth and stencil to a specified value.
		 *        If the target has no depth buffer, the call does nothing.
		 *
		 * @param aTarget Target to clear the depth buffer of.
		 * @param aDepth Depth value to clear to.
		 * @param aStencil Stencil value to clear to.
		 */
		static void ClearDepth(const RenderTarget& aTarget, float aDepth, std::uint8_t aStencil);
	};
}
