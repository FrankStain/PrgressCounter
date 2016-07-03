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
		/// @brief	Representation of abstract progress goal.
		class ProcessGoal final
		{
		public:
			ProcessGoal() = default;
			ProcessGoal( const ProcessGoal& other ) = delete;
			ProcessGoal( ProcessGoal&& other ) = delete;
			ProcessGoal( const ProgressCounter* counter );
			~ProcessGoal();

			/// @brief	Returns the progress counter this goal corresponded. The counter may be used to produce new goals.
			/// @retval	nullptr	Returned by invalid goal.
			inline const ProgressCounter* GetHostCounter() const	{ return m_host_counter.get(); };

			/// @brief	Check this goal is valid. Goal without counter considered invalid.
			inline const bool IsValid() const						{ return m_host_counter != nullptr; };

		private:
			std::shared_ptr<ProgressCounter>	m_host_counter;	// The host counter for this goal.
		};

		/// @brief	Regular shared goal of progress.
		using SharedProgressGoal = std::shared_ptr<ProcessGoal>;

		// To allow the use of protected functions by goals.
		friend class ProcessGoal;

	public:
		/// @brif	Regular construction function.
		inline static auto Create()		{ return std::make_shared<ProgressCounter>(); };

		ProgressCounter() = default;
		ProgressCounter( const ProgressCounter& other ) = delete;
		ProgressCounter( ProgressCounter&& other ) = delete;

		/// @brief	Reset the progress and renew the counter of goals.
		/// Function is thread-safe.
		void Reset();

		/// @brief	Produce the new goal of progress.
		/// Function is thread-safe.
		SharedProgressGoal ProduceGoal() const;

		/// @brief	Get the percent of finished goals.
		/// @return	`0.0` if nothing is finished yet, `1.0` if all goals reached.
		inline const float GetFinisedPercent() const	{ return 1.0f - m_remained_percent; };

		/// @brief	Get the percent of remained goals.
		/// @return	`1.0` if nothing is finished yet, `0.0` if all goals reached.
		inline const float GetRemainsPercent() const	{ return m_remained_percent; };

	protected:
		// Used by goals to update the state of counter.
		void OnProgressGoalReached();

	private:
		float						m_remained_percent	= 1.0f;	// The remained value of progress.
		mutable int32_t				m_goals_count		= 0;	// Number of currently active goals.
		mutable ConcurrencyLatch	m_latch;					// The concurrency latch.
	};
};
