#ifndef ENETPP_GLOBAL_STATE_H_
#define ENETPP_GLOBAL_STATE_H_

#include <assert.h>
#include "enet/enet.h"

namespace enetpp {

	//ugh
	class global_state {
	private:
		bool _is_initialized;

	public:
		static global_state& get() {
			static global_state g;
			return g;
		}

	public:
		bool is_initialized() const {
			return _is_initialized;
		}

		void initialize() {
			assert(!_is_initialized);
			enet_initialize();
			_is_initialized = true;
		}

		void deinitialize() {
			enet_deinitialize();
			_is_initialized = false;
		}

	private:
		global_state()
			: _is_initialized(false) {
		}
	};

}

#endif
