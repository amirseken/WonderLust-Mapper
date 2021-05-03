/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   global_variables.h
 * Author: sekenami
 *
 * Created on March 2, 2021, 6:58 AM
 */


#pragma once
#include "m1.h"
#include "structs.h"
#include "functions_load.h"
#include <unordered_map>
#include <map>


// global variable for average latitude.
extern double avg_lat;

// global variable for Nodes and their OSMID
extern std::multimap<OSMID, Node> NodesID;


// global variable for street ids and their names
extern std::unordered_multimap<std::string, int> StreetandNames;

//Street - Street Segment - Intersection Info Containers

extern std::vector<StreetSegment> Street_Segment_Info; 
// global variable for intersection structs
extern std::vector<Intersection>Intersection_Info; 
// global variable for street structs
extern std::vector<Street> Street_Info;
// global variable for feature structs
extern std::vector<Feature> Feature_Info;
// global variable for poi structs
extern std::vector<POI> POI_Info;

// global variable for storing hospital OSM nodes
extern std::vector<const OSMNode*> osm_hospitals;

// global variable for storing subway station nodes 

extern std::vector<const OSMNode*> Subway_Stations;
// global variable for storing restaurant OSM nodes
extern std::vector<const OSMNode*> osm_restaurants;

// global variable for storing Highway nodes
extern std::vector<Node> Highway_Nodes;             
// global variable for storing main_street nodes
extern std::vector<Node> Main_Street_Nodes;



//m3 vars

extern std::vector<Node2> Node_Info;  

extern unsigned Destination_Id;

extern float max_speed_limit; 
