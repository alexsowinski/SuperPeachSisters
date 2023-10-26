#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(StudentWorld* sw, int imageID, int x, int y, int dir, int depth)
	:GraphObject(imageID, x, y, dir, depth), m_dead(false), m_world(sw)
{}

void Actor::setDead()
{
	m_dead = true;
}

bool Actor::isDead() const
{
	return m_dead;
}

bool Actor::blocksMovement() const
{
	return false;
}

StudentWorld* Actor::world() const
{
	return m_world;
}

Peach::Peach(StudentWorld* w, int x, int y)
    :Actor(w, IID_PEACH, x, y), m_starTick(0), m_jumpPower(false), m_shootPower(false), m_health(1), m_jumpDistance(0), m_shootTick(0), m_jumping(false), m_immuneTick(0)
{}

void Peach::getBonked(bool bonkerIsInvinciblePeach)
{
    sufferDamageIfDamageable();
}

void Peach::sufferDamageIfDamageable()
{
    if (!isInvincible() && !isImmune())
    {
        m_health--;
        m_shootPower = false;
        m_jumpPower = false;
        m_immuneTick = 10;

        if (m_health == 1)
            world()->playSound(SOUND_PLAYER_HURT);
        else if (m_health == 0)
            setDead();
    }
}

void Peach::setHP(int hp)
{
    m_health = hp;
}

void Peach::gainInvincibility(int ticks)
{
    m_starTick = ticks;
}

void Peach::gainShootPower()
{
    m_shootPower = true;
}

void Peach::gainJumpPower()
{
    m_jumpPower = true;
}

bool Peach::isInvincible() const
{
    if (m_starTick > 0)
        return true;

    return false;
}

bool Peach::isImmune() const
{
    if (m_immuneTick > 0)
        return true;

    return false;
}

bool Peach::hasShootPower() const
{
    return m_shootPower;
}

bool Peach::hasJumpPower() const
{
    return m_jumpPower;
}

bool Peach::isRecharging() const
{
    if (m_shootTick > 0)
        return true;

    return false;
}

void Peach::doSomethingAux()
{   
        if (isDead())
            return;

        if (isInvincible())
            m_starTick--;

        if (isImmune())
            m_immuneTick--;

        if (isRecharging())
            m_shootTick--;
    
        
        world()->peachBonk();

        if (m_jumpDistance > 0)
        {
            if (world()->moveOrBonk(this, 'U'))
                m_jumpDistance -= 1;
            else
                m_jumpDistance = 0;
        }

        if (!m_jumping)
                world()->moveOrBonk(this, 'D');

        int input;
        if (world()->getKey(input))
            switch (input)
            {
            case KEY_PRESS_LEFT:
                world()->moveOrBonk(this, 'L');
                break;
            case KEY_PRESS_RIGHT:
                world()->moveOrBonk(this, 'R');
                break;
            case KEY_PRESS_UP:
                if (!world()->isMovePossible(getX(), getY() - (SPRITE_WIDTH / 2)))
                {
                    m_jumping = true;
                    if (hasJumpPower())
                        m_jumpDistance = 12;
                    else
                        m_jumpDistance = 8;
                    world()->playSound(SOUND_PLAYER_JUMP);
                }
                break;
            case KEY_PRESS_SPACE:
                if (hasShootPower() && !isRecharging())
                {
                    world()->playSound(SOUND_PLAYER_FIRE);
                    m_shootTick = 8;
                    if (getDirection() == 0)
                        world()->addActor(new PeachFireball(world(), getX() + (SPRITE_WIDTH / 2), getY(), 0));
                    if (getDirection() == 180)
                        world()->addActor(new PeachFireball(world(), getX() - (SPRITE_WIDTH / 2), getY(), 180));
                }
                break;
            }

        if (m_jumpDistance == 0)
            m_jumping = false;
}

Obstacle::Obstacle(StudentWorld* w, int imageID, int x, int y)
	:Actor(w, imageID, x, y, 0, 2)
{}

bool Obstacle::blocksMovement() const
{
	return true;
}

Block::Block(StudentWorld* w, int x, int y, GoodieType g)
	:Obstacle(w, IID_BLOCK, x, y), m_goodie(g), m_released(false)
{}

