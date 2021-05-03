#include <iostream>
#include "m1.h"
#include "StreetsDatabaseAPI.h"

#include <limits.h>
#include <cmath>

#include <vector>
#include <utility>
#include <algorithm> 

// this function returns the time it takes to travel along a street segment 
double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id){
   
    StreetSegmentInfo ss_info = getStreetSegmentInfo(street_segment_id);
  
    double time = findStreetSegmentLength(street_segment_id)/ss_info.speedLimit;
    
    return time;
}

// this function returns the nearest intersection to the given position. 

int findClosestIntersection(LatLon my_position){

    double min_distance = INT_MAX;
    int near_intersection_id = 0;
    
    for(int intersection = 0; intersection < getNumIntersections(); ++intersection){
        
        std::pair<LatLon, LatLon> pair_of_points = std::make_pair(my_position, getIntersectionPosition(intersection));
        
        double dist = findDistanceBetweenTwoPoints(pair_of_points);
        
        if(min_distance > dist){
            min_distance = dist;
            near_intersection_id = intersection;  
        }
    }
    return near_intersection_id;
}



// this function returns all intersections reachable by traveling down one street segment from a given intersection
std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id){
    
    std::vector<IntersectionIdx> adjacent_intersections;
    
    // looping through all of the street segments for the given intersections 
    for(int ss = 0; ss < getNumIntersectionStreetSegment(intersection_id) ; ++ss){
        
        int ssOfIntersection = getIntersectionStreetSegment(intersection_id, ss); 
        StreetSegmentInfo ss_info = getStreetSegmentInfo(ssOfIntersection);
        if(ss_info.from == ss_info.to){
            adjacent_intersections.push_back(ss_info.to);
        }
        else{
            if(ss_info.oneWay == false){
                if(ss_info.from == intersection_id){
                   adjacent_intersections.push_back(ss_info.to);
                   
                }
                else{
                    adjacent_intersections.push_back(ss_info.from);
                   
                }
             
            }
        
            else if(ss_info.oneWay == true && ss_info.from == intersection_id){
                adjacent_intersections.push_back(ss_info.to);
            }
        }
        
        
    }
    //sorting the vector and removing duplicates 
    std::sort(adjacent_intersections.begin(), adjacent_intersections.end());
    auto last = std::unique(adjacent_intersections.begin(), adjacent_intersections.end());
    adjacent_intersections.erase(last, adjacent_intersections.end());
    
    return adjacent_intersections;
}


// this function returns the area of the given closed feature in square meters. It returns 0 if the feature is not a closed polygon.
double findFeatureArea(FeatureIdx feature_id){

    double Area = 0;
    if(!(getFeaturePoint(feature_id, 0) == getFeaturePoint(feature_id,getNumFeaturePoints(feature_id) - 1))){
        return 0;
    }
    else{
        double lat_avg = 0;
        
        for(int i = 1; i< getNumFeaturePoints(feature_id) ; ++i){
            
           // calculating average latitude based on latitude of given feature_id and the feature_id before it 
            lat_avg = 0.5*(getFeaturePoint(feature_id, i).latitude() + getFeaturePoint(feature_id, i-1).latitude())*kDegreeToRadian;
            
            double y2 = getFeaturePoint(feature_id, i).latitude() ; 
            double y1 = getFeaturePoint(feature_id, i-1).latitude(); 
            double x2 = getFeaturePoint(feature_id, i).longitude()*cos(lat_avg) ; 
            double x1 = getFeaturePoint(feature_id, i-1).longitude()*cos(lat_avg) ; 
            Area += ((y2 - y1)*(x2 + x1)/2)*(kEarthRadiusInMeters*kEarthRadiusInMeters)*(kDegreeToRadian*kDegreeToRadian);
        }
        
        if(Area< 0) // if the area if negative, then we return it's absolute value 
            return Area*(-1);
        else{
            return Area;
        }
    }
}
