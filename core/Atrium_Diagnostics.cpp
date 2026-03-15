
#include "Atrium_Diagnostics.hpp"

#ifdef TRACY_ENABLE
_NODISCARD
_Ret_notnull_
_Post_writable_byte_size_(count)
_VCRT_ALLOCATOR
void* operator new(std::size_t count) noexcept(false)
{
	auto ptr = malloc(count);
	if (!ptr)
		throw std::bad_alloc();
	TracyAlloc(ptr, count);
	return ptr;
}

void operator delete(void* ptr) noexcept
{
	TracyFree(ptr);
	free(ptr);
}

class TracyDebugLog final : public Atrium::Debug::Impl
{
public:
	static void Attach()
	{
		static TracyDebugLog instance;
		instance.ourParentHandler = Atrium::Debug::ourDebugImplementation;
		Atrium::Debug::ourDebugImplementation = &instance;
	}

	static void Detach()
	{
		TracyDebugLog& instance = static_cast<TracyDebugLog&>(*Atrium::Debug::ourDebugImplementation);

		Atrium::Debug::ourDebugImplementation = instance.ourParentHandler;
		instance.ourParentHandler = nullptr;
	}

	void Log(LogType aType, const std::string& aMessage) override
	{
		tracy::MessageSeverity severity = tracy::MessageSeverity::Info;
		uint32_t color = 0;
		switch (aType)
		{
			case LogType::Information:
				severity = tracy::MessageSeverity::Info;
				break;
			case LogType::Warning:
				severity = tracy::MessageSeverity::Warning;
				color = 0xFFFF80;
				break;
			case LogType::Error:
				severity = tracy::MessageSeverity::Error;
				color = 0xFF8080;
				break;
			case LogType::Fatal:
				severity = tracy::MessageSeverity::Fatal;
				color = 0xFF0000;
				break;
		}

		TracyLogString(severity, color, TRACY_CALLSTACK, aMessage.size(), aMessage.c_str());
		TracyDebugLog::ourParentHandler->Log(aType, aMessage);
	}

	void Log(LogType aType, const std::wstring& aMessage) override
	{
		tracy::MessageSeverity severity = tracy::MessageSeverity::Info;
		uint32_t color = 0;
		switch (aType)
		{
			case LogType::Information:
				severity = tracy::MessageSeverity::Info;
				break;
			case LogType::Warning:
				severity = tracy::MessageSeverity::Warning;
				color = 0xFFFF80;
				break;
			case LogType::Error:
				severity = tracy::MessageSeverity::Error;
				color = 0xFF8080;
				break;
			case LogType::Fatal:
				severity = tracy::MessageSeverity::Fatal;
				color = 0xFF0000;
				break;
		}

		std::string convertedVector;
		convertedVector.resize(aMessage.size());

		for (size_t i = 0; i <= aMessage.size(); ++i)
			convertedVector[i] = static_cast<char>(aMessage[i]);

		TracyLogString(severity, color, TRACY_CALLSTACK, convertedVector.size(), convertedVector.c_str());

		TracyDebugLog::ourParentHandler->Log(aType, aMessage);
	}

	void SetThreadName(const std::string& aThreadName) override
	{
		TracyDebugLog::ourParentHandler->SetThreadName(aThreadName);
	}

	void SetThreadName(std::thread& aThread, const std::string& aThreadName) override
	{
		TracyDebugLog::ourParentHandler->SetThreadName(aThread, aThreadName);
	}

	void TriggerCrash(const std::string& aMessage) override
	{
		TracyDebugLog::ourParentHandler->TriggerCrash(aMessage);
	}

	void TriggerCrash(const std::wstring& aMessage) override
	{
		TracyDebugLog::ourParentHandler->TriggerCrash(aMessage);
	}

private:
	Atrium::Debug::Impl* ourParentHandler;
};

void TracyDebug_AttachLog()
{
	TracyDebugLog::Attach();
}

void TracyDebug_DetachLog()
{
	TracyDebugLog::Detach();
}
#endif
