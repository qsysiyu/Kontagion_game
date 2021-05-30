//
//  ExpandableHashMap.hpp
//  proj4
//
//  Created by qsy on 3/4/20.
//  Copyright © 2020 qsy. All rights reserved.
//



#ifndef ExpandableHashMap_h
#define ExpandableHashMap_h
#include<list>
#include <algorithm>
// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    double maxload;
    int bucketnum;
    int n_association;
    struct Pair{
        KeyType key;
        ValueType val;
    };
    
    std::list<Pair>* pairArr;
    unsigned int getBucketNum(const KeyType& key) const{
        unsigned int hasher(const KeyType& key);
        unsigned int h = hasher(key);
        return h%bucketnum;
    }
    
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    maxload=maximumLoadFactor;
    bucketnum=8;
    n_association=0;
    pairArr=new std::list<Pair>[bucketnum];
    
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    delete [] pairArr;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    bucketnum=8;
    n_association=0;
    delete [] pairArr;
    pairArr=new std::list<Pair>[bucketnum];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return n_association;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    unsigned int bucketNo=getBucketNum(key);
    typename std::list<Pair>::iterator it;
    it=pairArr[bucketNo].begin();
    while(it!=pairArr[bucketNo].end()){//check whether there is a duplicate key
        if(key==it->key){
            it->val=value;
            return;
        }
        it++;
    }
    n_association++;
    if((n_association)/bucketnum>maxload){//rehash if exceeds limit
        bucketnum=bucketnum*2;
        std::list<Pair>* newarr=new std::list<Pair>[bucketnum];
        for(int i=0;i<bucketnum/2;i++){//copy items from old map to new
            typename std::list<Pair>::iterator it;
            unsigned int index;
            it=pairArr[i].begin();
            while(it!=pairArr[i].end()){
                index=getBucketNum(it->key);
                newarr[index].splice(newarr[index].begin(),pairArr[i],it);
                it=pairArr[i].begin();
            }
        }
        delete [] pairArr;
        pairArr=newarr;
        newarr=nullptr;
        bucketNo=getBucketNum(key);
    }
    
    //insert
    Pair p;
    p.key=key;
    p.val=value;
    pairArr[bucketNo].push_back(p);
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    int position = getBucketNum(key);//find which bucket it is in
    typename std::list<Pair>::iterator it;
    it=pairArr[position].begin();
    while(it!=pairArr[position].end()){
        if(it->key==key)
            return &(it->val);
        it++;
    }
    return nullptr;  
   
}



#endif /* ExpandableHashMap_hpp */

