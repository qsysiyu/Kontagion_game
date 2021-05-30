#include "provided.h"
#include <vector>
#include <cmath>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* m_map;
    double calculateDis(GeoCoord depot, const vector<DeliveryRequest> deliveries) const;
    void swap(vector<DeliveryRequest>& deliveries) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_map=sm;
}


DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}
double DeliveryOptimizerImpl:: calculateDis(GeoCoord depot,const vector<DeliveryRequest> deliveries) const{
    double distance=0;
    GeoCoord prevcord=deliveries[0].location;
       distance=distanceEarthMiles(depot,prevcord);//distance between depot and the first delivery spot
       for(int i=1;i<deliveries.size();i++){
           distance += distanceEarthMiles(prevcord,deliveries[i].location);//traverse the vector
           prevcord=deliveries[i].location;
       }
      distance += distanceEarthMiles(prevcord,depot);//distance between last delivery spot
    return distance;
}

void DeliveryOptimizerImpl::swap(vector<DeliveryRequest>& deliveries) const{
    int size=deliveries.size();
    if(size==0)
        return;
   int index1=rand()%size;//swap two element in two random position
  int index2=rand()%size;
    while(index1==index2){
        index1=rand()%size;//make sure two random number generated are not the same
        index2=rand()%size;
    }
    DeliveryRequest temp=deliveries[index1];
    deliveries[index1]=deliveries[index2];
    deliveries[index2]=temp;
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = 0;
    newCrowDistance = 0;
    if(deliveries.size()==0)
        return;
     
    oldCrowDistance=calculateDis(depot,deliveries);
    if(deliveries.size()==1){
        newCrowDistance=oldCrowDistance;
        return;
    }
    double T=1000;
    double Tmin=0.000001;
    double alpha=0.99;
    double bestdis=oldCrowDistance;
    srand((unsigned)time(NULL));
    vector<DeliveryRequest> possbest=deliveries;
    int count=pow(deliveries.size(),2);//maximum number of time a loop runs will be square the size
    double deltadif,random;
    for(int i=count;i>=0;i--){
        swap(possbest);
        random=rand()%RAND_MAX;
        deltadif=calculateDis(depot,possbest)-bestdis;
        if(deltadif<0||exp(-deltadif/T)>random){
            deliveries=possbest;//update deliveries vector to new best order
            bestdis += deltadif;
        }
        T *= alpha;
        if(T<Tmin){
            break;
        }
    }
    
    newCrowDistance = bestdis;
    
    
   
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}

