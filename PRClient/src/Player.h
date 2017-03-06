#ifndef PLAYER_H
#define PLAYER_H

class Player {
public:
	Player(Texture* texture);
	virtual ~Player();
	
	void spawn(int x, int y);
	void draw();
	
	bool isAlive();
	
private:
	int x;
	int y;
	
	bool alive;
	
	Texture* texture;
};

#endif /* PLAYER_H */

