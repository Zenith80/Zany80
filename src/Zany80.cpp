#include <Zany80/Zany80.hpp>
#include <iostream>
#include <cstring>

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
	#include <direct.h>
	#define GetCurrentDir _getcwd
#else
	#include <unistd.h>
	#define GetCurrentDir getcwd
#endif

typedef void (*init_t)(liblib::Library *);

bool Zany80::attemptLoad(std::string name, liblib::Library **library) {
	if (*library != nullptr) {
		delete *library;
		*library = nullptr;
	}
	try {
		*library = new liblib::Library(folder + name);
		((init_t)(**library)["init"])(*library);
	}
	catch (std::exception &e) {
		if (*library != nullptr) {			
			delete *library;
			*library = nullptr;
		}
		return false;
	}
	return true;
}

Zany80::Zany80(){
	zany = this;
	window = new sf::RenderWindow(sf::VideoMode(LCD_WIDTH,LCD_HEIGHT),"Zany80 IDE");
	window->setFramerateLimit(60);
	if (!font.loadFromFile(folder + "font.png")) {
		// tries a few different options for the path
		if (!font.loadFromFile(absolutize(folder + "/../font.png")) && !font.loadFromFile(absolutize(folder + "../../font.png")) &&
		!font.loadFromFile("font.png") && !font.loadFromFile("../font.png")) {
			std::cerr << "Failed to load font!\n";
			exit(1);
		}
		else {
			// If the font is in this folder, everything else should be as well
			// Note: this only happens when the font is here AND ALSO NOT where it should be
			std::cout << "[Zany80] " << folder << " setup invalid, falling back to ";
			folder =  path.substr(0,path.find_last_of("/")+1);
			std::cout << folder << "\n";
		}
	}
	char *working_directory = new char[FILENAME_MAX];
	if (GetCurrentDir(working_directory, FILENAME_MAX)) {
		true_folder = absolutize(working_directory + (std::string)"/" + folder);
	}
	else {
		exit(1);
	}
	if (!attemptLoad("plugins/plugin_manager",&plugin_manager)) {
		close("Error loading plugin manager!\n");
	}
	try {
		if ((std::vector<std::string>*)(*plugin_manager)["enumerate_plugins"]() == nullptr) {
			close("No plugins found! This is probably a problem with the installation.\n");
		}
	}
	catch (liblib::SymbolLoadingException &e) {
		close("Error detecting plugins!\n");
	}
	// At this point, all detected plugins have been validated. In addition, there is at minimum one plugin.
	// Get default runner from the plugin manager. The plugin manager decides what runner to choose
	try {
		if ((runner = (liblib::Library*)(*plugin_manager)["getDefaultRunner"]()) == nullptr) {
			close("Unable to find suitable runner.\n");
		}
	}
	catch (std::exception &e) {
		close("Invalid plugin manager.\n");
	}
}

Zany80::~Zany80(){
	delete window;
	try {
		(*plugin_manager)["cleanup"]();
	}
	catch (std::exception &e) {
		std::cerr << "[Crash handler] Error cleaning up plugins\n";
	}
	delete plugin_manager;
}

void Zany80::run(){
	while (window->isOpen()) {
		frame();
	}
}

void Zany80::frame(){
	sf::Event e;
	while (window->pollEvent(e)) {
		switch (e.type) {
			case sf::Event::Closed:
				close();
				break;
			default:
				try {
					((void(*)(sf::Event&))((*runner)["event"]))(e);
				}
				catch (std::exception) {
					replaceRunner();
				}
				break;
		}
	}
	try {
		(*runner)["run"]();
	}
	catch (std::exception) {
		replaceRunner();
	}
	window->display();
}

void Zany80::setRunner(liblib::Library *runner) {
	if (this->runner !=nullptr) {
		try {
			((void(*)(PluginMessage))(*this->runner)["postMessage"])({
				0, "deactivate", (int)strlen("deactivate"), "Zany80", nullptr
			});
		}
		catch (...){}
	}
	this->runner = runner;
}

Zany80 *zany;
