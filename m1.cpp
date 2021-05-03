/* 
 * Copyright 2021 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include "m1.h"
#include "m1_helper_functions.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "functions_load.h"

#include <string>
#include <vector>
#include <cmath>
#include <limits.h>
#include <utility>
#include <map>
#include <algorithm>
#include<unordered_set>
#include<set>

// loadMap will be called with the name of the file that stores the "layer-2"
// map data accessed through StreetsDatabaseAPI: the street and intersection 
// data that is higher-level than the raw OSM data). 
// This file name will always end in ".streets.bin" and you 
// can call loadStreetsDatabaseBIN with this filename to initialize the
// layer 2 (StreetsDatabase) API.
// If you need data from the lower level, layer 1, API that provides raw OSM
// data (nodes, ways, etc.) you will also need to initialize the layer 1 
// OSMDatabaseAPI by calling loadOSMDatabaseBIN. That function needs the 
// name of the ".osm.bin" file that matches your map -- just change 
// ".streets" to ".osm" in the map_streets_database_filename to get the proper
// name.


// global variables used for findStreetIntersections

std::vector<StreetSegmentInfo>** segmentsByStreetIDs;
std::unordered_set<StreetIdx>** streetIntersections;

// global variable used for findStreetIdsFromPartialStreetName
std::multimap <std::string, StreetIdx> streetsNamesAndIDs;

// global variable used for findStreetSegmentsOfIntersection
std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;

// data structures for findStreetLength
std::vector <double> street_lengths;
StreetSegmentInfo ssInfo;

// helper function for data initialization for findStreetSegmentsOfIntersection 
void load_intersection_street_segments() {
    intersection_street_segments.resize(getNumIntersections());
    for (int intersection = 0; intersection < getNumIntersections(); ++intersection) {

        for (int i = 0; i < getNumIntersectionStreetSegment(intersection); ++i) {

            StreetSegmentIdx ss_id = getIntersectionStreetSegment(intersection, i);

            intersection_street_segments[intersection].push_back(ss_id);
        }
    }
}

// helper function to initialize data structure for findStreetIdsFromPartialStreetName
void loadStreetNamesAndStreetIDs() {
   for (int i = 0; i < getNumStreets(); ++i) {

            std::string a = getStreetName(i);
    
            //key.erase(key.find(' '), 1);
            a.erase(std::remove_if(a.begin(), a.end(), ::isspace), a.end());

             // transforms everything to lowercase
            std::transform(a.begin(), a.end(), a.begin(), ::tolower); 
            
            std::pair <std::string, StreetIdx> pair(a, i);
            streetsNamesAndIDs.insert(pair);
        }
}

// helper function for initializing the data structures for findIntersectionsOfStreet 
void initializeSegmentsMap() {
    // vector that dynamically holds the streetSegmentInfo
    segmentsByStreetIDs = new std::vector<StreetSegmentInfo>* [getNumStreets()];
    streetIntersections = new std::unordered_set<IntersectionIdx>* [getNumIntersections()];
    for (int i = 0; i < getNumStreets(); i++) {
        segmentsByStreetIDs[i] = new std::vector<StreetSegmentInfo>();
        streetIntersections[i] = new std::unordered_set<IntersectionIdx>();
    }
}

// clears the dynamic data created in initializeSegmentsMap()
void destroySegmentsMap() {
   for (int i = 0; i < getNumStreets(); i++) {
        
       delete segmentsByStreetIDs [i];
       delete streetIntersections [i];
    }
    delete [] segmentsByStreetIDs;
    delete [] streetIntersections;
    
   
}


 
std::string map_name1;
bool loadMap(std::string map_streets_database_filename) {
    bool load_successful = false; //Indicates whether the map has loaded 
    map_name1 = parse_load_map(map_streets_database_filename); 
    
    std::string name = map_streets_database_filename; 
    name.replace(name.end() -11, name.end(), "osm.bin"); 
    
    //successfully
    std::cout << "loadMap: " << map_streets_database_filename << std::endl;
    //         
    // Load your map related data structures here.
    //
    if (loadStreetsDatabaseBIN(map_streets_database_filename)) {
        load_successful = true; // reflect whether loading the map succeeded or failed
        // calling and initializing data for findIntersectionsOfStreet 
        loadOSMDatabaseBIN(name); 
        initializeSegmentsMap();
        for (int i = 0; i < getNumStreetSegments(); i++) {
            StreetSegmentInfo segmentInfo = getStreetSegmentInfo(i);
            StreetIdx streetId = segmentInfo.streetID;
            segmentsByStreetIDs[streetId]->push_back(segmentInfo);
            streetIntersections[streetId]->insert(segmentInfo.from);
            streetIntersections[streetId]->insert(segmentInfo.to);
        }
        // For findStreetSegmentsOfIntersection, initializing data structures created previously 

        // m2 load functions
        load_POI(); 
        load_feature();
        load_intersection();
        load_intersection_street_segments();
        load_OSM_Nodes();
        load_highways_And_MainStreets();
        load_hospitals();
        load_restaurants(); 
        load_street(); 
        load_street_segments();
        load_transit();
        load_node(); 
        load_speed_limit(); 
        
        // data structure streetsNamesAndIDs for findStreetIdsFromPartialStreetName 
        loadStreetNamesAndStreetIDs();
        // data structure for findStreetLength 
        street_lengths.resize(getNumStreets(), 0);
        for (int ss_id = 0; ss_id < getNumStreetSegments(); ++ss_id) {
            ssInfo = getStreetSegmentInfo(ss_id);
            street_lengths[ssInfo.streetID] += findStreetSegmentLength(ss_id);
        }
    }
    return load_successful;
}

void closeMap() {
    //Clean-up your map related data structures here
   
    street_lengths.resize(0);
    destroySegmentsMap(); 
   
    closeStreetDatabase();
    closeOSMDatabase();
    
}

// this functions returns the distance between 2 points given their LatLon coordinates 
double findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> points) {
    double firstLatitude = (points.first).latitude();
    double secondLatitude = (points.second).latitude();
    double firstLongitude = (points.first).longitude();
    double secondLongitude = (points.second).longitude();

    double latAvg = kDegreeToRadian * ((firstLatitude + secondLatitude) / 2);

    //calculate x-coordinates using formula given: longitude * cos(latAvg);
    double firstXCoordinate = kDegreeToRadian * firstLongitude * cos(latAvg);
    double secondXCoordinate = kDegreeToRadian * secondLongitude * cos(latAvg);

    // returns distance calculated (formula given in doc.)
    return kEarthRadiusInMeters * sqrt(pow(kDegreeToRadian * (firstLatitude - secondLatitude), 2) + pow(firstXCoordinate - secondXCoordinate, 2));

}

double findStreetSegmentLength(StreetSegmentIdx street_segment_id) {
    double length = 0;
    StreetSegmentInfo thisStreetSegment = getStreetSegmentInfo(street_segment_id);
    std::vector <LatLon> locationIntersectionPoints;
    // storing theLatLon coordinates of the first (from) intersection of the given street segment
    locationIntersectionPoints.push_back(getIntersectionPosition(thisStreetSegment.from));
    // storing theLatLon coordinates of the all of the CurvePoints of the given street segment
    for (int i = 0; i < thisStreetSegment.numCurvePoints; i++) {
        locationIntersectionPoints.push_back(getStreetSegmentCurvePoint(street_segment_id, i));
    }
    // storing theLatLon coordinates of the last (to) intersection of the given street segment
    locationIntersectionPoints.push_back(getIntersectionPosition(thisStreetSegment.to));
    
    // Calculating distance between each LatLon coordinate and adding it to length 
    for (int j = 1; j < locationIntersectionPoints.size(); j++) {
        std::pair <LatLon, LatLon> intersections(locationIntersectionPoints[j - 1], locationIntersectionPoints[j]);
        length += findDistanceBetweenTwoPoints(intersections);
    }
    return length;

}

double findStreetLength(StreetIdx street_id) {
    return street_lengths[street_id];
}
// this function returns the street names at the given intersection
std::vector<std::string> findStreetNamesOfIntersection(IntersectionIdx intersection_id) {
    std::vector <std::string> streetNames;
    StreetSegmentInfo tempVar;
    
    // looping through the street segments incident on the intersection
    for (int i = 0; i < getNumIntersectionStreetSegment(intersection_id); i++) {
        tempVar = getStreetSegmentInfo(getIntersectionStreetSegment(intersection_id, i));
        streetNames.push_back(getStreetName(tempVar.streetID));
    }
    return streetNames;

}
// this function returns the street segments that connect to the given intersection 
std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id) {
    return intersection_street_segments[intersection_id]; // indexes the previously created data structure
}

// this function returns all intersection ids at which the two given streets intersect
std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(std::pair<StreetIdx, StreetIdx> street_ids) {
    
    // creating 2 vectors that each store the intersections IDs of street_id1 and street_id2 respectively 
    std::vector <IntersectionIdx> intersectionsfOfStreet1 = findIntersectionsOfStreet(street_ids.first);
    std::vector <IntersectionIdx> intersectionsfOfStreet2 = findIntersectionsOfStreet(street_ids.second);
    
    // creating the vector we want to return 
    std::vector <IntersectionIdx> intersectionsOf2Streets;
    
    for (int i = 0; i < intersectionsfOfStreet1.size(); i++) {
        for (int j = 0; j < intersectionsfOfStreet2.size(); j++) {
            if (intersectionsfOfStreet1[i] == intersectionsfOfStreet2[j]) {
                intersectionsOf2Streets.push_back(intersectionsfOfStreet1[i]);
            }
        }  
    }
    return intersectionsOf2Streets;

}

// this function returns all intersections along the a given street.
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id) {
    // creating an unordered set to remove duplicates 
    std::unordered_set <IntersectionIdx>* intersectionsSet = streetIntersections[street_id];
    
    //created a vector in order to return intersection IDs in this vector 
    std::vector<IntersectionIdx> result(intersectionsSet->size());
    
    // copied all of the elements of the set in to the vector 
    std::copy(intersectionsSet->begin(), intersectionsSet->end(), result.begin());
    return result;
}

// this function returns the nearest point of interest of the given name to the given position
POIIdx findClosestPOI(LatLon my_position, std::string POIname) {
    double min_distance = INT_MAX;
    int near_POI_id = 0;
    for (int i = 0; i < getNumPointsOfInterest(); i++) {
        if (POIname == getPOIName(i)) {
            std::pair<LatLon, LatLon> pair_of_points = std::make_pair(my_position, getPOIPosition(i));
            double dist = findDistanceBetweenTwoPoints(pair_of_points);
            if (min_distance > dist) {
                min_distance = dist;
                near_POI_id = i;
            }
        }
    }
    return near_POI_id;
}

/*the following helper function is used in findStreetIdsFromPartialStreetName
 * It was inspired from a StackOverflow post found here : https://stackoverflow.com/questions/9349797/partial-match-for-the-key-of-a-stdmap
*/
std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix) {
 
    if (street_prefix.size() != 0) { 
        // removes whitespace
        street_prefix.erase(std::remove_if(street_prefix.begin(), street_prefix.end(), ::isspace), street_prefix.end()); 
         
        // transforms everything to lowercase
        std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower); 
        
        std::vector<StreetIdx> result;
        
        for(std::multimap <std::string, StreetIdx>::const_iterator i = streetsNamesAndIDs.begin(); i != streetsNamesAndIDs.end() 
                && streetsNamesAndIDs.lower_bound(street_prefix) !=  streetsNamesAndIDs.end(); ++i) {
            std::string key = i->first;
        
            // compares the street name elements to the street prefix 
            if (key.compare(0, street_prefix.size(), street_prefix) == 0){
                result.push_back(i->second);
            }
        }
       
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
    
        return result;
        
    } else {
        std::vector<StreetIdx> resultIsZero(0); // if the street_prefix is empty, we return 0
        return resultIsZero;
    }
}

