#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;
class StudentWorld;
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


//Actor
Actor::Actor(StudentWorld* world,int imageID, double startX, double startY,Direction dir,int depth):GraphObject(imageID,startX,startY,dir,depth){
    isalive=true;
    m_world=world;
}

bool Actor::isdamageable() const{
    return false;
}
bool Actor::isAlive ()const{
    return isalive;
}

void Actor::setDead(){
    isalive=false;
}



//mainActor
mainActor::mainActor(StudentWorld* world, int imageID, double startX, double startY,Direction dir,int health):Actor(world, imageID,startX,startY,dir,0){
    m_health=health;
}

bool mainActor::setHealth(int newhealth){
    m_health += newhealth;
    if(m_health<=0){
        setDead();
        return false;
    }
        
    return true;
}
int mainActor::getHealth()const{
    return m_health;
}


//Bacteria
Bacteria::Bacteria(StudentWorld* world,int dam,int imageID, double startX, double startY, int health):mainActor(world,imageID,startX,startY,90,health){
    healthdam=dam;
    foodcount=0;
    movement=0;
    
}

void Bacteria::setDead(){
    Actor::setDead();
    getworld()->decBacteria();
}

void Bacteria::computeBabyCord(double &newx,double &newy){
    if(newx<128)
        newx += SPRITE_RADIUS;
    else if(newx>128)
        newx -= SPRITE_RADIUS;
    
    if(newy<128)
        newy += SPRITE_RADIUS;
    else if(newy>128)
        newy -= SPRITE_RADIUS;
}

void Bacteria::damageOrEat(StudentWorld *sw){
    if(sw->isSoverlap(getX(),getY()))
       sw->damageHealth(healthdam);
    else if(foodcount>=3){
            double newx=getX();
            double newy=getY();
            computeBabyCord(newx,newy);
        giveBirth(sw,newx,newy);
        foodcount=0;
        }
    else if(sw->isFoodOverlap(getX(),getY())){
        foodcount++;
    }
}


void Bacteria::doSamSpec(StudentWorld *sw){
    if(movement>0){
        movement--;
         double px,py;
         getPositionInThisDirection(getDirection(),3,px,py);
         if(sw->bacCanMove(px,py)){
             moveTo(px,py);
         }
         else{
             int randDir=randInt(0,359);
             setDirection(randDir);
             movement=10;
         }
         return;
     }
     //movement distance=0; move to another direction
     double foodx,foody;
     int randDir=randInt(0,359);
     //cannot find food
     if(!sw->findClosestFood(getX(),getY(),foodx,foody)){
         setDirection(randDir);
         movement=10;
         return;
     }
     //can find food
    
     double foodDir=atan((getY()-foody)/(getX()-foodx));
     foodDir = foodDir * 180 / PI;
     if(foodx<getX())
           foodDir += 180;
     double possx,possy;
     getPositionInThisDirection(foodDir,3,possx,possy);
     //will not encounter dirt when moving toward food
     if(sw->bacCanMove(possx,possy)){
          setDirection(foodDir);
         moveTo(possx,possy);
     }
     else{//will encounter dirt when moving toward closest food
        setDirection(randDir);
         movement=10;
     }
}

//Regular Salmonella
RegSam::RegSam(StudentWorld* world,double startX, double startY):Bacteria(world,1,IID_SALMONELLA,startX,startY,4){
    
}

void RegSam:: giveBirth(StudentWorld *sw,double newx, double newy){
    sw->addBacteria(1,newx,newy);
}

void RegSam::doSomething(){
    if(!isAlive())
        return;
     StudentWorld *sw=getworld();
    damageOrEat(sw);
    doSamSpec(sw);

    }


