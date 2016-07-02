#pragma once


/// @brief	Progress counter.
namespace pc{};


#include <pc_config.h>


namespace pc
{
	/**
		@brief		Progress counter.
		@note		Use `ProgressCounter::Create()` to easily obtain the instance of counter.
		@warning	Inherited from `std::enable_shared_from_this`, any allocation out from `std::shared_ptr` considered ill-formed.
		This counter helps easily control the progress of some tasks.
		The range of progress is placed in [0.0-1.0] (inclusive) range. This range will be divided between count of goals.
		Each goal represents the finishing stage of some concurrent or long-running task.
		The progress goal must be delegated into some task, live while task remains unfinished, and destroyed after task finished.
		Each progress goal is designed as shared object, so it's pretty easy to hold it inside of task.
		In same time the tasks operating, the progress counter itself can be polled for the value of current progress.
		All is thread safe. The polling of counter, the goals creation and completion. Every operation is concurrently safe.
	*/
	class ProgressCounter final : public std::enable_shared_from_this<ProgressCounter>
	{
	public:
		class ProcessGoal final
		{
		public:
			ProcessGoal() = default;
			ProcessGoal( const ProcessGoal& other ) = delete;
			ProcessGoal( ProcessGoal&& other ) = delete;
			ProcessGoal( const ProgressCounter* counter );
			~ProcessGoal();

			inline const ProgressCounter* GetHostCounter() const	{ return m_host_counter.get(); };
			inline const bool IsValid() const						{ return m_host_counter != nullptr; };

		private:
			std::shared_ptr<ProgressCounter>	m_host_counter;
		};

		using SharedProgressGoal = std::shared_ptr<ProcessGoal>;

		friend class ProcessGoal;
	
	public:
		inline static auto Create()		{ return std::make_shared<ProgressCounter>(); };

		ProgressCounter() = default;
		ProgressCounter( const ProgressCounter& other ) = delete;
		ProgressCounter( ProgressCounter&& other ) = delete;

		void Reset();

		SharedProgressGoal ExtractProgressPoint() const;

		inline const float GetFinisedPercent() const	{ return 1.0f - m_remains_part; };
		inline const float GetRemainsPercent() const	{ return m_remains_part; };

	protected:
		void OnProgressGoalReached();

	private:
		float						m_remains_part	= 1.0f;
		mutable int32_t				m_goals_count	= 0;
		mutable ConcurrencyLatch	m_latch;
	};
};
