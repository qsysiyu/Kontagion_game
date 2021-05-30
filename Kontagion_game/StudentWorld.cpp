#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream> // defines the overloads of the << operator
#include <sstream>
#include <iomanip>
using namespace std;
class Socrates;
GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    
}
StudentWorld::~StudentWorld(){
    cleanUp();
}

int StudentWorld::init()
{
    n_bacteria=0;
    pits=0;
    //add pits
    player=new Socrates(this);
    for(int i=0;i<getLevel();i++){
        double x,y;
        generateCord(x,y,120);
         while(initoverlap(x,y,false)){
             generateCord(x,y,120);
         }
         actors.push_back(new Pit(this,x,y));
        pits++;
    }
    //add food
    for(int i=0;i<min(5*getLevel(),25);i++){
       double x,y;
       generateCord(x,y,120);
        while(initoverlap(x,y,false)){
            generateCord(x,y,120);
        }
        actors.push_back(new Food(this,x,y));
    }
    //add dirts
    for(int i=0;i<max(180-20*getLevel(),20);i++){
        double x,y;
        generateCord(x,y,120);
        while(initoverlap(x,y,true)){
            generateCord(x,y,120);
            }
        actors.push_back(new dirtpile(this,x,y));
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::generateCord(double &x, double &y, int r){
    x=randInt(VIEW_WIDTH/2-r,VIEW_WIDTH/2+r);
    double possy=sqrt(r*r-(x-128)*(x-128));//y must be within the circle
    y=randInt(-possy+128,possy+128);
}

bool StudentWorld:: initoverlap(double x1, double y1,bool b){
    list<Actor*>::iterator it;
    it = actors.begin();
    if(b==false){
    while(it!=actors.end()){
        int d=calculate(x1,y1,(*it)->getX(),(*it)->getY());//food and pit
        if(d<=8)
            return true;
        it++;
    }
        
    }
    else{
        while(it!=actors.end()){
            int d=calculate(x1,y1,(*it)->getX(),(*it)->getY());
            if((*it)->isoverlap()&&d<=8)//for dirt
                return true;
            it++;
        }
    }
    
    return false;
}



int StudentWorld::move()
{
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it!=actors.end()){
        if((*it)->isAlive()){
        (*it)->doSomething();
            if(!player->isAlive()){//player is dead
            decLives();
            return GWSTATUS_PLAYER_DIED;}
            if(n_bacteria<=0&&pits<=0){//completed current level
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;}
        }
        it++;
    }
player->doSomething();
    removeDeadObj();
    addNewActors();
    setGameStatText(updateText());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::addNewActors(){
    int chancefungus=max(510-getLevel()*10,200);
    int rfungus=randInt(0,chancefungus-1);
    
    
    if(rfungus==0){
        double x,y;
        newActorCord(x,y);
        actors.push_back(new Fungus(getLevel(),this,x,y));//add new fungus
    }
    
        int chanceGoodie=max(510-getLevel()*10,200);
        int rgoodie=randInt(0,chanceGoodie-1);
        if(rgoodie==0){
            double x,y;
            newActorCord(x,y);
            int random=randInt(1,10);
            if(random>=1&&random<=6)//add goodie based on possibility
                actors.push_back(new RHealthGoodie(getLevel(),this,x,y));
            else if(random>=7&&random<=9)
                actors.push_back(new FThrowerGoodie(getLevel(),this,x,y));
            else if(random==10)
                actors.push_back(new ELifeGoodie(getLevel(),this,x,y));
        }
    
}

void StudentWorld::newActorCord(double &x, double &y){
    x=randInt(0,255);
    double z=sqrt(128*128-(x-128)*(x-128));
    int rand=randInt(0,1);
    if(rand==0){
        y=z+128;
        if(y==256)//y cannot equal to 256
            y=-z+128;
    }
    else
        y=-z+128;
}
string StudentWorld::updateText(){
    int a=getScore();
    string sign="  ";
    string space = "  ";
    ostringstream ostr;
    if(a<0){
        a=-a;
        sign="  -";
        ostr<<setfill('0')<<setw(5)<<a;//deal with negative sign
    }
    else
        ostr<<setfill('0')<<setw(6)<<a;
    string score=ostr.str();
    ostringstream disp;
    disp<<"Score:"<<sign<<score<<space<<"Level:  "<<getLevel()<<space<<"Lives:  "<<getLives()<<space<<"Health:  "<<player->getHealth()<<space<<"Sprays:  "<<player->getSprayChargers()<<space<<"Flames:  "<<player->getFlamesThrowers();
    string display=disp.str();
    return display;
}

void StudentWorld::removeDeadObj(){
     list<Actor*>::iterator it;
    it = actors.begin();
    while(it!=actors.end()){
        if(!(*it)->isAlive()){
            Actor* dead=*it;
            it=actors.erase(it);
            delete dead;
        }
        else
        it++;
    }
    
}

void StudentWorld::addSpray(){
    int r=(128-2*SPRITE_RADIUS);//slope should be in front of the player
    double sprayx=128+r*cos(player->getPosAngle() / 180.0 * PI);
    double sprayy=128+r*sin(player->getPosAngle() / 180.0 * PI);
    actors.push_back(new Spray(this,sprayx,sprayy,player->getDirection()));
    
    
}

void StudentWorld::addFlames(){
    int posang = player->getDirection();
    int r=2*SPRITE_RADIUS;
    double centerx=player->getX();
    double centery=player->getY();
    for(int i=0;i<16;i++){
        
        double x=centerx+r*cos((posang+i*22) / 180.0 * PI);
        double y=centery+r*sin((posang+i*22) / 180.0 * PI);
        actors.push_back(new Flame(this,x,y,posang+i*22));//increment angle
        
    }
    
    
}

double StudentWorld:: calculate(double x1, double y1, double x2, double y2){
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));//calculate eulidean distance
}

