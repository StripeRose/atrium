#include "Graphics_Tasks.hpp"

#include "Common_Debug.hpp"

#include <algorithm>

namespace RoseGold::Core::Graphics
{
	GraphicsTask::GraphicsTask(const char* aName)
		: myName(aName)
	{ }

	void GraphicsTask::ResolveWorkTasks(std::vector<GraphicsTask*>& outTasks)
	{
		GetWorkTasks(outTasks);
		std::sort(outTasks.begin(), outTasks.end(), [](GraphicsTask* a, GraphicsTask* b) {
			return b->DependsOn(*a);
			});
	}

	void GraphicsTask::AddDependency(GraphicsTask& aTask)
	{
		Debug::Assert(
			!DependsOn(aTask),
			"GraphicsTask circular dependency.");
		myDependencies.push_back(&aTask);
	}

	CommandBuffer& GraphicsTask::AddWork()
	{
		Debug::Assert(myTasks.empty(), "A group task cannot itself contain any work.");
		return myCommandBuffers.emplace_back();
	}

	GraphicsTask& GraphicsTask::CreateTask()
	{
		Debug::Assert(myCommandBuffers.empty(), "A work-task cannot contain any sub-tasks.");
		return *myTasks.emplace_back(std::make_unique<GraphicsTask>()).get();
	}

	GraphicsTask& GraphicsTask::CreateTask(const char* aTaskName)
	{
		Debug::Assert(myCommandBuffers.empty(), "A work-task cannot contain any sub-tasks.");
		return *myTasks.emplace_back(std::make_unique<GraphicsTask>(aTaskName)).get();
	}

	bool GraphicsTask::DependsOn(const GraphicsTask& aTask) const
	{
		for (const GraphicsTask* dependency : myDependencies)
		{
			if (dependency == &aTask)
				return true;

			if (dependency->DependsOn(aTask))
				return true;
		}

		for (const std::unique_ptr<GraphicsTask>& subTask : myTasks)
		{
			if (subTask.get() == &aTask)
				return true;

			if (subTask->DependsOn(aTask))
				return true;
		}

		return false;
	}

	GraphicsTask* GraphicsTask::FindTask(const char* aTaskName)
	{
		if (myName == aTaskName)
			return this;

		for (const std::unique_ptr<GraphicsTask>& subTask : myTasks)
		{
			GraphicsTask* foundTask = subTask->FindTask(aTaskName);
			if (foundTask != nullptr)
				return foundTask;
		}

		return nullptr;
	}

	const GraphicsTask* GraphicsTask::FindTask(const char* aTaskName) const
	{
		if (myName == aTaskName)
			return this;

		for (const std::unique_ptr<GraphicsTask>& subTask : myTasks)
		{
			const GraphicsTask* foundTask = subTask->FindTask(aTaskName);
			if (foundTask != nullptr)
				return foundTask;
		}

		return nullptr;
	}

	bool GraphicsTask::HasWork() const
	{
		if (!myCommandBuffers.empty())
			return true;

		for (const std::unique_ptr<GraphicsTask>& task : myTasks)
		{
			if (task->HasWork())
				return true;
		}

		return false;
	}

	void GraphicsTask::GetWorkTasks(std::vector<GraphicsTask*>& outTasks)
	{
		for (std::unique_ptr<GraphicsTask>& task : myTasks)
		{
			if (!task->myCommandBuffers.empty())
				outTasks.push_back(task.get());
			else
				task->GetWorkTasks(outTasks);
		}
	}
}
