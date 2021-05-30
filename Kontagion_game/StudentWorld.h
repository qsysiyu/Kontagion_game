#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
#include "Actor.h"
class Socrates;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addFlames();
    void addSpray();
    bool isSoverlap(double x1, double y1);
    bool isWeaponOverlap(double x1, double y1,int damval);
    void damageHealth(int damage);
    void chargeFlame();
    void restoreHealth();
    bool isFoodOverlap(double x,double y);
    bool bacCanMove(double x,double y);
    void decPits(){
        pits--;
    }
    void decBacteria(){
        n_bacteria--;
    }
    void addBacteria(int type, double x, double y);
    void getPlayerCord(double &x,double &y);
    bool findClosestFood(double mx,double my,double &x,double &y);
    double calculate(double x1, double y1, double x2, double y2);

private:
    std::list<Actor*> actors;
    Socrates* player;
    int n_bacteria, pits;
    bool initoverlap(double x1, double y1,bool b);
    void generateCord(double &x, double &y, int r);
    void newActorCord(double &x, double &y);
    void removeDeadObj();
    std::string updateText();
    void addNewActors();
};

#endif // STUDENTWORLD_H_
