#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, int x, int y, int dir = right, int depth = 0);

      // Action to perform for each tick.
    void doSomething()
    {
        if (!m_dead)
            doSomethingAux();
    }

      // Bonk this actor.  Parameter says whether bonker is Peach with invincibiity.
    virtual void getBonked(bool bonkerIsInvinciblePeach)
    {}
    
      // Do what the spec says happens when damage is inflicted on this actor.
    virtual void sufferDamageIfDamageable(){}
    
      // Mark this actor as dead.
    void setDead();

      // Is this actor dead?
    bool isDead() const;

      // Does this actor block movement?
    virtual bool blocksMovement() const;

      // Get this actor's world
    StudentWorld* world() const;

      // Set destx and desty to the coordinates dist pixels away in direction
      // dir from this actor's position.
    void converDirectionAndDistanceToXY(int dir, int dist, int& destx, int& desty) const;

    virtual bool isMortal()
    {
        return false;
    }

    virtual void animate()
    {}

private:
    virtual void doSomethingAux() = 0;
    bool m_dead;
    StudentWorld* m_world;
};

class Peach : public Actor
{
public:
    Peach(StudentWorld* w, int x, int y);
    virtual void getBonked(bool bonkerIsInvinciblePeach);
    virtual void sufferDamageIfDamageable();
    
      // Set Peach's hit points.
    void setHP(int hp);
    
      // Grant Peach invincibility for this number of ticks.
    void gainInvincibility(int ticks);
    
      // Grant Peach Shoot Power.
    void gainShootPower();

      // Grant Peach Jump Power.
    void gainJumpPower();
    
      // Is Peach invincible?
    bool isInvincible() const;
    
      // Does Peach have Shoot Power?
    bool hasShootPower() const;

      // Does Peach have Jump Power?
    bool hasJumpPower() const;

    bool isRecharging() const;

    bool isImmune() const;

private:
    virtual void doSomethingAux();
    int m_health;
    int m_starTick;
    int m_shootTick;
    int m_jumpDistance;
    int m_immuneTick;
    bool m_jumpPower;
    bool m_shootPower;
    bool m_jumping;
};

// (Further private sections won't be shown.)

class Obstacle : public Actor
{
public:
    Obstacle(StudentWorld* w, int imageID, int x, int y);
    virtual bool blocksMovement() const;
private:
    virtual void doSomethingAux() = 0;
};

class Block : public Obstacle
{
public:
    enum GoodieType { none, flower, mushroom, star };

    Block(StudentWorld* w, int x, int y, GoodieType g = none);
    virtual void getBonked(bool bonkerIsInvinciblePeach);
private:
    virtual void doSomethingAux();
    GoodieType m_goodie;
    bool m_released;
};

class Pipe : public Obstacle
{
public:
    Pipe(StudentWorld* w, int x, int y);
private:
    virtual void doSomethingAux();
};

class LevelEnder : public Actor
{
public:
    LevelEnder(StudentWorld* w, int x, int y, bool isGameEnder, int imageID);
private:
    virtual void doSomethingAux();
    bool m_end;
};

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* w, int imageID, int x, int y);
    virtual void animate();
private:
    virtual void doSomethingAux() = 0;
};

class Flower : public Goodie
{
public:
    Flower(StudentWorld* w, int x, int y);
private:
    virtual void doSomethingAux();
};

class Mushroom : public Goodie
{
public:
    Mushroom(StudentWorld* w, int x, int y);
private:
    virtual void doSomethingAux();
};

class Star : public Goodie
{
public:
    Star(StudentWorld* w, int x, int y);
private:
    virtual void doSomethingAux();
};

class Projectile : public Actor
{
public:
    Projectile(StudentWorld* w, int imageID, int x, int y, int dir);
    virtual void animate();
private:
    virtual void doSomethingAux() = 0;
};

class PiranhaFireball : public Projectile
{
public:
    PiranhaFireball(StudentWorld* w, int x, int y, int dir);
private:
    virtual void doSomethingAux();
};

class PeachFireball : public Projectile
{
public:
    PeachFireball(StudentWorld* w, int x, int y, int dir);
private:
    virtual void doSomethingAux();
};

class Shell : public Projectile
{
public:
    Shell(StudentWorld* w, int x, int y, int dir);
private:
    virtual void doSomethingAux();
    int it;
};

class Enemy : public Actor
{
public:
    Enemy(StudentWorld* w, int imageID, int x, int y);
    virtual void getBonked(bool bonkerIsInvinciblePeach);
    virtual void sufferDamageIfDamageable();
    virtual bool isMortal()
    {
        return true;
    }
    virtual void animate();
private:
    virtual void doSomethingAux() = 0;
};

class Goomba : public Enemy
{
public:
    Goomba(StudentWorld* w, int x, int y);
private:
    virtual void doSomethingAux();
};

class Koopa : public Enemy
{
public:
    Koopa(StudentWorld* w, int x, int y);
    virtual void getBonked(bool bonkerIsInvinciblePeach);
    virtual void sufferDamageIfDamageable();
private:
    virtual void doSomethingAux();
};

class Piranha : public Enemy
{
public:
    Piranha(StudentWorld* w, int x, int y);
private:
    virtual void doSomethingAux();
    int m_delay;
};

#endif // ACTOR_INCLUDED