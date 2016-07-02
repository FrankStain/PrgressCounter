#include <pc/ProgressCounter.h>
#include <algorithm>

namespace pc
{
	ProgressCounter::ProcessGoal::ProcessGoal( const ProgressCounter* counter )
	{
		// This is a trick, the `const` qualifier must be removed from shared progress counter.
		m_host_counter = std::const_pointer_cast<ProgressCounter>( counter->shared_from_this() );
	};
	
	ProgressCounter::ProcessGoal::~ProcessGoal()
	{
		if( m_host_counter )
		{
			m_host_counter->OnProgressGoalReached();
		};
	};
	
	void ProgressCounter::Reset()
	{
		ConcurrencyLock lock( m_latch );
		m_remains_part	= 1.0f;
		m_goals_count	= shared_from_this().use_count();
	};

	ProgressCounter::SharedProgressGoal ProgressCounter::ExtractProgressPoint() const
	{
		ConcurrencyLock lock( m_latch );
		++m_goals_count;
		return std::make_shared<ProcessGoal>( this );
	};

	void ProgressCounter::OnProgressGoalReached()
	{
		ConcurrencyLock lock( m_latch );
		auto shared_counter = shared_from_this();

		// Proportionally decrease the part of remained progress.
		m_remains_part = std::max( 0.0f, m_remains_part - m_remains_part / m_goals_count );
		--m_goals_count;
	};
};
