/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   data_structures.h
 * Author: sekenami
 *
 * Created on March 2, 2021, 7:03 AM
 */



#pragma once


#include "OSMDatabaseAPI.h"
#include <string>
#include <vector>
#include <limits>

#define NO_EDGE -1 

//structure holding OSM node data
struct Node{
    OSMID id;
    LatLon position;
    int tag;
    std::string roadName;
    // initializing the node with values
    Node (OSMID ID , LatLon Position, int Tag, std::string RoadName ) { 
        id = ID ;
        position = Position;
        tag = Tag; 
        roadName = RoadName;
    }
};


//structure holding intersection data
struct Intersection{
    LatLon position;
    std::string name;
    bool highlight = false;
    std::vector<StreetSegmentIdx> Street_Segments;
    std::vector<IntersectionIdx> adjacentNode; 
    std::vector<StreetSegmentIdx> linkingEdge; 
    bool visited = false; 
   
};

//structure holding street segment data
struct StreetSegment{ 
    int from, to;
    bool oneWay; 
    int curvePointCount; 
    int streetID; 
    float speedLimit;
    std::vector<LatLon> curvePointPosition;
    double travelTime; 
};

// structure of street data
struct Street{ 
    std::string Name;
    std::vector<int> Intersections;
    std::vector<int> Street_Segments;
};

//structure holding feature data
struct Feature{
  FeatureType Feature_type;
  std::string Feature_name;
  int Point_Count;
  std::vector<LatLon> Point_Position;
};

//structure holding POI data
struct POI{
    std::string type;
    std::string name;
    LatLon position;
};

struct Node2 {
    IntersectionIdx id;
    StreetSegmentIdx reachingEdge;
    std::vector<IntersectionIdx> adjacentNode;
    std::vector<StreetSegmentIdx> linkingEdge;
    double bestTime; 
    //bool visited = false; 
    
    Node2(IntersectionIdx id1, std::vector <IntersectionIdx> nodes, std::vector <StreetSegmentIdx> edges){
        id = id1; 
        reachingEdge = NO_EDGE; 
        adjacentNode = nodes; 
        linkingEdge = edges; 
        bestTime = std::numeric_limits <double>::infinity();          
    }
    
    Node2(){}
    
    Node2(IntersectionIdx id1, StreetSegmentIdx prevEdge, std::vector <IntersectionIdx> nodes, std::vector <StreetSegmentIdx> edges){
    id = id1; 
    reachingEdge = prevEdge; 
    adjacentNode = nodes; 
    linkingEdge = edges; 
    bestTime = std::numeric_limits <double>::infinity(); 

    }
};


struct WaveElem {
    Node2 *node;
    int edgeID; // ID of edge used to reach this node
    double path_length; 
    double estimated_time; 
   // bool visited; 
    
    WaveElem(Node2 *n, int id, double path_len, double prediction) {
        node = n;
        edgeID = id;
        path_length = path_len;
        estimated_time = prediction; 
        //visited = is_visited; 
    }
};


struct Dropoff{
    IntersectionIdx dropoff; 
    bool isPickedUp; 

}; 
