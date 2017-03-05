#ifndef WINDOW_H
#define WINDOW_H

#include "Renderer.h"

class Window {
public:
	Window();
	~Window();
	
	inline int getScreenWidth();
	inline int getScreenHeight();
	
	bool failed();
	
	Renderer* getRenderer();
	
	void showError(const char* title, const char* message);
	
private:
	SDL_Window* window;
	Renderer* renderer;
};

#endif /* WINDOW_H */

