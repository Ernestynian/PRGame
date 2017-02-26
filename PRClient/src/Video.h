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
	
private:
	SDL_Window* window;
};

#endif /* VIDEO_H */

