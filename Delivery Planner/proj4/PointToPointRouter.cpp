

#include "provided.h"
#include <list>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_streetmap;
    struct Point{
        double G,H;
        Point *parent;
        const GeoCoord gc;
        Point(const GeoCoord g):gc(g),G(0),H(0),parent(nullptr){
        }
        double F(){
            return G+H;
        }
    };
    
};



PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_streetmap=sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    vector<StreetSegment> seg;
    if(m_streetmap->getSegmentsThatStartWith(start,seg)==false||m_streetmap->getSegmentsThatStartWith(end,seg)==false)
            return BAD_COORD;//start or end coordinate cannot be found
   
    if(start==end){
        totalDistanceTravelled=0;//start coordinate is the delivery coordinate; no need to travel
        route.clear();
        return DELIVERY_SUCCESS;
    }
    
    
    //initialize start point and push to openlist
    Point* startpt=new Point(start);
    startpt->H=distanceEarthMiles(start,end);
   
    list<Point*> openlist;
       list<Point*>closedlist;
    openlist.push_back(startpt);
    bool found=false;
    Point* cur;
    
    while(openlist.size()>0){
        cur=openlist.front();
        bool changed=false;
        auto it = openlist.begin();
        for (;it!=openlist.end();it++){
            if (cur->F() > (*it)->F()){//find the point in the openlist with least F value
                cur = *it;
                changed=true;
                break;
            }
        }
        if(cur->gc==end){
            found=true;
            break;//found
        }
        if(changed){//point with least F is not cur
            openlist.erase(it);
        }
        else
            openlist.pop_front();
        
        vector<StreetSegment> childsegs;
        m_streetmap->getSegmentsThatStartWith(cur->gc,childsegs);
        for(auto it=childsegs.begin();it!=childsegs.end();it++){
            string name=it->name;
            double cost = cur->G+distanceEarthMiles(it->start,it->end);
            Point* openoverlap=nullptr;
            for(auto openit = openlist.begin();openit!=openlist.end();openit++){//search thru openlist
                if(it->end==(*openit)->gc){
                    openoverlap=*openit;
                    break;
                }
            }
            
            if(openoverlap!=nullptr){//case 1; found in openlist
                if(openoverlap->G<=cost)//the orignial route is better
                    continue;
                //new route is better
                openoverlap->G=cost;
                openoverlap->parent=cur;
            }
            else{
                Point* closeoverlap=nullptr;
                auto closeit = closedlist.begin();
                for(;closeit!=closedlist.end();closeit++){//search thru closedlist
                    if(it->end==(*closeit)->gc){
                        closeoverlap=*closeit;
                        break;
                    }
                }
                if(closeoverlap!=nullptr){//case 2;found in closed list
                    if(closeoverlap->G<=cost)
                        continue;
                    openlist.push_back(closeoverlap);
                    closedlist.erase(closeit);
                }
                else{//case 3; not found in both lists
                    openlist.push_back(new Point(it->end));
                    double h=distanceEarthMiles(it->end,end);
                    openlist.back()->H=h;
                }
                openlist.back()->G=cost;
                openlist.back()->parent=cur;
        }
          
        }//end for
        
        closedlist.push_back(cur);
        
    }//end while
    
        if(found){
            totalDistanceTravelled=cur->G;
            route.clear();
            
            while(cur->parent!=nullptr){//update route vector
                vector<StreetSegment> stseg;
                if(m_streetmap->getSegmentsThatStartWith(cur->parent->gc,stseg)){
                    for(int i=0;i<stseg.size();i++){
                        if(stseg[i].start==cur->parent->gc&&stseg[i].end==cur->gc){
                            route.push_front(stseg[i]);
                            break;
                        }
                            
                    }
                }
                cur=cur->parent;
            }
            //clear memory used
            for(auto itt = openlist.begin();itt!=openlist.end();itt++){
                       delete *itt;
                   }
            for(auto itt = closedlist.begin();itt!=closedlist.end();itt++){
                      
                       delete *itt;
                   }
            return DELIVERY_SUCCESS;
        }
      
    //clear memory used
    for(auto itt = openlist.begin();itt!=openlist.end();itt++){
                          delete *itt;
                      }
    for(auto itt = closedlist.begin();itt!=closedlist.end();itt++){
             
              delete *itt;
          }
    return NO_ROUTE;  
}




//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}


