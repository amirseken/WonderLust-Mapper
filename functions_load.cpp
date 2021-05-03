/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "functions_load.h"
#include "vars.h"
#include "StreetsDatabaseAPI.h"
#include "structs.h"
#include <string>
#include<algorithm>
//some definitions of global variables
std::multimap<OSMID, Node> NodesID;
std::vector<StreetSegment> Street_Segment_Info;
std::vector<Intersection> Intersection_Info;
std::vector<Street> Street_Info;
std::vector<Feature> Feature_Info;
std::vector<POI> POI_Info;

std::vector<Node> Main_Street_Nodes;
std::vector<Node> Highway_Nodes;
std::vector<Node2> Node_Info; 

std::unordered_multimap<std::string, int> StreetandNames;
std::vector<const OSMNode*> osm_restaurants;
std::vector<const OSMNode*> osm_hospitals;
std::vector<const OSMNode*> Subway_Stations;
float max_speed_limit = 0; 

void load_street() {
    for (int i = 0; i < getNumStreets(); i++) {
        std::string temp = getStreetName(i);
        StreetandNames.insert(std::make_pair(temp, i)); //store street name and id in (unordered_multimap)

        Street info;
        info.Name = temp;
        Street_Info.push_back(info); // store street id and name in (2d vector)
    }
}

void load_intersection() {
    for (int i = 0; i < getNumIntersections(); i++) {
        //Intersection Info
        struct Intersection info;
        info.position = getIntersectionPosition(i);
        info.name = getIntersectionName(i);
        Intersection_Info.push_back(info);
        auto range = StreetandNames.equal_range(info.name); // return pair of street ids(second) and names(first) that matches names of intersections

        for (auto it = range.first; it != range.second; it++)
            Street_Info[it->second].Intersections.push_back(i);
    } 
}

void load_street_segments() {
    for (int i = 0; i < getNumStreetSegments(); i++) {
        //id -> Street Segment Info
        StreetSegmentInfo segment_info = getStreetSegmentInfo(i);
        StreetSegment temp;
        temp.curvePointCount = segment_info.numCurvePoints;
        temp.from = segment_info.from;
        temp.to = segment_info.to;
        temp.oneWay = segment_info.oneWay;
        temp.streetID = segment_info.streetID;
        temp.speedLimit = segment_info.speedLimit;

        //curve point positions
        for (int j = 0; j < temp.curvePointCount; j++) {
            temp.curvePointPosition.push_back(getStreetSegmentCurvePoint(i, j));
        }
        Street_Segment_Info.push_back(temp);
        //street id -> street segments
        Street_Info[segment_info.streetID].Street_Segments.push_back(i);
        //intersection id -> number of street segments       
        Intersection_Info[segment_info.from].Street_Segments.push_back(i);
        Intersection_Info[segment_info.to].Street_Segments.push_back(i);
        Intersection_Info[segment_info.from].adjacentNode.push_back(segment_info.to); 
        Intersection_Info[segment_info.from].linkingEdge.push_back(i); 

        if(!segment_info.oneWay){
            Intersection_Info[segment_info.to].adjacentNode.push_back(segment_info.from); 
            Intersection_Info[segment_info.to].linkingEdge.push_back(i); 
        }
        
        Street_Info[segment_info.streetID].Intersections.push_back(segment_info.from);
        Street_Info[segment_info.streetID].Intersections.push_back(segment_info.to);
    } // get all street segment info in one for loop
}

void load_feature() {
    for (int i = 0; i < getNumFeatures(); i++) {
        Feature temp;
        const std::string & getFeatureName(FeatureIdx featureIdx);

        temp.Feature_type = getFeatureType(i);
        temp.Feature_name = getFeatureName(i);
        temp.Point_Count = getNumFeaturePoints(i);

        for (int j = 0; j < temp.Point_Count; j++) {
            temp.Point_Position.push_back(getFeaturePoint(i, j));
        }
       
        Feature_Info.push_back(temp);
    }
}


