#ifndef STUDENTWORLD_INCLUDED
#define STUDENTWORLD_INCLUDED

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <list>

class Actor;
class Peach;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	virtual ~StudentWorld();

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	// Add an actor to the world.
	void addActor(Actor* a);

	// Record that a level was finished.  The parameter is true if it was
	// the last level.
	void endLevel(bool isGameWon);

	// If a can move to (destx,desty), move it and return true; otherwise
	// bonk the object that's blockig it and return false.
	bool moveOrBonk(Actor* a, char dir) const;

	// If a can move to (destx,desty), move it and return true; otherwise,
	// return false.
	bool moveIfPossible(Actor* a, int destx, int desty) const;

	// If a can move to (destx,destx), return true (but don't move it);
	// otherwise (it would be blocked), return false.
	bool isMovePossible(int destx, int desty) const;

	// If a non-Peach actor overlaps damager, damage that non-Peach actor
	// and return true; otherwise, return false.
	bool damageOverlappingActor(Actor* damager) const;

	// Return true if a overlaps Peach; otherwise, return false.
	bool overlapsPeach(Actor* a) const;

	// Set Peach's hit points to hp.
	void setPeachHP(int hp) const;

	// Grant Peach invincibility for this number of ticks.
	void grantInvincibility(int ticks) const;

	// Grant Peach Shoot Power.
	void grantShootPower() const;

	// Grant Peach Jump Power.
	void grantJumpPower() const;

	void updateStatusLine();

	void peachBonk();

	void damagePeach();
	
	bool determineLevel(int y);

	bool determineRight(int x); 

	bool determineRange(int x);

private:
	Peach* m_player;
	std::list<Actor*> m_actors;
	Level m_lev;
	bool m_levelWon;
	bool m_gameWon;
};

#endif // STUDENTWORLD_INCLUDED