void Block::getBonked(bool bonkerIsInvinciblePeach)
{
    if (m_goodie == none)
        world()->playSound(SOUND_PLAYER_BONK);
    else
    {
        if (!m_released)
        {
            world()->playSound(SOUND_POWERUP_APPEARS);

            if (m_goodie == star)
                world()->addActor(new Star(world(), getX(), getY() + SPRITE_HEIGHT));
            else if (m_goodie == mushroom)
                world()->addActor(new Mushroom(world(), getX(), getY() + SPRITE_HEIGHT));
            else if (m_goodie == flower)
                world()->addActor(new Flower(world(), getX(), getY() + SPRITE_HEIGHT));

            m_released = true;
        }
    }
}

void Block::doSomethingAux()
{}

Pipe::Pipe(StudentWorld* w, int x, int y)
	:Obstacle(w, IID_PIPE, x, y)
{}

void Pipe::doSomethingAux()
{}

LevelEnder::LevelEnder(StudentWorld* w, int x, int y, bool isGameEnder, int imageID)
	:Actor(w, imageID, x, y, 0, 1), m_end(isGameEnder)
{}

void LevelEnder::doSomethingAux()
{
    if (!isDead())
    {
        if (world()->overlapsPeach(this))
        {
            world()->increaseScore(1000);
            setDead();
            world()->endLevel(m_end);
        }
    }
}

Goodie::Goodie(StudentWorld* w, int imageID, int x, int y)
	:Actor(w, imageID, x, y, 0, 1)
{}

void Goodie::animate() 
{
    world()->moveIfPossible(this, getX(), getY() - 2);

    if (getDirection() == 0)
    {
        if (!world()->moveIfPossible(this, getX() + 2, getY()))
        {
            setDirection(180);
            return;
        }
    }
    else
    {
        if (!world()->moveIfPossible(this, getX() - 2, getY()))
        {
            setDirection(0);
            return;
        }
    }
}

Flower::Flower(StudentWorld* w, int x, int y)
	:Goodie(w, IID_FLOWER, x, y)
{}

void Flower::doSomethingAux()
{
    if (world()->overlapsPeach(this))
    {
        world()->increaseScore(50);
        world()->grantShootPower();
        world()->setPeachHP(2);
        setDead();
        world()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }

    animate();
}

Mushroom::Mushroom(StudentWorld* w, int x, int y)
	:Goodie(w, IID_MUSHROOM, x, y)
{}

void Mushroom::doSomethingAux()
{
    if (world()->overlapsPeach(this))
    {
        world()->increaseScore(75);
        world()->grantJumpPower();
        world()->setPeachHP(2);
        setDead();
        world()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }

    animate();
}

Star::Star(StudentWorld* w, int x, int y)
	:Goodie(w, IID_STAR, x, y)
{}

void Star::doSomethingAux()
{
    if (world()->overlapsPeach(this))
    {
        world()->increaseScore(100);
        world()->grantInvincibility(150);
        world()->setPeachHP(2);
        setDead();
        world()->playSound(SOUND_PLAYER_POWERUP);
        return;
    }

    animate();
}

Projectile::Projectile(StudentWorld* w, int imageID, int x, int y, int dir)
	:Actor(w, imageID, x, y, dir, 1)
{

}

void Projectile::animate()
{
    world()->moveIfPossible(this, getX(), getY() - 2);

    if (getDirection() == 0)
    {
        if (!world()->moveIfPossible(this, getX() + 2, getY()))
        {
            setDead();
            return;
        }
    }
    else
    {
        if (!world()->moveIfPossible(this, getX() - 2, getY()))
        {
            setDead();
            return;
        }
    }
}

PiranhaFireball::PiranhaFireball(StudentWorld* w, int x, int y, int dir)
	:Projectile(w, IID_PIRANHA_FIRE, x, y, dir)
{}

void PiranhaFireball::doSomethingAux()
{
    if (world()->overlapsPeach(this))
    {
        world()->damagePeach();
        setDead();
        return;
    }

    animate();
}

PeachFireball::PeachFireball(StudentWorld* w, int x, int y, int dir)
	:Projectile(w, IID_PEACH_FIRE, x, y, dir)
{}

void PeachFireball::doSomethingAux()
{
    if (world()->damageOverlappingActor(this))
    {
        setDead();
        return;
    }

    animate();
}

