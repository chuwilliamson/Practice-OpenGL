#include "_03___Rendering_GeometryApp.h"

int main() {
	
	// allocation
	auto app = new _03___Rendering_GeometryApp();

	// initialise and loop
	app->run("AIE", 1280, 720, false);

	// deallocation
	delete app;

	return 0;
}