bool  StudentWorld:: bacCanMove(double x,double y){
    if(calculate(x,y,VIEW_WIDTH/2,VIEW_HEIGHT/2)>=128)//determine whether the coordinate is out of dish
        return false;
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it!=actors.end()){
            Actor* current=*it;
        if(current->isblock()&&current->isAlive()&&calculate(x,y,current->getX(),current->getY())<=SPRITE_RADIUS){//determine whether overlaps with dirt
            return false;
        }
        it++;
    }
    return true;
}

bool StudentWorld:: findClosestFood(double mx,double my,double &x,double &y){
    list<Actor*>::iterator it;
    it = actors.begin();
    bool found=false;
    double mind=128;
    while(it!=actors.end()){
            Actor* current=*it;
        double d=calculate(mx,my,current->getX(),current->getY()); if(current->iseatable()&&current->isAlive()&&d<=128){
            found=true;
            if(d<=mind){
                mind=d;
                x=current->getX();//update coordinate to the closest food
                y=current->getY();
            }
        }
        it++;
    }
    return found;
}


bool StudentWorld:: isWeaponOverlap(double x1, double y1, int damval){
      list<Actor*>::iterator it;
    it = actors.begin();
    while(it!=actors.end()){
            Actor* current=*it;
        if(current->isdamageable()&&current->isAlive()){
        
            if(calculate(x1,y1,current->getX(),current->getY())<=8){
                if(current->hashitpt()){
                    if(current->setHealth(-damval)){//hurt but alive
                        playSound(current->getSound());
                    }
                    else{
                        playSound(current->getSound()-1);//dead after hurt
                        increaseScore(100);
                        int rand=randInt(0,1);
                        if(rand==0)//50% chance
                            actors.push_back(new Food(this,current->getX(),current->getY()));
                    }
                }
                else
                    current->setDead();
        
                return true;
                
            }
        }
        it++;
    }
    return false;
}

bool StudentWorld:: isFoodOverlap(double x,double y){
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it!=actors.end()){
            Actor* current=*it;
        if(current->iseatable()&&current->isAlive()&&calculate(x,y,current->getX(),current->getY())<=8){
            current->setDead();//set food to dead
            return true;
        }
        it++;
    }
    return false;
}
bool StudentWorld:: isSoverlap(double x1, double y1){
    if(calculate(x1,y1,player->getX(),player->getY())<=8){
        return true;
    }
        
    return false;
}

void StudentWorld::damageHealth(int damage){
    if(player->setHealth(-damage))//decrement player hit point
        playSound(SOUND_PLAYER_HURT);
    else
        playSound(SOUND_PLAYER_DIE);
}
void StudentWorld::chargeFlame(){
    player->chargeFlameThrower();
}
void StudentWorld::restoreHealth(){
    int charge=100-player->getHealth();
    player->setHealth(charge);
}


void StudentWorld::addBacteria(int type,double x, double y){
    if(type==1){//add regsam
        actors.push_back(new RegSam(this,x,y));
    }
    else if(type==2){//add agsam
        actors.push_back(new AgSam(this,x,y));
    }
    else//add ecoli
        actors.push_back(new Ecoli(this,x,y));
    n_bacteria++;
}

void StudentWorld::getPlayerCord(double &x,double &y){
    x=player->getX();
    y=player->getY();
}
void StudentWorld::cleanUp()
{
     list<Actor*>::iterator it;
    it = actors.begin();
    while(it!=actors.end()){//delete actor list
            Actor* dead=*it;
            it=actors.erase(it);
            delete dead;
    }
    delete player;
    player = nullptr;
     
}

