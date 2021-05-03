/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3_helper_functions.h
 * Author: kambrisr
 *
 * Created on March 27, 2021, 11:04 AM
 */

#pragma once
#include "structs.h"

bool  bfsPath(Node2* sourceNode, int destID, double turn_penalty); 
std::vector<StreetSegmentIdx> bfsTraceBack(int destID); 