//Aggressive Salmonella
AgSam::AgSam(StudentWorld* world,double startX, double startY):Bacteria(world,2,IID_SALMONELLA,startX,startY,10){
    
}
void AgSam::doSomething(){
    if(!isAlive())
        return;
     StudentWorld *sw=getworld();
    double playerx,playery;
    sw->getPlayerCord(playerx,playery);
    double d=sw->calculate(playerx,playery,getX(),getY());
    if(d<=72){
        double playerDir=atan((getY()-playery)/(getX()-playerx));
        playerDir = playerDir * 180 / PI;
        if(playerx<getX())
              playerDir += 180;
        setDirection(playerDir);
        double newx,newy;
        getPositionInThisDirection(getDirection(),3,newx,newy);
        if(sw->bacCanMove(newx,newy))
        moveForward(3);
      damageOrEat(sw);
        return;
    }
    //d>72
    damageOrEat(sw);
    doSamSpec(sw);
}
void AgSam::giveBirth(StudentWorld *sw,double newx, double newy){
    sw->addBacteria(2,newx,newy);
}


//Ecoli
Ecoli::Ecoli(StudentWorld* world,double startX, double startY):Bacteria(world,4,IID_ECOLI,startX,startY,5){
    
}

void Ecoli::doSomething(){
    if(!isAlive())
        return;
     StudentWorld *sw=getworld();
    damageOrEat(sw);
    double playerx,playery;
    sw->getPlayerCord(playerx,playery);
    double d=sw->calculate(playerx,playery,getX(),getY());
    if(d<=256){
        double playerDir=atan((getY()-playery)/(getX()-playerx));
        playerDir = playerDir * 180 / PI;
        if(playerx<getX())
              playerDir += 180;
        setDirection(playerDir);
        for(int i=0;i<10;i++){
            double px,py;
            getPositionInThisDirection(getDirection(),2,px,py);
            if(sw->bacCanMove(px,py)){
                moveTo(px,py);
                return;
            }
            playerDir += 10;
            setDirection(playerDir);
        }
    }
}

void Ecoli::giveBirth(StudentWorld *sw,double newx, double newy){
    sw->addBacteria(3,newx,newy);
}
//Socrates
Socrates::Socrates(StudentWorld *world):mainActor(world,IID_PLAYER,0,128,0,100){
    n_flameThrowers=5;
    n_sprayChargers=20;
    posangle=180;
}

void Socrates:: moveS(){
    while (posangle < 0)
        posangle += 360;
    posangle = posangle % 360;
    double newX=128+128*cos(posangle * PI/ 180.0);
    double newY=128+128*sin(posangle * PI / 180.0);
    moveTo(newX,newY);
    setDirection(posangle+180);
}

void Socrates::useFlameThrower(){
    n_flameThrowers--;
}
void Socrates::useSprays(){
    n_sprayChargers--;
}
void Socrates::chargeFlameThrower(){
    n_flameThrowers += 5;
}

int Socrates::getFlamesThrowers() const{
    return n_flameThrowers;
}
int Socrates::getSprayChargers()const{
    return n_sprayChargers;
}

int Socrates:: getPosAngle() const{
    return posangle;
}

void Socrates::doSomething(){
    if(!isAlive())
        return;
    StudentWorld *sworld=getworld();
    int val;
    if(sworld->getKey(val)){
    switch(val){
        case KEY_PRESS_SPACE:
            if(getSprayChargers()<=0)
                return;
            sworld->addSpray();
            useSprays();
            sworld->playSound(SOUND_PLAYER_SPRAY);
            break;
        case KEY_PRESS_ENTER:
            if(getFlamesThrowers()<=0)
                return;
            sworld->addFlames();
            useFlameThrower();
            sworld->playSound(SOUND_PLAYER_FIRE);
            break;
        case KEY_PRESS_LEFT://move counterclockwise/down
            posangle += 5;
            moveS();
            break;
        case KEY_PRESS_RIGHT:
            posangle -= 5;
            moveS();
            break;
    }
    }
    else{
        if (n_sprayChargers<20)
        n_sprayChargers++;
    }
}


//Projectile
Projectile::Projectile(StudentWorld* world,int dval,int dis,int imageID, double startX, double startY, Direction dir):Actor(world, imageID,startX,startY,dir,1){
    traveldis=dis;
    damageval=dval;
}