void load_POI()
{
    for (int i = 0; i < getNumPointsOfInterest(); i++){
        POI temp;
        temp.type = getPOIType(i);
        temp.name = getPOIName(i);
        temp.position = getPOIPosition(i);  
        POI_Info.push_back(temp);       
        
        
    }
}


void load_OSM_Nodes() {

    for (int i = 0; i < getNumberOfNodes(); i++) {
        const OSMNode* OSMNodeTemp = getNodeByIndex(i);
        Node tempNode(OSMNodeTemp->id(), OSMNodeTemp->coords(), -1, "<unknown>");
        NodesID.insert(std::pair<OSMID, Node>(OSMNodeTemp->id(), tempNode));
    }
}

void load_highways_And_MainStreets() {

    std::vector<std::vector < OSMID>> RoadNodesTemp;
    std::vector<std::vector < OSMID>> HighwayNodesTemp;
    std::vector<std::string> RoadNames;

    for (int i = 0; i < getNumberOfWays(); i++) {
        //Get Way Tags
        const OSMWay* tempOSMWay = getWayByIndex(i);
        int tags_number = getTagCount(tempOSMWay);
        std::string tempRoadName;

        //This loop is done because OSM entities can have more that one tag
        for (int j = 0; j < tags_number; j++) {
            //Get OSM way tag
            std::pair<std::string,std::string> tempTag = getTagPair(tempOSMWay, j);

            //If the way has a name, store it
            if (tempTag.first == "name")
                tempRoadName = tempTag.second;
            //If the OSMWay is a secondary Road, store its nodes in RoasNodesTemp
            if ((tempTag.first == "highway") && (tempTag.second == "secondary" || tempTag.second == "residential"  || tempTag.second == "tertiary") ) {
                std::vector<OSMID> temp_node_ids = tempOSMWay->ndrefs();
                RoadNodesTemp.push_back(temp_node_ids);
                RoadNames.push_back(tempRoadName);
            }
            //If the OSMWay is a primary Road, store its nodes in HighwayNodesTemp
            if ((tempTag.first == "highway") && (tempTag.second == "motorway" || tempTag.second == "trunk" || tempTag.second == "primary")) {
                std::vector<OSMID> temp_node_ids = tempOSMWay->ndrefs();
                HighwayNodesTemp.push_back(temp_node_ids);
            }
        }
    }
    //Loop to store the nodes in Secondary_Road_Nodes global vector
    for (int i = 0; i < RoadNodesTemp.size(); i++)
        for (int j = 0; j < RoadNodesTemp[i].size(); j++) {
            //Get current node ID
            OSMID current_node_ID = RoadNodesTemp[i][j];
            //Copy node position
            LatLon tempPos = NodesID.find(current_node_ID)->second.position;
            //Create a tag
            int tempTag = i;
            //Create temporary Node
            Node nodeTemp(current_node_ID, tempPos, tempTag, RoadNames[i]);
            //Include the node in the global vector structure
            Main_Street_Nodes.push_back(nodeTemp);
        }
    //Loop to store the nodes in Secondary_Road_Nodes global vector
    for (int i = 0; i < HighwayNodesTemp.size(); i++)
        for (int j = 0; j < HighwayNodesTemp[i].size(); j++) {
            //Get current node ID
            OSMID current_node_ID = HighwayNodesTemp[i][j];
            //Copy node position
            LatLon tempPos = NodesID.find(current_node_ID)->second.position;
            //Create a tag
            int tempTag = i;
            //Create temporary Node
            Node nodeTemp(current_node_ID, tempPos, tempTag,  RoadNames[i]);
            //Include the node in the global vector structure
            Highway_Nodes.push_back(nodeTemp);
        }
}

