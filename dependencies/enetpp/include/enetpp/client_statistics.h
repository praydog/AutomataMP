#pragma once

#include <atomic>

namespace enetpp {

	class client_statistics {
	public:
		std::atomic<int> _round_trip_time_in_ms;
		std::atomic<int> _round_trip_time_variance_in_ms;

	public:
		client_statistics()
			: _round_trip_time_in_ms(0)
			, _round_trip_time_variance_in_ms(0) {
		}
	};

}