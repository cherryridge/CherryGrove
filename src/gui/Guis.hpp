#pragma once
#include <cstdint>

namespace Guis {
	typedef uint32_t u32;

	enum GuiWindow {
		wMainMenu,
		wCopyright,
		wVersion,
		wSaves,
		wContentPacks,
		wSettings,
		wCreateWorld,
		wContentPackDetails,
		wAbout,
		wInventory,
		wDebug,
		
		wCount
	};

	struct WindowInfoCache {
		u32 width;
		u32 height;
		float aspectRatio;
	};

	//For GUI utils to access the cache.
	extern WindowInfoCache cache;

	//Must be called after `Sound::init`.
	void init();

	void setVisible(GuiWindow gui, bool visible = true);
	void render();
}