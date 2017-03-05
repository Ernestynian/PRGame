#ifndef PLAYER_H
#define PLAYER_H

class Player {
public:
	Player(Texture* texture);
	virtual ~Player();
	
	void draw();
	
private:
	int x;
	int y;
	
	Texture* texture;
};

#endif /* PLAYER_H */

