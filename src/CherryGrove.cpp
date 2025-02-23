#include <atomic>
#include <glfw/glfw3.h>

#include "debug/debug.hpp"
#include "gui/MainWindow.hpp"
#include "graphic/Renderer.hpp"
#include "sound/Sound.hpp"
#include "gui/Guis.hpp"
#include "pack/PackManager.hpp"
#include "CherryGrove.hpp"

namespace CherryGrove {
	using std::atomic;

	atomic<bool> isCGAlive(false);

	static void hold();
	static void exit();

	void launch() {
		isCGAlive = true;
		lout << "Main" << flush;

	//Initialize libraries
		lout << "Setting up CherryGrove window..." << endl;
		MainWindow::initGlfw(2560, 1440, "CherryGrove");
		MainWindow::loadIcon("assets/icons/CherryGrove-trs-64.png");

		lout << "Initialzing input handler..." << endl;
		MainWindow::initInputHandler();

		lout << "Initializing bgfx & ImGui..." << endl;
		Renderer::start();

		lout << "Initializing SoLoud..." << endl;
		Sound::init();
		Sound::test();

		lout << "Initialzing GUI..." << endl;
		Guis::init();

		lout << "Initializing pack manager..." << endl;
		PackManager::init();
		
	//Set up main menu
		Guis::setVisible(Guis::wMainMenu);
		Guis::setVisible(Guis::wCopyright);
		Guis::setVisible(Guis::wVersion);

		hold();
	}

	//Main loop.
	//Set `isCGAlive` to `false` directly to exit the program.
	//Check for condition updates in `MainWindow::update()` instead.
	static void hold() {
		while (isCGAlive) { MainWindow::update(); }
		exit();
	}

	static void exit() {
		Sound::shutdown();
		Renderer::waitShutdown();
		PackManager::shutdown();
		MainWindow::close();
		Logger::shutdown();
	}
}