void load_transit()
{
    for (int i = 0; i < getNumberOfNodes(); i++)
    {
        //Get Node tags
        const OSMNode* tempOSMNode = getNodeByIndex(i);
        int tags_number = getTagCount(tempOSMNode);
        
        //This loop is done because OSM entities can have more that one tag
        for (int j=0; j < tags_number; j++)
        {
            std::pair<std::string,std::string> tempTag = getTagPair(tempOSMNode,j);
            //If the node is a subway station
            if ((tempTag.first == "railway") && (tempTag.second == "subway_entrance"))
          
                Subway_Stations.push_back(tempOSMNode);      
                
                        
        }             
    }
}


void load_hospitals() {


    // Loop through all OSM nodes
    for (int i = 0; i < getNumberOfNodes(); i++) {
        const OSMNode* currNode = getNodeByIndex(i);

        // Check the tag of the currNode
        for (int j = 0; j < getTagCount(currNode); j++) {
            std::pair<std::string, std::string> tagPair = getTagPair(currNode, j);

            // Push nodes with the station=subway tag
            if (tagPair.first == "amenity" && tagPair.second == "hospital") {
                osm_hospitals.push_back(currNode);
                break;
            }
        }

    }

}


void load_restaurants() {


     // Loop through all OSM nodes
     for (int i = 0; i < getNumberOfNodes(); i++) {
        const OSMNode* currNode = getNodeByIndex(i);

       // Check the tag of the currNode
        for (int j = 0; j < getTagCount(currNode); j++) {
            std::pair<std::string, std::string> tagPair = getTagPair(currNode, j);

            // Push nodes with the station=subway tag
            if (tagPair.first == "amenity" && tagPair.second == "restaurant") {
                osm_restaurants.push_back(currNode);
                break;
            }
        }
     }
}


std::string parse_load_map(std::string map){
    std:: string path = "/cad2/ece297s/public/maps/"; 
    std::string suffix = ".streets.bin"; 
    std::string arabic = "cairo"; 
    std::string russian = "moscow";
    std::string chinese = "beijing"; 
    std::string name; 
    
    if(map.find(path) != std::string::npos){
        map.erase(map.find(path), path.length()); 
    }
    if(map.find(suffix) != std::string::npos){
        map.erase(map.find(suffix), suffix.length()); 
    }
    
    if(map.find(arabic) != std::string::npos){
        name = "cairo"; 
    }
    else if(map.find(russian) != std::string::npos){
        name = "moscow"; 
    }
    else if(map.find(chinese) != std::string::npos){
        name = "beijing"; 
    }
    else {
        name = "unknown"; 
    }
    
    return name;
}

void load_node() {

    std::vector <StreetSegmentIdx> linkingEdge; 
    std::vector <IntersectionIdx> adjacentNode; 
    
    for (int i = 0; i < getNumIntersections(); i++){
        linkingEdge = Intersection_Info[i].linkingEdge; 
        adjacentNode = Intersection_Info[i].adjacentNode; 

        Node_Info.push_back(Node2(i, adjacentNode, linkingEdge)); 
    }

}


void load_speed_limit(){ 
   
    for(int i = 0; i< getNumStreetSegments(); i++){
        if(getStreetSegmentInfo(i).speedLimit > max_speed_limit){
            max_speed_limit = getStreetSegmentInfo(i).speedLimit; 
            
        }  
    }
}
/*
void load_paths(const std::vector<DeliveryInf> deliveries){
    std::vector <StreetSegmentIdx> street_segments; 
    std::vector <StreetSegmentIdx> street_seg1;
    std::vector <StreetSegmentIdx> street_seg2;
    
    for(int i = 0; i < deliveries.size(); i++){
      std::vector <StreetSegmentIdx> street_seg1 = findStreetSegmentsOfIntersection(i);
      std::vector <StreetSegmentIdx> street_seg2 = findStreetSegmentsOfIntersection(i + 1); 
      for(int j = 0; j < street_seg1.size(); j++){
          for(int k = 0; k < street_seg2.size(); k++){
              if(street_seg1[j] == street_seg2[k]){
                  street_segments.push_back(street_seg1[j]); 
              }
          }
      }
    
    }


}
 */  
