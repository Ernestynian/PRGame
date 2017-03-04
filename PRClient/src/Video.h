#ifndef VIDEO_H
#define VIDEO_H

class Video {
public:
	Video();
	~Video();
	
	inline int getScreenWidth();
	inline int getScreenHeight();
	
	bool failed();
	
	void render();
	void showError(const char* title, const char* message);
	
private:
	SDL_Window* window;
};

#endif /* VIDEO_H */

