// Filter "Graphics"

#pragma once

//#include "Graphics_CommandBuffer.hpp"

#include <memory>
#include <string>
#include <vector>

// Todo: Move to Engine code.

//namespace RoseGold::Core::Graphics
//{
//	class GraphicsTask
//	{
//	public:
//		GraphicsTask() = default;
//		GraphicsTask(const char* aName);
//
//		void ResolveWorkTasks(std::vector<const GraphicsTask*>& outTasks) const;
//
//		void AddDependency(GraphicsTask& aTask);
//		CommandBuffer& AddWork();
//
//		GraphicsTask& CreateTask();
//		GraphicsTask& CreateTask(const char* aTaskName);
//
//		bool DependsOn(const GraphicsTask& aTask) const;
//
//		GraphicsTask* FindTask(const char* aTaskName);
//		const GraphicsTask* FindTask(const char* aTaskName) const;
//
//		const std::vector<std::shared_ptr<CommandBuffer>>& GetWork() const { return myCommandBuffers; }
//
//		bool HasWork() const;
//
//	private:
//		void GetWorkTasks(std::vector<const GraphicsTask*>& outTasks) const;
//
//	private:
//		std::string myName;
//
//		std::vector<GraphicsTask*> myDependencies;
//
//		std::vector<std::unique_ptr<GraphicsTask>> myTasks;
//		std::vector<std::shared_ptr<CommandBuffer>> myCommandBuffers;
//	};
//}
