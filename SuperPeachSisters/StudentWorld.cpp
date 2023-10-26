#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <sstream>
#include <iomanip>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr), m_lev(assetPath), m_levelWon(false), m_gameWon(false)
{}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << getLevel() << ".txt";
    string file = oss.str();

    
    Level::LoadResult result = m_lev.loadLevel(file);
    if (result == Level::load_fail_file_not_found)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success)
    {
        for (int i = 0; i < GRID_WIDTH; i++)
            for (int j = 0; j < GRID_HEIGHT; j++)
            {
                switch (m_lev.getContentsOf(i, j))
                {
                case Level::empty:
                    break;
                case Level::peach:
                    m_player = new Peach(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT);
                    break;
                case Level::koopa:
                    addActor(new Koopa(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Level::goomba:
                    addActor(new Goomba(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Level::piranha:
                    addActor(new Piranha(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Level::block:
                    addActor(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Level::star_goodie_block:
                    addActor(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, Block::star));
                    break;
                case Level::mushroom_goodie_block:
                    addActor(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, Block::mushroom));
                    break;
                case Level::flower_goodie_block:
                    addActor(new Block(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, Block::flower));
                    break;
                case Level::pipe:
                    addActor(new Pipe(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Level::flag:
                    addActor(new LevelEnder(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, false, IID_FLAG));
                    break;
                case Level::mario:
                    addActor(new LevelEnder(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, true, IID_MARIO));
                    break;
                }
            }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_player != nullptr)
        m_player->doSomething();

    for (list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if (*p != nullptr && !(*p)->isDead())
            (*p)->doSomething();

        if (m_player != nullptr && m_player->isDead())
        {
            playSound(SOUND_PLAYER_DIE);
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }

        if (m_levelWon)
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }

        if (m_gameWon)
        {
            playSound(SOUND_GAME_OVER);
            return GWSTATUS_PLAYER_WON;
        }
    }

    for (list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if ((*p) != nullptr && (*p)->isDead())
        {
            delete* p;
            *p = nullptr;
        }
    }

    updateStatusLine();

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (m_player != nullptr)
    {
        delete m_player;
        m_player = nullptr;
    }

    if (!m_actors.empty())
    {
        for (list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        {
            if ((*p) != nullptr)
            {
                delete *p;
                *p = nullptr;
            }
        }

        m_actors.clear();
    }

    m_levelWon = false;
}

void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
}

void StudentWorld::endLevel(bool isGameWon)
{
    if (isGameWon)
        m_gameWon = true;
    else
        m_levelWon = true;
}

bool StudentWorld::moveOrBonk(Actor* a, char dir) const
{
    bool moved = false;
    switch (dir)
    {
    case 'L':
    {
        a->setDirection(180);
        if (moveIfPossible(a, a->getX() - (SPRITE_WIDTH / 2), a->getY()))
        {
            moved = true;
        }
        else
        {
            if (a == m_player)
            {
                for (list<Actor*>::const_iterator p = m_actors.begin(); p != m_actors.end(); p++)
                    if (*p != nullptr && (*p)->getX() == a->getX() - SPRITE_WIDTH && (*p)->getY() == a->getY())
                        (*p)->getBonked(m_player->isInvincible());
            }
        }
        break;
    }
    case 'R':
    {
        a->setDirection(0);
        if (moveIfPossible(a, a->getX() + (SPRITE_WIDTH / 2), a->getY()))
        {
            moved = true;
        }
        else
        {
            if (a == m_player)
            {
                for (list<Actor*>::const_iterator p = m_actors.begin(); p != m_actors.end(); p++)
                    if (*p != nullptr && (*p)->getX() == a->getX() + SPRITE_WIDTH && (*p)->getY() == a->getY())
                        (*p)->getBonked(m_player->isInvincible());
            }
        }
        break;
    }
    case 'U':
    {
        if (moveIfPossible(a, a->getX(), a->getY() + (SPRITE_WIDTH / 2)))
        {
            moved = true;
        }
        else
        {
            if (a == m_player)
            {
                int i = m_player->getX();
                if (i % SPRITE_WIDTH == 0)
                {
                    for (list<Actor*>::const_iterator p = m_actors.begin(); p != m_actors.end(); p++)
                        if (*p != nullptr && (*p)->getX() == a->getX() && (*p)->getY() == a->getY() + SPRITE_HEIGHT)
                            (*p)->getBonked(m_player->isInvincible());
                }
                else
                {
                    for (list<Actor*>::const_iterator p = m_actors.begin(); p != m_actors.end(); p++)
                        if ((*p != nullptr && (*p)->getX() == a->getX() - (SPRITE_WIDTH / 2) && (*p)->getY() == a->getY() + SPRITE_HEIGHT) || (*p != nullptr && (*p)->getX() == a->getX() + (SPRITE_WIDTH / 2) && (*p)->getY() == a->getY() + SPRITE_HEIGHT))
                            (*p)->getBonked(m_player->isInvincible());
                }
            }
        }
        break;
    }
    case 'D':
    {
        if (moveIfPossible(a, a->getX(), a->getY() - (SPRITE_WIDTH / 2)))
        {
            moved = true;
        }
        else
        {

        }
        break;
    }
    }

    return moved;
}

bool StudentWorld::isMovePossible(int destx, int desty) const
{
        for (list<Actor*>::const_iterator p = m_actors.begin(); p != m_actors.end(); p++)
        {
            if (*p != nullptr && (*p)->blocksMovement())
            {
                int dx = (*p)->getX() - destx;
                int dy = (*p)->getY() - desty;

                if (dx < 0)
                    dx = -dx;

                if (dy < 0)
                    dy = -dy;

                if (dx < SPRITE_WIDTH && dy < SPRITE_HEIGHT)
                    return false;
            }
        }

    return true;
}

bool StudentWorld::moveIfPossible(Actor* a, int destx, int desty) const
{
    if (a != nullptr)
        if (isMovePossible(destx, desty))
        {
            a->moveTo(destx, desty);
            return true;
        }

    return false;
}

bool StudentWorld::damageOverlappingActor(Actor* damager) const
{
    for (list<Actor*>::const_iterator p = m_actors.begin(); p != m_actors.end(); p++)
    {
        if (*p != nullptr && damager != nullptr && (*p)->isMortal())
        {
            int dx = (*p)->getX() - damager->getX();
            int dy = (*p)->getY() - damager->getY();

            if (dx < 0)
                dx = -dx;

            if (dy < 0)
                dy = -dy;

            if (dx < SPRITE_WIDTH && dy < SPRITE_HEIGHT)
            {
                (*p)->sufferDamageIfDamageable();
                return true;
            }
        }
    }

    return false;
}

bool StudentWorld::overlapsPeach(Actor* a) const
{
    if (m_player != nullptr && a != nullptr)
    {
        int dx = m_player->getX() - a->getX();
        int dy = m_player->getY() - a->getY();

        if (dx < 0)
            dx = -dx;

        if (dy < 0)
            dy = -dy;

        if (dx < SPRITE_WIDTH && dy < SPRITE_HEIGHT)
            return true;
    }
    
        return false;
}

void StudentWorld::setPeachHP(int hp) const
{
    m_player->setHP(hp);
}

void StudentWorld::grantInvincibility(int ticks) const
{
    m_player->gainInvincibility(ticks);
}

void StudentWorld::grantShootPower() const
{
    m_player->gainShootPower();
}

void StudentWorld::grantJumpPower() const
{
    m_player->gainJumpPower();
}

void StudentWorld::updateStatusLine()
{
    ostringstream oss;
    oss.fill('0');
    oss << "Lives: " << getLives() << "  Level: " << setw(2) << getLevel() << "  Points: " << setw(6) << getScore();

    oss.fill(' ');
    if (m_player->isInvincible())
        oss << setw(11) << "StarPower!";

    if (m_player->hasShootPower())
        oss << setw(12) << "ShootPower!";

    if (m_player->hasJumpPower())
        oss << setw(11) << "JumpPower!";

    string text = oss.str();
    setGameStatText(text);
}

void StudentWorld::peachBonk()
{
    for (list<Actor*>::iterator p = m_actors.begin(); p != m_actors.end(); p++)
        if (overlapsPeach(*p))
        {
            (*p)->getBonked(m_player->isInvincible());
        }
}

void StudentWorld::damagePeach()
{
    m_player->getBonked(false);
}

bool StudentWorld::determineLevel(int y)
{
    int i = y - m_player->getY();
    if (i < 0)
        i = -i;

    if (i < 1.5 * SPRITE_HEIGHT)
        return true;

    return false;
}

bool StudentWorld::determineRight(int x)
{
    int i = x - m_player->getX();

    if (i > 0)
        return true;

    return false;
}

bool StudentWorld::determineRange(int x)
{
    int i = x - m_player->getX();

    if (i < 0)
        i = -i;

    if (i < 8 * SPRITE_WIDTH)
        return true;

    return false;
}