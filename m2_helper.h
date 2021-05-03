/* To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2_helper_functions.h
 * Author: kambrisr
 * Created on February 18, 2021, 12:31 AM
 */
#pragma once

#include <vector>
#include <sstream>
#include <string>
#include "vars.h"

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/callback.hpp"


double x_from_lon(double lon);
double y_from_lat(double lat);
double lat_from_y(double y);
double lon_from_x(double x);


// functions for x/y to LATLON conversions and vice versa
inline double x_from_lon(double lon){
    return kEarthRadiusInMeters*lon*kDegreeToRadian*std::cos(kDegreeToRadian*(avg_lat));
}
inline double y_from_lat(double lat){
    return lat*kEarthRadiusInMeters*kDegreeToRadian;
}
inline double lat_from_y(double y){
    return y/kEarthRadiusInMeters/kDegreeToRadian;
}
inline double lon_from_x(double x){
    return x/kEarthRadiusInMeters/kDegreeToRadian/std::cos(kDegreeToRadian*(avg_lat));
}


// callback functions 
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_mouse_move(ezgl::application *application, GdkEventButton *event, double x, double y);
void act_on_key_press(ezgl::application *application, GdkEventKey *event, char *key_name);
void initial_setup(ezgl::application *application, bool /*new_window*/);

/* You must be able to visualize streets, intersections, points of interest and features (lakes,
buildings, etc.) - essentially all the items provided by the layer 2 StreetsDatabaseAPI.h.*/
float get_degree(LatLon from, LatLon to); //get degree from two position
// draw main canvas that invokes all other drawing functions

void draw_main_canvas(ezgl::renderer *g); 


// basic drawing functions
void draw_intersections(ezgl::renderer *g);
void unhighlight_intersection(); 
void draw_street(ezgl::renderer *g);
void draw_street(float to_lon, float to_lat, float from_lon, float from_lat, std::string type, ezgl::renderer *g); 
void draw_street_name(ezgl::renderer *g);
void draw_main_street(ezgl::renderer *g);
void draw_highways(ezgl::renderer *g);
void draw_OneWay(ezgl::renderer *g,LatLon , LatLon );
void draw_feature(ezgl::renderer *g);
void draw_POI(ezgl::renderer *g); 




// functions for search bar, highlighting intersections, and helper functions for them
void on_find_clicked(GtkButton* button, GtkEntry* search_entry); 
bool checkStreamErrors(std::stringstream &userInput); 
void highlight_intersections(std::vector<IntersectionIdx> intersections); 
void parse_search_bar(const char* text); 

/* Extra Feature: Symbols for hospitals, restaurants and subway stations*/
void draw_hospital_symbol(ezgl::renderer *g); 
void draw_restaurants(ezgl::renderer *g);
void draw_subway_stations(ezgl::renderer *g);




// m3
/*********************** Find Fastest Path helper functions ********************/
void draw_fastest_path(std::vector<StreetSegmentIdx> result,ezgl::renderer *g);

void draw_fastest_path_in_load_map(ezgl::renderer *g); 

//helper functions for displaying directions
void print_help_fastest_path(ezgl::rectangle rect, ezgl::renderer *g);
void print_fastest_path (std::vector<StreetSegmentIdx> result, int start, int end, double turn_penalty, ezgl::renderer *g);
void disable_interest_button (GtkButton*);
std::string get_direction(double degree);


//helper functions for displaying invalid input
void invalid_input_box(float x, float y, float width, ezgl::renderer *g);
void draw_invalid_input( ezgl::renderer *g);

// helper functions for drawing path and displaying directions 
void parse_search_bar2(std::string text, ezgl::renderer *g);

void draw_text_box ( ezgl::rectangle rect, float width, float length, ezgl::renderer *g);

// callback functions for newly created buttons
void on_help_clicked(GtkButton* button, ezgl::application *application); 
void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data); 
void on_fastest_path_clicked(GtkButton* button, GtkEntry* search_entry );