bool Projectile::travel(){
   traveldis -= SPRITE_RADIUS*2;
     moveForward(SPRITE_RADIUS*2);
    if(traveldis<=0){
        setDead();
        return false;}
    
    return true;
}
void Projectile::doSomething(){
    if(!isAlive())
        return;
    StudentWorld *sworld=getworld();
    if(sworld->isWeaponOverlap(getX(),getY(),damageval)){
        setDead();
        return;}
   
        travel();
}

//Static Object
StaticObject::StaticObject(StudentWorld* world,int imageID, double startX, double startY,Direction dir): Actor(world,imageID,startX,startY,dir,1){
    
}
void StaticObject::doSomething(){
    
}


//flame
Flame::Flame(StudentWorld* world,double startX,double startY,Direction dir):Projectile(world,5,32,IID_FLAME,startX,startY,dir){
}



//spray
Spray::Spray(StudentWorld* world,double startX,double startY,Direction dir): Projectile(world,2,112,IID_SPRAY,startX,startY,dir){
    
}

//dirt
dirtpile::dirtpile(StudentWorld* world,double startX,double startY):StaticObject(world,IID_DIRT,startX,startY,0){
}

bool dirtpile::isdamageable()const{
    return true;
}


Food::Food(StudentWorld* world,double startX,double startY):StaticObject(world,IID_FOOD,startX, startY,90){
    
}

LTObject:: LTObject(int level,int pt,StudentWorld* world,int imageID, double startX, double startY):Actor(world,imageID,startX,startY,0,1){
  
    lifetime= max(rand()%(300-10*level), 50);
    bonus=pt;
}



void LTObject:: doSomething(){
    if(!isAlive())
        return;
    StudentWorld *sworld=getworld();
    if(sworld->isSoverlap(getX(),getY())){
        sworld->increaseScore(getpoints());
        setDead();
        dospecific(sworld);
        return;
    }
    decLifetime();
        
}
Fungus::Fungus(int level,StudentWorld* world, double startX, double startY): LTObject(level,-50,world,IID_FUNGUS,startX,startY){
    
}

void Fungus::dospecific(StudentWorld *sw){
    sw->damageHealth(20);
}

RHealthGoodie::RHealthGoodie(int level,StudentWorld* world, double startX, double startY):LTObject(level,250,world,IID_RESTORE_HEALTH_GOODIE,startX,startY){
    
}

void RHealthGoodie::dospecific(StudentWorld *sw){
    sw->playSound(SOUND_GOT_GOODIE);
    sw->restoreHealth();
}

FThrowerGoodie::FThrowerGoodie(int level,StudentWorld* world, double startX, double startY):LTObject(level,300,world,IID_FLAME_THROWER_GOODIE,startX,startY){
    
}

void FThrowerGoodie::dospecific(StudentWorld *sw){
    sw->playSound(SOUND_GOT_GOODIE);
    sw->chargeFlame();
}

ELifeGoodie::ELifeGoodie(int level,StudentWorld* world, double startX, double startY):LTObject(level,500,world,IID_EXTRA_LIFE_GOODIE,startX,startY){
    
    
}
void ELifeGoodie::dospecific(StudentWorld *sw){
    sw->playSound(SOUND_GOT_GOODIE);
    sw->incLives();
}


Pit::Pit(StudentWorld* world,double startX, double startY):Actor(world,IID_PIT,startX,startY,0,1){
    regsam=5;
    agsam=3;
    Ecoli=2;
}

void Pit::setDead(){
    Actor::setDead();
    getworld()->decPits();
}

void Pit::doSomething(){
    StudentWorld* sworld=getworld();
    if(getTotal()<=0){
        sworld->decPits();
        setDead();
        return;
    }
    int rand=randInt(1,50);
    if(rand==1){
        int type=randInt(1,3);
        while(!hasBac(type)){
            type=randInt(1,3);
        }
        sworld->addBacteria(type, getX(),getY());
        if(type==1)
            regsam--;
        else if(type==2)
            agsam--;
        else
           Ecoli--;
        sworld->playSound(SOUND_BACTERIUM_BORN);
    }
}
