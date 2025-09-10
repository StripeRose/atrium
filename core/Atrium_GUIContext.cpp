#include "Atrium_GUIContext.hpp"

#include <memory>
#include <vector>

namespace Atrium
{
    class GUICompositeContext : public GUIContext
    {
    public:
        GUICompositeContext(std::vector<std::unique_ptr<GUIContext>>&& someContexts)
            : myContexts(std::move(someContexts))
        {

        }

        void MarkFrameStart() override
        {
            for (auto& context : myContexts)
                context->MarkFrameStart();
        }

        void MarkFrameEnd() override
        {
            for (auto& context : myContexts)
                context->MarkFrameEnd();
        }

    private:
        std::vector<std::unique_ptr<GUIContext>> myContexts;
    };

    std::unique_ptr<GUIContext> GUIContext::Composite(std::vector<std::unique_ptr<GUIContext>>&& someContexts)
    {
        return std::unique_ptr<GUIContext>(new GUICompositeContext(std::move(someContexts)));
    }
}
