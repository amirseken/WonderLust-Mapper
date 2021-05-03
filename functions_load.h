/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   load_helpers.h
 * Author: sekenami
 *
 * Created on March 2, 2021, 7:10 AM
 */

#pragma once
#include<string>

/*All the structures loaded can be found as global variables under the file "global_variables.h*/

// Load helper functions for m1 
void load_intersection_street_segments(); 

void loadStreetNamesAndStreetIDs(); 

void initializeSegmentsMap(); 

void destroySegmentsMap();
//Load Number of intersections, streets, street segments, points of interest, Features, OSM Nodes, OSM Ways and OSM relations

//Load street structures
void load_street();
//Load intersection structures
void load_intersection();
//Load street segment structures
void load_street_segments();
//Load natural features structures
void load_feature();
//Load points of interest structures
void load_POI();
//Load OSMNodes multimap
void load_OSM_Nodes();
//Load OSM Ways, specifically secondary roads
void load_highways_And_MainStreets();
//Load subway stations and bus stops
void load_transit();
//Load hospitals using OSMIds
void load_hospitals(); 
//Load restaurants using OSMIds
void load_restaurants(); 

//Load node information 
void load_node(); 

//Load the maximum speed limit of the city 
void load_speed_limit(); 

// Used for Extra Feature: parse the map filename to get the map name 
std::string parse_load_map(std::string map_name); 

