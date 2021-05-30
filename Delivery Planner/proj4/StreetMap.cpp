
#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord,vector<StreetSegment>>* map;
};

StreetMapImpl::StreetMapImpl()
{
    map=new ExpandableHashMap<GeoCoord,vector<StreetSegment>>;
}

StreetMapImpl::~StreetMapImpl()
{
    delete map;
}

bool StreetMapImpl::load(string mapFile)
{
   ifstream input(mapFile);
   if(!input){
       return false;
   }
   string line;
   while (getline(input, line)){
       string name=line;
       getline(input, line);
       istringstream iss(line);
       int count;
       if(iss>>count){
           for(int i=0;i<count;i++){
               getline(input, line);
                istringstream cord(line);
               string startlat,startlong,endlat,endlong;
               if(cord >> startlat >> startlong >> endlat >> endlong){
                   GeoCoord start=GeoCoord(startlat,startlong);
                   GeoCoord end(endlat,endlong);
                   StreetSegment s(start,end,name);
                   StreetSegment revs(end,start,name);
                   vector <StreetSegment> streets;
                   auto vec=map->find(start);//push back StreetSeg
                   if(vec==nullptr){
                       vector <StreetSegment> streets;
                       streets.push_back(s);
                       map->associate(start,streets);
                   }
                   else{
                       vec->push_back(s);
                       map->associate(start,*vec);
                   }
                    auto revvec=map->find(end);//push back reverse street seg
                   if(revvec==nullptr){
                       vector <StreetSegment> revstreets;
                        revstreets.push_back(revs);
                        map->associate(end,revstreets);
                    }
                    else{
                        revvec->push_back(revs);
                        map->associate(end,*revvec);
                        }
               }
                   
           }
       }
       
   }
       
   return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    auto v=map->find(gc);
    if(v==nullptr)
    return false;
    
    segs.clear();
    segs=*v;
    return true;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}