// this function returns the smallest axis-aligned rectangle that contains all the  intersections and curve points of the given street
LatLonBounds findStreetBoundingBox(StreetIdx street_id) {
    //LatLonBounds a ; 
    std::vector <LatLon> intersectionsAndCurvePoints; // this vector will store all the LatLon coordinates of all of the intersections and Curve points found in street_id 
    std::vector<IntersectionIdx> intersections = findIntersectionsOfStreet(street_id);
    for (int i = 0; i < intersections.size(); ++i) {
        intersectionsAndCurvePoints.push_back(getIntersectionPosition(intersections[i]));
    }
    for (int i = 0; i < getNumStreetSegments(); ++i) {
        StreetSegmentInfo ss_info = getStreetSegmentInfo(i);
        if (ss_info.streetID == street_id) {
            for (int j = 0; j < ss_info.numCurvePoints; ++j) {
                intersectionsAndCurvePoints.push_back(getStreetSegmentCurvePoint(i, j));
            }
        }
    }
  /* we will be looping through the vector containing the LatLon coordinates to get the maxLat, maxLon, minLat and minLon
    */ 
    double maxLon = -180; 
    for (int l = 0; l < intersectionsAndCurvePoints.size(); ++l){
        if(intersectionsAndCurvePoints[l].longitude() > maxLon){
            maxLon = intersectionsAndCurvePoints[l].longitude(); 
         }
    }
    
    double minLon = 180; 
    for (int m = 0; m < intersectionsAndCurvePoints.size(); ++m){
        if(intersectionsAndCurvePoints[m].longitude() < minLon){
            minLon = intersectionsAndCurvePoints[m].longitude(); 
        }
    }
   
    
    double minLat = 90; 
    for (int m = 0; m < intersectionsAndCurvePoints.size(); ++m){
        if(intersectionsAndCurvePoints[m].latitude() < minLat){
            minLat = intersectionsAndCurvePoints[m].latitude(); 
        }
    }
    
    double maxLat = -90; 
    for (int m = 0; m < intersectionsAndCurvePoints.size(); ++m){
        if(intersectionsAndCurvePoints[m].latitude() > maxLat){
            maxLat = intersectionsAndCurvePoints[m].latitude(); 
        }
    }
        
    LatLon maxLL(maxLat, maxLon);
    LatLon minLL(minLat, minLon);
    
    LatLonBounds box{minLL, maxLL}; 
    return box; 
}


std::string appropriate_font(){
    std::string font; 
    if (map_name1 == "cairo"){
        font = "Noto Sans Arabic"; 
    }
    else if(map_name1 == "moscow"){
        font = "Noto Sans"; 
    }
    else if (map_name1 == "beijing"){
        font = "Noto Sans CJK TC";
    }
    else if(map_name1 == "unknown"){
        font = "serif"; 
    }
    else{
        font = "serif"; 
    }     
    return font; 
}
