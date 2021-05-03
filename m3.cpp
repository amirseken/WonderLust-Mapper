/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m3.h"
#include "m3_helper_functions.h"
#include "structs.h"
#include "m2.h"
#include "m2_helper.h"
#include "m1.h"

#include <list> 
#include <queue> 
#include <iostream> 
#include <limits>
#include <cstdbool>

#define NO_EDGE -1 
#define TURN_PENALTY  0.015


double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, const double turn_penalty){
    double travel_time = 0;
    
    //check if path size is 0
    if(path.size() == 0){
        return travel_time;
    }
    
    travel_time += findStreetSegmentTravelTime(path[0]);
    
    for (int i = 1; i< path.size();++i){
        StreetSegmentInfo info_curr = getStreetSegmentInfo(path[i-1]);
        StreetSegmentInfo info_next = getStreetSegmentInfo(path[i]);
        

        travel_time += findStreetSegmentTravelTime(path[i]);

        if(info_curr.streetID != info_next.streetID){
            travel_time += turn_penalty;
        }

    }

    return travel_time;

}

struct astar_compare{
    bool operator() (WaveElem a, WaveElem b){
        return (a.path_length + a.estimated_time) > (b.path_length + b.estimated_time); 
    }
}; 


bool bfsPath(Node2* sourceNode, int destID, double turn_penalty) {
    
    std::priority_queue<WaveElem, std::vector<WaveElem>, astar_compare> wavefront;
    bool found = false; 
    wavefront.emplace(WaveElem(sourceNode, NO_EDGE, 0, 0));
    LatLon destination_position = Intersection_Info[destID].position; 
    std::vector <bool> visited(getNumStreetSegments(), false); 
  
    std::vector<IntersectionIdx> visited_nodes; 
    
    while (!wavefront.empty() && !found) {
        
        WaveElem wave = wavefront.top();
        wavefront.pop(); // Remove node from wavefront
        
        Node2 *currNode = wave.node;
       
        
        
        if (wave.path_length < currNode->bestTime) {
                // Was this a better path to this node? Update if so.
            
            currNode->reachingEdge = wave.edgeID;  
            currNode->bestTime = wave.path_length; 
           
            visited_nodes.push_back(currNode->id); 
            
            if (currNode->id == destID){
               found = true; 
               break; 
            }
            
            for (int i = 0 ; i < currNode->adjacentNode.size() ; i++) {
                
                Node2 *toNode = &(Node_Info[(currNode->adjacentNode[i])]); 
               
                int linkingEdge = 0; 
                if( currNode->adjacentNode.size() < currNode->linkingEdge.size() || currNode->adjacentNode.size() > currNode->linkingEdge.size()){
                    for (int j = 0 ; j < currNode->linkingEdge.size() ; j++){
                        linkingEdge = currNode->linkingEdge[j]; 
                    }
                }
                else{ 
                    linkingEdge = currNode->linkingEdge[i]; 
                }
                
                int prevEdge = currNode->reachingEdge; 
                double travel_time = 0; 
                
                
                if(prevEdge == NO_EDGE){
                    travel_time = currNode->bestTime + findStreetSegmentTravelTime(linkingEdge); 
                }
                else if (Street_Segment_Info[prevEdge].streetID != Street_Segment_Info[linkingEdge].streetID) {
                    travel_time = currNode->bestTime + findStreetSegmentTravelTime(linkingEdge) + turn_penalty; 
                }
                else{
                    travel_time = currNode->bestTime + findStreetSegmentTravelTime(linkingEdge); 
                }
            
                if(toNode != NULL && !visited[linkingEdge]){
                    LatLon current_position = Intersection_Info[toNode->id].position; 
                    double distance_between_2_nodes = findDistanceBetweenTwoPoints(std::make_pair(current_position, destination_position)); 
                    double prediction = distance_between_2_nodes / max_speed_limit; 
                    visited[linkingEdge] = true; 
                    
                    wavefront.emplace(WaveElem(toNode, linkingEdge, travel_time, prediction));
                }
             
            }
            
        }     // End if best path to this node
        
        std::sort(visited_nodes.begin(), visited_nodes.end());
        
        auto it = std::unique(visited_nodes.begin(), visited_nodes.end());
        
        bool wasUnique = (it == visited_nodes.end()); // set to true if there are no duplicates 
        
        // if we visit the same node twice, the path is illegal 
        if(!wasUnique){
            return false; 
        }
        
    }// End while wavefront not empty (more to search)
     

    return found; 
}

//backtracks from the end intersection
std::vector<StreetSegmentIdx> bfsTraceBack(int destID) {
    
    std::vector<StreetSegmentIdx> path; 
    
    Node2 *currNode = &Node_Info[destID]; 
    
    int prevEdge = currNode->reachingEdge;
    
    //while backtracking does not reach start intersection yet:
    while (prevEdge != NO_EDGE) {
        path.push_back(prevEdge);
        StreetSegment info = Street_Segment_Info[prevEdge]; 
        
        
        
        if(info.from == currNode->id){
            currNode = &Node_Info[info.to]; 
        }
        else{
            currNode = &Node_Info[info.from];
        }
        prevEdge = currNode->reachingEdge;
    }
    std::reverse(path.begin(), path.end());
     
    return (path); 
}

std::vector<StreetSegmentIdx> findPathBetweenIntersections(
		          const IntersectionIdx intersect_id_start, 
                  const IntersectionIdx intersect_id_end,
                  const double turn_penalty){

    std::vector<StreetSegmentIdx> path; 
    Node2* sourceNode = &(Node_Info[intersect_id_start]); 
    
    if(bfsPath(sourceNode, intersect_id_end, turn_penalty)){
        path = bfsTraceBack(intersect_id_end); 
    }
    else {
        std::cout << "The path can not be found" << std::endl; 
    }
    return path; 
}
