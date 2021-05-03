#pragma once
#include <vector>



struct DeliveryInf {
    //Specifies a delivery order (input to your algorithm).
    //
    //To satisfy the order the item-to-be-delivered must have been picked-up 
    //from the pickUp intersection before visiting the dropOff intersection.

    DeliveryInf(int pick_up, int drop_off)
        : pickUp(pick_up), dropOff(drop_off) {}

    //The intersection id where the item-to-be-delivered is picked-up.
    int pickUp;

    //The intersection id where the item-to-be-delivered is dropped-off.
    int dropOff;
};


struct CourierSubPath { 
    // Specifies one subpath of the courier truck route

    // The intersection id where a start depot, pick-up intersection or drop-off intersection 
    // is located
    int start_intersection;

    // The intersection id where this subpath ends. This must be the 
    // start_intersection of the next subpath or the intersection of an end depot
    int end_intersection;

    // Street segment ids of the path between start_intersection and end_intersection 
    // They form a connected path (see m3.h)
    std::vector<int> subpath;
};


// This routine takes in a vector of N deliveries (pickUp, dropOff
// intersection pairs), another vector of M intersections that
// are legal start and end points for the path (depots), and a turn 
// penalty in seconds (see m3.h for details on turn penalties).
//
// The first vector 'deliveries' gives the delivery information.  Each delivery
// in this vector has pickUp and dropOff intersection ids.
// A delivery can only be dropped-off after the associated item has been picked-up. 
// 
// The second vector 'depots' gives the intersection ids of courier company
// depots containing trucks; you start at any one of these depots and end at
// any one of the depots.
//
// This routine returns a vector of CourierSubPath objects that form a delivery route.
// The CourierSubPath is as defined above. The first street segment id in the
// first subpath is connected to a depot intersection, and the last street
// segment id of the last subpath also connects to a depot intersection.
// A package should not be dropped off if you haven't picked it up yet.
//
// The start_intersection of each subpath in the returned vector should be 
// at least one of the following (a pick-up and/or drop-off can only happen at 
// the start_intersection of a CourierSubPath object):
//      1- A start depot.
//      2- A pick-up location
//      3- A drop-off location. 
//
// You can assume that N is always at least one, M is always at least one
// (i.e. both input vectors are non-empty).
//
// It is legal for the same intersection to appear multiple times in the pickUp
// or dropOff list (e.g. you might have two deliveries with a pickUp
// intersection id of #50). The same intersection can also appear as both a
// pickUp location and a dropOff location.
//        
// If you have two pickUps to make at an intersection, traversing the
// intersection once is sufficient to pick up both packages. Additionally, 
// one traversal of an intersection is sufficient to drop off all the 
// (already picked up) packages that need to be dropped off at that intersection.
//
// Depots will never appear as pickUp or dropOff locations for deliveries.
//  
// If no valid route to make *all* the deliveries exists, this routine must
// return an empty (size == 0) vector.
std::vector<CourierSubPath> travelingCourier(
		            const std::vector<DeliveryInf>& deliveries,
	       	        const std::vector<int>& depots, 
		            const float turn_penalty);
