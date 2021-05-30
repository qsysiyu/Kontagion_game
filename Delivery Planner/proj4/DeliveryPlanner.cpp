#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_map;
    void generateCommands(vector<DeliveryCommand>& commands, const list<StreetSegment>& route) const;
    string convertToDir(double angle) const;
    string leftOrRight(double angle) const;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_map=sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    commands.clear();
    
   
    if(deliveries.size()==0)//nothing to deliver
        return DELIVERY_SUCCESS;
    DeliveryOptimizer optimizer(m_map);
    double olddis,newdis;
    vector<DeliveryRequest> newdeliveries=deliveries;
    optimizer.optimizeDeliveryOrder(depot,newdeliveries,olddis,newdis);//get optimized delivery order
    list<StreetSegment> firstroute;
    PointToPointRouter navigator(m_map);
    double totaldis=0;
    //get route from depot to first delivery spot
    DeliveryResult result=navigator.generatePointToPointRoute(depot,newdeliveries[0].location,firstroute,totaldis);
    if(result!=DELIVERY_SUCCESS)
        return result;
    generateCommands(commands,firstroute);
    DeliveryCommand deliver;
    deliver.initAsDeliverCommand(newdeliveries[0].item);
    commands.push_back(deliver);
    
    double deliverdis=0;
    for(int i=1;i<newdeliveries.size();i++){//loop thru to plan from first delivery spot to last
        result=navigator.generatePointToPointRoute(newdeliveries[i-1].location,newdeliveries[i].location,firstroute,deliverdis);
        if(result!=DELIVERY_SUCCESS)
               return result;
            totaldis += deliverdis;
           generateCommands(commands,firstroute);
        deliver.initAsDeliverCommand(newdeliveries[i].item);
        commands.push_back(deliver);
    }
    double backdis;//plan for going from last delivery spot to depot
     result=navigator.generatePointToPointRoute(newdeliveries[newdeliveries.size()-1].location,depot,firstroute,backdis);
       if(result!=DELIVERY_SUCCESS)
           return result;
       generateCommands(commands,firstroute);
    totaldis += backdis;
    totalDistanceTravelled=totaldis;
    
    
    return DELIVERY_SUCCESS;
}
void DeliveryPlannerImpl::generateCommands(vector<DeliveryCommand>& commands, const list<StreetSegment>& route) const{
   
    string prevname;
    double firstproceeddis,angle;
    auto it = route.begin();
    StreetSegment prevline;
    
    if(route.size()>0){
        prevline=*it;
            prevname=it->name;
            firstproceeddis=distanceEarthMiles(it->start,it->end);
               angle=angleOfLine(*it);
               DeliveryCommand d;
               d.initAsProceedCommand(convertToDir(angle),prevname,firstproceeddis);//first command is always proceed
               commands.push_back(d);
        it++;
  
    }
    for(;it!=route.end();it++){
        if(it->name==prevname){//check whether it's the same street
            double proceeddis = distanceEarthMiles(it->start,it->end);
            commands.back().increaseDistance(proceeddis);
            prevline=*it;
        }
        else{//going to a different street
            double turnangle = angleBetween2Lines(prevline,*it);
            string turndir=leftOrRight(turnangle);
            if(turndir!="proceed"){
                DeliveryCommand d;
                d.initAsTurnCommand(turndir,it->name);//generate turn command
                commands.push_back(d);
                
            }
                prevname=it->name;
                prevline=*it;
                firstproceeddis=distanceEarthMiles(it->start,it->end);
                 angle=angleOfLine(*it);
                  DeliveryCommand d;
               d.initAsProceedCommand(convertToDir(angle),it->name,firstproceeddis);//generate proceed command
                commands.push_back(d);
            }
        }
        
     
    }
    


string DeliveryPlannerImpl::leftOrRight(double angle) const{
    if(angle>360){
        int ang=angle;
        angle = ang % 360;
    }
    if(angle>=1&&angle<180)
        return "left";
    else if(angle>=180&&angle<=359)
        return "right";
    return "proceed";
}

string DeliveryPlannerImpl::convertToDir(double angle) const{
    if(angle>360){
        int ang=angle;
        angle = ang % 360;
    }
    if(angle>=0&&angle<22.5)
        return "east";
    else if(angle>=22.5&&angle<67.5)
        return "northeast";
    else if(angle>=67.5&&angle<112.5)
        return "north";
    else if(angle>=112.5&&angle<157.5)
        return "northwest";
    else if(angle>=157.5&&angle<202.5)
        return "west";
    else if(angle>=202.5&&angle<247.5)
        return "southwest";
    else if(angle>=247.5&&angle<292.5)
        return "south";
    else if(angle>=292.5&&angle<337.5)
        return "southeast";
    else
        return "east";
}
//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}

