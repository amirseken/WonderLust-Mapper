/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m4.h"
#include "m3.h"
#include "m2.h"
#include "m1.h"
#include <map>
#include <limits.h>
#include <chrono>
#include <cfloat>
#include <limits>
#include "math.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <list>
#include <numeric>
#include <queue>
#define TIME_LIMIT 50



std::vector <double> two_opt(std::vector <double> deliveryOrder); 
std::vector <double> anneal(std::vector <double> distance_inter); 
std::vector<CourierSubPath> travelingCourier(const std::vector<DeliveryInf>& deliveries, const std::vector<IntersectionIdx>& depots, const float turn_penalty);


std::vector <double> two_opt(std::vector <double> deliveryOrder){
    
    for(int i = 1; i < deliveryOrder.size() ; i = i + 2){
        double c = deliveryOrder[i];
        deliveryOrder[i] = deliveryOrder[i + 2]; 
        deliveryOrder[i + 2] = c; 
    }
    
    return deliveryOrder; 

}

std::vector <double> anneal(std::vector <double> distance_inter){
    int iteration = -1;

    double temperature = 10000.0;
    double deltaDistance = 0;
    double coolingRate = 0.9999;
    double absoluteTemperature = 0.00001;
    std::vector <double> nextOrder; 
    double shortestDistance = 0; 
    double total_distance = 0; 

    total_distance = std::accumulate(distance_inter.begin(), distance_inter.end(), 0);

    while (temperature > absoluteTemperature)
    {
        nextOrder = two_opt(distance_inter);

        deltaDistance = std::accumulate(nextOrder.begin(), nextOrder.end(), 0) - total_distance;

        //if the new order has less time or if the new order has a larger time but satisfies Boltzman condition then accept the arrangement
        double prob = rand()/double(RAND_MAX); 
       
        if ((deltaDistance < 0) || (total_distance > 0 && exp(-deltaDistance / temperature) > prob))
        {
            for (int i = 0; i < nextOrder.size(); i++)
                distance_inter[i] = nextOrder[i];

            total_distance  = deltaDistance+ total_distance ;
        }

        //cool down the temperature
        temperature *= coolingRate;

        iteration++;
    }

    shortestDistance = total_distance ;
    return distance_inter; 
}



void appendVector(std::vector<double> & v, const std::vector<double> & w)
{
    for(int i=0; i < w.size(); ++i)
    {
        v.push_back(w[i]);
    }
}

std::vector<double> finding_legal_paths(const std::vector<DeliveryInf>& deliveries, const std::vector<int>& depots, const float turn_penalty){
    std::vector <IntersectionIdx> interesting_intersections; 
    IntersectionIdx depot_start; 
    
    for (unsigned i= 0; i < deliveries.size(); i++){
        interesting_intersections.push_back (deliveries[i].pickUp);
        interesting_intersections.push_back (deliveries[i].dropOff);
    }
    
    int number_intersections = interesting_intersections.size(); 
    
    bool isPickedUp [number_intersections] = {false};
    for (int i = 0; i < number_intersections; i = i + 2){
        isPickedUp [i] = true;
    }
    
    std::vector <double> distance_intersections; 
    for(int i = 0; i < number_intersections - 1; i++){
        distance_intersections.push_back(findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> (getIntersectionPosition(i), getIntersectionPosition(i + 1))));   
    }
    
    std::vector <double> anneal_result = anneal(distance_intersections); 
    
    
    std::vector <double> result; 
    double min_distance = std::numeric_limits <double>::infinity();
    for(int i = 0; i < depots.size(); i++){

        
        double dist = findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> (getIntersectionPosition(depots[i]), getIntersectionPosition(interesting_intersections[0])));
        if(dist < min_distance){
            depot_start = depots[i];
            min_distance = dist;
        }
    }
    result.push_back(min_distance); 
    appendVector(result, anneal_result); 
    
    //StreetSegmentInfo ss6 = getStreetSegmentInfo(intersectiong_intersections.size()-1);
    // insert closest depot at the end
    IntersectionIdx depot_finish = 0;

    double minimum_distance = DBL_MAX;
    
    for(int i = 0; i< depots.size(); i++){
        double distance = findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> (getIntersectionPosition(depots[i]), getIntersectionPosition(interesting_intersections[interesting_intersections.size() - 1])));
        if(distance < minimum_distance){
            depot_finish = depots[i];
            minimum_distance = distance;
        }
    }
    result.push_back(minimum_distance); 
    return result; 
    /*
    std::vector<IntersectionIdx> final_path; 
    final_path.push_back(depot_start); 
    appendVector(final_path, path); 
    final_path.push_back(depot_finish); 
    std::vector <StreetSegmentIdx> legal_path; 
    for(int l = 0; l < final_path.size() - 1; l++){
         legal_path= findPathBetweenIntersections(final_path[l], final_path[l+1], turn_penalty); 
    }
    
    return legal_path; 

     */ 
        
}
    
  

 



