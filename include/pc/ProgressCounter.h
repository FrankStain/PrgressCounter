#pragma once


namespace pc{};


#include <pc_config.h>


namespace pc
{
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

			inline const ProgressCounter* GetCounter() const	{ return m_host_counter.get(); };
			inline const bool IsValid() const					{ return m_host_counter != nullptr; };

		private:
			std::shared_ptr<ProgressCounter>	m_host_counter;
		};

		using SharedProgressGoal = std::shared_ptr<ProcessGoal>;

		friend class ProcessGoal;
	
	public:
		inline static auto Create()		{ return std::make_shared<ProgressCounter>(); };

		ProgressCounter() = default;
		ProgressCounter( const ProgressCounter& other ) = default;
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
