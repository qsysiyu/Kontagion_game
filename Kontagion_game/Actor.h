#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;
const double PI = 4 * atan(1);
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor:public GraphObject{
public:
    Actor(StudentWorld* world,int imageID, double startX, double startY, Direction dir,int depth);
    virtual ~Actor(){}
    virtual void doSomething()=0;
    bool isAlive() const;
    virtual void setDead();
    virtual bool setHealth(int newhealth){
        return false;
    }
    virtual bool isdamageable() const;
    virtual bool isblock() const{
        return false;
    }
    virtual bool isoverlap()const{
        return false;
    }
    virtual bool iseatable()const{
        return false;
    }
    virtual bool hashitpt() const{
        return false;
    }
    StudentWorld* getworld() const{
        return m_world;
    }
    virtual int getSound(){
        return SOUND_NONE;
    }
    
private:
    bool isalive;
    StudentWorld* m_world;
    };



class mainActor:public Actor{
public:
    mainActor(StudentWorld* world,int imageID, double startX, double startY, Direction dir, int health);
    virtual ~mainActor(){}
    virtual bool setHealth(int newhealth);
    virtual bool hashitpt() const{
        return true;
    }
    int getHealth() const;
    
private:
    int m_health;

    
};

class Bacteria: public mainActor{
public:
    Bacteria(StudentWorld* world,int dam,int imageID, double startX, double startY, int health);
    virtual ~Bacteria(){}
    virtual int getSound(){
        return SOUND_SALMONELLA_HURT;
    }
    void computeBabyCord(double &newx,double &newy);
    virtual bool isdamageable() const{
        return true;
    }
    void damageOrEat(StudentWorld *sw);
    virtual void giveBirth(StudentWorld *sw,double newx, double newy)=0;
    virtual void doSamSpec(StudentWorld *sw);
    virtual void setDead();
private:
    int foodcount,movement,healthdam;
    
};

class RegSam: public Bacteria{
public:
    RegSam(StudentWorld* world,double startX, double startY);
    virtual void doSomething();
    virtual void giveBirth(StudentWorld *sw,double newx, double newy);
    
    
};

class AgSam: public Bacteria{
public:
    AgSam(StudentWorld* world,double startX, double startY);
     virtual void doSomething();
    virtual void giveBirth(StudentWorld *sw,double newx, double newy);
};

class Ecoli: public Bacteria{
public:
    Ecoli(StudentWorld* world,double startX, double startY);
     virtual void doSomething();
    virtual int getSound(){
        return SOUND_ECOLI_HURT;
    }
    virtual void doSamSpec(StudentWorld *sw){
        
    }
    virtual void giveBirth(StudentWorld *sw,double newx, double newy);
};

class Socrates:public mainActor{
public:
    Socrates(StudentWorld* world);
    virtual ~Socrates(){}
    virtual void doSomething();
    int getFlamesThrowers() const;
    int getSprayChargers()const;
    int getPosAngle() const;
    void chargeFlameThrower();
private:
    int n_flameThrowers,n_sprayChargers;
    int posangle;
    void useFlameThrower();
    void useSprays();
    void moveS();
    
    
};

class StaticObject:public Actor{
public:
    StaticObject(StudentWorld* world,int imageID, double startX, double startY,Direction dir);
    virtual ~StaticObject(){}
    virtual void doSomething();
   
    
};
class dirtpile:public StaticObject{
public:
    dirtpile(StudentWorld* world,double startX,double startY);
    virtual ~dirtpile(){}
    virtual bool isdamageable() const;
    virtual bool isblock() const{
        return true;
    }
    
};

class Food: public StaticObject{
public:
    Food(StudentWorld* world,double startX,double startY);
    virtual ~Food(){}
    virtual bool iseatable()const{
           return true;
       }
    virtual bool isoverlap()const{
        return true;
    }

};

class Projectile:public Actor{
public:
    Projectile(StudentWorld* world,int dval,int dis,int imageID, double startX, double startY, Direction dir);
    virtual ~Projectile(){}
    virtual void doSomething();
private:
    int traveldis;
    int damageval;
    bool travel();
};

class Flame:public Projectile{
public:
    Flame(StudentWorld* world,double startX,double startY,Direction dir);
    virtual ~Flame(){}
    
};

class Spray:public Projectile{
public:
    Spray(StudentWorld* world,double startX,double startY,Direction dir);
    virtual ~Spray(){}
};


class LTObject: public Actor{
public:
    LTObject(int level,int pt,StudentWorld* world,int imageID, double startX, double startY);
    virtual ~LTObject(){}
    virtual bool isdamageable() const{
        return true;
    }
    virtual void doSomething();
private:
    int lifetime;
    int bonus;
    virtual void dospecific(StudentWorld *sw)=0;
    virtual int getpoints() const{
        return bonus;
    }
    virtual void decLifetime(){
        lifetime--;
        if(lifetime<=0)
            setDead();
            
    }
};

class Fungus: public LTObject{
public:
    Fungus(int level,StudentWorld* world, double startX, double startY);
    virtual ~Fungus(){}
    
private:
    virtual void dospecific(StudentWorld *sw);
    
};

class RHealthGoodie: public LTObject{
public:
    RHealthGoodie(int level,StudentWorld* world, double startX, double startY);
    virtual ~RHealthGoodie(){}
    
private:
    virtual void dospecific(StudentWorld *sw);
};


class FThrowerGoodie: public LTObject{
public:
    FThrowerGoodie(int level,StudentWorld* world, double startX, double startY);
    virtual ~FThrowerGoodie(){}
    
private:
    virtual void dospecific(StudentWorld *sw);
};

class ELifeGoodie: public LTObject{
public:
    ELifeGoodie(int level,StudentWorld* world, double startX, double startY);
    virtual ~ELifeGoodie(){}
private:
    virtual void dospecific(StudentWorld *sw);
};

class Pit:public Actor{
public:
    Pit(StudentWorld* world,double startX, double startY);
    virtual ~Pit(){}
    virtual void doSomething();
    virtual void setDead();
    virtual bool isoverlap()const{
        return true;
    }
private:
    int regsam, agsam, Ecoli;
    bool hasBac(int type){
        if(type==1){
            return regsam>0;}
        else if(type==2){
            return agsam>0;}
        return Ecoli>0;
    }
    int getTotal() const{
        return regsam+agsam+Ecoli;
    }
    
};



#endif // ACTOR_H_
