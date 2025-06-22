#include "Core_ImGuiContext.hpp"

#include <memory>
#include <vector>

namespace Atrium::Core
{
    class ImGuiCompositeContext : public ImGuiContext
    {
    public:
        ImGuiCompositeContext(std::vector<std::unique_ptr<ImGuiContext>>&& someContexts)
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
        std::vector<std::unique_ptr<ImGuiContext>> myContexts;
    };

    std::unique_ptr<ImGuiContext> ImGuiContext::Composite(std::vector<std::unique_ptr<ImGuiContext>>&& someContexts)
    {
        return std::unique_ptr<ImGuiContext>(new ImGuiCompositeContext(std::move(someContexts)));
    }
}
