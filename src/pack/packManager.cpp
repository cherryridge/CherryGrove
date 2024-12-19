#include <iostream>

#include "V8Wrapper.hpp"
#include "J4mcWrapper.hpp"
#include "PackManager.hpp"

namespace PackManager {
	const char* workingDirectory;

	void init(const char* _workingDirectory) {
		V8Wrapper::init(_workingDirectory);
		J4mcWrapper::test();
		J4mcWrapper::parseManifest("test/packs/manifest.json");
	}

	void shutdown() {
		V8Wrapper::shutdown();
	}

}