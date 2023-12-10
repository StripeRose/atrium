#pragma once

#include "Graphics_CommandBuffer.hpp"

#include <memory>
#include <string>
#include <vector>

namespace RoseGold::Core::Graphics
{
	class GraphicsTask
	{
	public:
		GraphicsTask() = default;
		GraphicsTask(const char* aName);

		void ResolveWorkTasks(std::vector<GraphicsTask*>& outTasks);

		void AddDependency(GraphicsTask& aTask);
		CommandBuffer& AddWork();

		GraphicsTask& CreateTask();
		GraphicsTask& CreateTask(const char* aTaskName);

		bool DependsOn(const GraphicsTask& aTask) const;

		GraphicsTask* FindTask(const char* aTaskName);
		const GraphicsTask* FindTask(const char* aTaskName) const;

		const std::vector<CommandBuffer>& GetWork() const { return myCommandBuffers; }

		bool HasWork() const;

	private:
		void GetWorkTasks(std::vector<GraphicsTask*>& outTasks);

	private:
		std::string myName;

		std::vector<GraphicsTask*> myDependencies;

		std::vector<std::unique_ptr<GraphicsTask>> myTasks;
		std::vector<CommandBuffer> myCommandBuffers;
	};
}