Shell::Shell(StudentWorld* w, int x, int y, int dir)
	:Projectile(w, IID_SHELL, x, y, dir), it(1)
{}

void Shell::doSomethingAux()
{
    if (it > 0)
    {
        it--;
        return;
    }

    if (world()->damageOverlappingActor(this))
    {
        setDead();
        return;
    }

    animate();
}

Enemy::Enemy(StudentWorld* w, int imageID, int x, int y)
	:Actor(w, imageID, x, y)
{
    if (x < 128)
        setDirection(0);
    else
        setDirection(180);
}

void Enemy::getBonked(bool bonkerIsInvinciblePeach)
{
    if(bonkerIsInvinciblePeach)
    {
        world()->playSound(SOUND_PLAYER_KICK);
        world()->increaseScore(100);
        setDead();
    }
}

void Enemy::sufferDamageIfDamageable()
{
    world()->increaseScore(100);
    setDead();
}

void Enemy::animate()
{
    if (getDirection() == 0)
    {
        int i = getX();
        if (i % SPRITE_WIDTH == 0)
        {
            if (!world()->isMovePossible(getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT))
            {
                if (!world()->moveIfPossible(this, getX() + 1, getY()))
                {
                    setDirection(180);
                    return;
                }
            }
            else
            {
                setDirection(180);
            }
        }
        else
        {
            if (!world()->moveIfPossible(this, getX() + 1, getY()))
            {
                setDirection(180);
                return;
            }
        }
    }
    else
    {
        int i = getX();
        if (i % SPRITE_WIDTH == 0)
        {
            if (!world()->isMovePossible(getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT))
            {
                if (!world()->moveIfPossible(this, getX() - 1, getY()))
                {
                    setDirection(0);
                    return;
                }
            }
            else
            {
                setDirection(0);
            }
        }
        else
        {
            if (!world()->moveIfPossible(this, getX() - 1, getY()))
            {
                setDirection(0);
                return;
            }
        }
    }
}

Goomba::Goomba(StudentWorld* w, int x, int y)
	:Enemy(w, IID_GOOMBA, x, y)
{}

void Goomba::doSomethingAux()
{
    if (isDead())
        return;

    if (world()->overlapsPeach(this))
    {
        world()->damagePeach();
        return;
    }

    animate();
}

Koopa::Koopa(StudentWorld* w, int x, int y)
    : Enemy(w, IID_KOOPA, x, y)
{}

void Koopa::doSomethingAux()
{
    if (isDead())
        return;

    if (world()->overlapsPeach(this))
    {
        world()->damagePeach();
        return;
    }

    animate();
}

void Koopa::getBonked(bool bonkerIsInvinciblePeach)
{
    if (bonkerIsInvinciblePeach)
    {
        world()->playSound(SOUND_PLAYER_KICK);
        world()->increaseScore(100);
       
        
        if (getDirection() == 0)
        {
            world()->addActor(new Shell(world(), getX(), getY(), 0));
        }
        else
        {
            world()->addActor(new Shell(world(), getX(), getY(), 180));
        }

        setDead();
    }
}

void Koopa::sufferDamageIfDamageable()
{
    world()->increaseScore(100);

    if (getDirection() == 0)
    {
        world()->addActor(new Shell(world(), getX(), getY(), 0));
    }
    else
    {
        world()->addActor(new Shell(world(), getX(), getY(), 180));
    }

    setDead();
}

Piranha::Piranha(StudentWorld* w, int x, int y)
	: Enemy(w, IID_PIRANHA, x, y), m_delay(0)
{}

void Piranha::doSomethingAux()
{
    if (isDead())
        return;

    increaseAnimationNumber();

    if (world()->overlapsPeach(this))
    {
        world()->damagePeach();
        return;
    }

    if (!world()->determineLevel(getY()))
        return;

    if (world()->determineRight(getX()))
        setDirection(180);
    else
        setDirection(0);

    if (m_delay > 0)
    {
        m_delay--;
        return;
    }

    if (world()->determineRange(getX()))
    {
        world()->addActor(new PiranhaFireball(world(), getX(), getY(), getDirection()));
        world()->playSound(SOUND_PIRANHA_FIRE);
        m_delay = 40;
    }
}