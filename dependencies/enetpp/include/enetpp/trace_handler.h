#ifndef ENETPP_TRACE_HANDLER_H_
#define ENETPP_TRACE_HANDLER_H_

#include <functional>
#include <string>

namespace enetpp {

	//!IMPORTANT! handler must be thread safe as trace messages come from worker threads as well.
	using trace_handler = std::function<void(const std::string&)>;

}

#endif
