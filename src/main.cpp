#include "game.hpp"

#include "Utilities/debug.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32
#include <stdexcept>
#include <string>

void showErrorMessageBox(const std::string& title, const std::string& message)
{
#ifdef _WIN32
	MessageBoxA(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
#endif // _WIN32
}

int main()
{
	try {
		PH_LOG(ph::LogType::Info, "start executing PopHead!");
		ph::Game game;
		game.run();
	}
	catch (const std::exception& e) {
		PH_LOG(ph::LogType::Exception, e.what());
		showErrorMessageBox("Error", e.what());
		throw;
	}
	catch (...) {
		PH_LOG(ph::LogType::UnhandledException, "Unknown error occurred!");
		showErrorMessageBox("Error", "Unknown error occurred!");
		throw;
	}
	return 0;
}