std::vector<CourierSubPath> travelingCourier(
		        const std::vector<DeliveryInf>& deliveries,

	       	        const std::vector<int>& depots, 
		        const float turn_penalty){

    //auto startTime = std::chrono::high_resolution_clock::now();
   // bool timeOut = false;
   // while(!timeOut){
   //std::vector<StreetSegmentIdx> shortest_delivery_path = finding_legal_paths(deliveries, depots, turn_penalty); 
   finding_legal_paths(deliveries,depots, turn_penalty); 
    
        // choose the shortest(travel time wise) path from global variable
    

      /*
    std::vector<StreetSegmentIdx> shortest_delivery_path;
    shortest_delivery_path.resize(paths.find(0)->second.size()); 

    double minimum_time = DBL_MAX;
    std::vector<StreetSegmentIdx> potential_short_path;
    potential_short_path.resize(paths.find(0)->second.size()); 

    for(int i = 0; i < paths.size(); ++i){

        double time = computePathTravelTime(paths.find(i)->second, turn_penalty);

        if(time < minimum_time){
            minimum_time = time;
            potential_short_path = paths.find(i)->second;
        }

    }

    shortest_delivery_path = potential_short_path;


       // insert closest depot at the start


    int depot_start = 0;
    minimum_time = DBL_MAX;
    StreetSegmentInfo ss7 = getStreetSegmentInfo(shortest_delivery_path[0]);
    for(int i = 0; i< depots.size(); i++){
        
        std::vector<StreetSegmentIdx> path2 = findPathBetweenIntersections(depots[i],ss7.from , turn_penalty);
        
        double time = computePathTravelTime(path2, turn_penalty);
        if(time < minimum_time){
            depot_start = depots[i];
            minimum_time = time;
        }
    }
    
    std::vector<StreetSegmentIdx> final_p; 
    final_p.push_back(depot_start); 
    appendVector(final_p, shortest_delivery_path); 
    //shortest_delivery_path.insert(shortest_delivery_path.begin(), depot_start);
   
    
    StreetSegmentInfo ss6 = getStreetSegmentInfo(shortest_delivery_path.size()-1);
    // insert closest depot at the end
    int depot_finish = 0;
    minimum_time = DBL_MAX;

    for(int i = 0; i< depots.size(); i++){

        std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(depots[i], ss6.to, turn_penalty);
        double time = computePathTravelTime(path, turn_penalty);
        if(time < minimum_time){
            depot_finish = depots[i];
            minimum_time = time;
        }

    }
      
   final_p.push_back(depot_finish);
       */
       
        // optimization algorithm   
    //helper_2opt(shortest_delivery_path, turn_penalty);
       
       
     //  auto currentTime = std::chrono::high_resolution_clock::now();
     //  auto wallClock = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);

      // if(wallClock.count()>0.9 *TIME_LIMIT){
       //    timeOut = true;
       //}
    //}

    
   
   
   
    std::vector<CourierSubPath> final_path;
    
    /*
//    int size0 = shortest_delivery_path.size();

  
    std::vector<CourierSubPath> final_path;
    
    int size0 = shortest_delivery_path.size();

    int size1 = deliveries.size();
    
    StreetSegmentInfo ss = getStreetSegmentInfo(shortest_delivery_path[0]);
    StreetSegmentInfo ss2 = getStreetSegmentInfo(shortest_delivery_path[1]);
    final_path.resize(size1); 
    
    //fix
    if(ss.from == ss2.to || ss.from == ss2.from){
        final_path[0].start_intersection = ss.to;
      
      
        
    }
    else if(ss.to == ss2.from || ss.to == ss2.to){
        final_path[0].start_intersection = ss.from;
  
        
    }
    
    int k = 0;
    
    for (int i = 1; i <= size0-1; i++){
        
        if(i !=size0 -1){
            bool flag = 0;
            for(int j = 1; j < size1; j++){
                StreetSegmentInfo ss4 = getStreetSegmentInfo(shortest_delivery_path[i]);

                if(!flag &&(ss4.from == deliveries[i].pickUp || ss4.from == deliveries[i].dropOff)){

                    final_path[i].start_intersection = ss4.from;
                    final_path[i-1].end_intersection = ss4.from;
                    k++;
                    flag = 1;
                }
                else if(!flag &&(ss4.to == deliveries[i].pickUp || ss4.to == deliveries[i].dropOff)){
                    final_path[i].start_intersection = ss4.to;
                    final_path[i-1].end_intersection = ss4.to;
                    k++;
                    flag = 1;
                
                }
                else if((ss4.to == deliveries[i].pickUp || ss4.to == deliveries[i].dropOff) && flag){

                    final_path[i].end_intersection = ss4.to;
                    final_path[i+1].start_intersection = ss4.to;
                    flag = 0;
                    k++;
                }
                else if((ss4.from == deliveries[i].pickUp || ss4.from == deliveries[i].dropOff) && flag){

                    final_path[i].end_intersection = ss4.from;
                    final_path[i+1].start_intersection = ss4.from;
                    flag = 0;
                    k++;
                }

            }
        }
        
    
    }
    
    StreetSegmentInfo ss9 = getStreetSegmentInfo(shortest_delivery_path[size0 -1]);
    StreetSegmentInfo ss8 = getStreetSegmentInfo(shortest_delivery_path[size0 -2]);
    //fix
    
    
    if(ss8.from == ss9.from || ss9.to == ss8.from){
        final_path[k].end_intersection = ss8.to;
    }
    else if(ss9.from == ss8.to || ss9.to == ss8.to){
        final_path[k].end_intersection = ss8.from;
    }

    int l = 0;
    
    for(int i = 0; i< k; ++i){
         std::vector<StreetSegmentIdx> subpath1;
         //streetsegments
        for(int j = l; j< size0; ++j){
            int test = 0;
            
            if(j == 0){
                StreetSegmentInfo ss11 = getStreetSegmentInfo(shortest_delivery_path[j]);
                StreetSegmentInfo ss12 = getStreetSegmentInfo(shortest_delivery_path[j+1]);
                
                            //fix
                if(ss11.to == ss12.from || ss12.to == ss11.to){
                    final_path[k].start_intersection = ss11.to;
                    test = ss11.to;
                }
                else if(ss12.from == ss11.from || ss12.from == ss11.to){
                    final_path[k].start_intersection = ss11.from;
                    test = ss11.from;
                } 
            }
            else{
                
                StreetSegmentInfo ss11 = getStreetSegmentInfo(shortest_delivery_path[j]);
                StreetSegmentInfo ss12 = getStreetSegmentInfo(shortest_delivery_path[j-1]);           
                            //fix
                if(ss12.to == ss11.from || ss11.from == ss12.from){
                    final_path[k].start_intersection = ss11.to;
                    test = ss11.to;
                    
                }
                else if(ss11.to == ss12.from || ss11.to == ss12.to){
                    final_path[k].start_intersection = ss11.from;
                    test = ss11.from;
                }
            }
            
            do{
                
                subpath1.push_back(shortest_delivery_path[j]);
                l++;
                
            }while(test != final_path[i].end_intersection);
             
            final_path[i].subpath = subpath1;
            break;
        }
    }

    */ 

    
    //annealing(final_path, turn_penalty);


    return final_path;
    
}

/*

void annealing(std::vector<CourierSubPath>& shortest_delivery_path, const float turn_penalty){


    double travel_time = computePathTravelTime(shortest_delivery_path, turn_penalty);
    
    
    int size = shortest_delivery_path.size();
    
    std::vector<StreetSegmentIdx> shortest_delivery_path_test;
    
    double minimum_traveltime = travel_time;
    for(int i = 0; i< size; i++){
        
        
        
    }
    
}
*/


