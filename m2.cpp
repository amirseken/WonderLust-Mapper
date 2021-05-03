#include "m1.h"
#include "m1_helper_functions.h"
#include "m2.h"
#include "m2_helper.h"
#include "m3.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/callback.hpp"
#include "ezgl/canvas.hpp"
#include "ezgl/rectangle.hpp"
#include <bits/stdc++.h>

#include <cmath>
#include <string> 
#include <algorithm> 
#include <chrono>
#include <thread>
#include <sstream>

//error range that is used to display map
#define MINIMUM_ERROR_RANGE 0.00005
#define MAXIMUM_ERROR_RANGE 0.0005

// pre-calculated constants
#define APPEAR_ZOOM_IN 10012122
#define BUILDINGS_APPEAR 5.33650E+06
#define APPEAR_RESTAURANT 116163
#define APPEAR_POI 896323

#define PI 3.14159265
#define APPEAR_TEXT 122678
#define STREETS_ZOOM_IN 20336480
#define SUBWAY_ZOOM_IN 2336480



#define FIRST_SCREEN_AREA 259645
#define HIGHWAY_SCREEN_AREA 447331
#define STREET_SCREEN_AREA 76332
#define BUILDING_SCREEN_AREA 15805
#define STREETNAME_SCREEN_AREA 21385
#define ARROW_SCREEN_AREA 74258


// global variables for average latitude
double avg_lat = 0;

// global variables for defining bounds of the map
ezgl::rectangle initial_world;
// global variables for whether or not intersection was pressed
Intersection lightedPoint;

//m3 global variables

//global variable to check for validity of input
bool invalid_input = false;
//global variable to check if user pressed on two intersections
bool done = false;
//global variable to check if user typed in the search bar
bool fast_type = false;


//global variable to check if user clicked onto 
bool fastest_click_button = false;
std::string fast_search;
bool click = false;
std::vector <IntersectionIdx> two_intersections;
std::vector <int> Found_Path;

void draw_main_canvas(ezgl::renderer *g) {

    // to check the response time for the functions 
    // auto startTime = std::chrono::high_resolution_clock::now();
    draw_feature(g);
    draw_street(g);
    draw_intersections(g);
    draw_POI(g);
    draw_hospital_symbol(g);
    draw_subway_stations(g);
    draw_restaurants(g);
    //draw_street_name(g);

    //m3
    //check if the user finds fastest path by clicking on the intersections. 
    if (fastest_click_button && done) {

        draw_fastest_path_in_load_map(g);
        fastest_click_button = false;
        done = false;
        std::cout << "The path has been drawn." << std::endl;
        std::cout << "If you want to draw another path, please click Disable button and then Fastest path button." << std::endl;

    } 
    //check if the user finds fastest path by searching the intersections in the search bar. 
    else if (fastest_click_button && fast_type) {

        parse_search_bar2(fast_search, g);
        fastest_click_button = false;
        fast_type = false;
        std::cout << "If you want to draw another path, please click Disable button and then Fastest path button." << std::endl;

    }
    else {
        draw_street_name(g);
    }
    //draw_street_name(g);
    draw_invalid_input(g);
    //auto currentTime = std::chrono::high_resolution_clock::now();
    // auto wallClock = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime); 
    //std::cout<<" Response time of the mapper is"<<wallClock.count()<<" seconds "<<std::endl;
}


// draw Map with all of its features

void draw_map() {

    //defining map bounds
    double max_lat = getIntersectionPosition(0).latitude();
    double min_lat = max_lat;
    double max_lon = getIntersectionPosition(0).longitude();
    double min_lon = max_lon;

    Intersection_Info.resize(getNumIntersections());

    for (int id = 0; id < getNumIntersections(); ++id) {
        Intersection_Info[id].position = getIntersectionPosition(id);
        Intersection_Info[id].name = getIntersectionName(id);

        max_lat = std::max(max_lat, Intersection_Info[id].position.latitude());
        min_lat = std::min(min_lat, Intersection_Info[id].position.latitude());
        max_lon = std::max(max_lon, Intersection_Info[id].position.longitude());
        min_lon = std::min(min_lon, Intersection_Info[id].position.longitude());
    }
    // calculating average latitude
    avg_lat = (max_lat + min_lat) / 2;
    //Setup code
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";

    ezgl::application application(settings);

    // determining bounds for initial world
    initial_world.m_first = {x_from_lon(min_lon), y_from_lat(min_lat)};
    initial_world.m_second = {x_from_lon(max_lon), y_from_lat(max_lat)};

    application.add_canvas("MainCanvas", draw_main_canvas, initial_world);

    application.run(initial_setup, act_on_mouse_press, NULL, NULL);

}

void on_find_clicked(GtkButton* , GtkEntry* search_entry) {
    
    const char* text = gtk_entry_get_text(search_entry);
    std::cout << text << std::endl;
    parse_search_bar(text);

}

bool checkStreamErrors(std::stringstream &userInput) {

    if (userInput.fail() || ((userInput.peek() != ' ') && !userInput.eof())) {
        userInput.clear();
        userInput.ignore(1000, '\n');
        return true;
    } else {
        return false;
    }
}

void parse_search_bar(const char* text) {
    std::string street1, street2;
    std::stringstream line_stream;
    line_stream << text;
    std::vector<StreetIdx> street_1_ids;
    std::vector<StreetIdx> street_2_ids;
    std::vector<IntersectionIdx> intersections;

    line_stream >> street1;
    line_stream >> street2;
    if (checkStreamErrors(line_stream)) {
        std::cout << "Invalid search entry" << std::endl;
    } else {
        street_1_ids = findStreetIdsFromPartialStreetName(street1);
        street_2_ids = findStreetIdsFromPartialStreetName(street2);
        std::cout << "The intersections of " << street1 << " and " << street2 << " are: " << std::endl;

        for (int i = 0; i < street_1_ids.size(); i++) {
            for (int j = 0; j < street_2_ids.size(); j++) {
                intersections = findIntersectionsOfTwoStreets(std::make_pair(street_1_ids[i], street_2_ids[j]));
                if (!intersections.empty()) {
                    highlight_intersections(intersections);
                }
            }
        }
    }
}

void initial_setup(ezgl::application *application, bool /*new_window*/) {
    // Update the status bar message
    application->update_message("Geographic Information System @2021");


    g_signal_connect(application->get_object("Find"), "clicked", G_CALLBACK(on_find_clicked), application->get_object("searchEntry"));
    g_signal_connect(application->get_object("Disable"), "clicked", G_CALLBACK(disable_interest_button), NULL);
    g_signal_connect(application->get_object("Help"), "clicked", G_CALLBACK(on_help_clicked), NULL);
    g_signal_connect(application->get_object("Path"), "clicked", G_CALLBACK(on_fastest_path_clicked), application->get_object("searchEntry"));


}

// the "disable" function un-highlights and un-selects intersections and street segments
void disable_interest_button(GtkButton* ) {

    done = false;
    fast_type = false;
    fastest_click_button = false;
    invalid_input = false;

    if (!two_intersections.empty()) {
        two_intersections.clear();
    }
    if (!Found_Path.empty()) {
        Found_Path.clear();
    }

    unhighlight_intersection();

    std::cout << " Map is restarted(intersections are unhighlighted, buttons are ready to be pressed, etc.) " << std::endl;
}
// this is the callback function for when "Fastest Path" button is clicked
void on_fastest_path_clicked(GtkButton* , GtkEntry* search_entry) {

    std::cout << "Fastest path button is clicked." << std::endl;
    
    fastest_click_button = true;

    const char* text = gtk_entry_get_text(search_entry);

    std::string text1(text);

    // if the intersections are clicked 
    if (click && done) {

        return;
    }
    // if the intersections are typed in the search bar. 
    else if (text[0] != '\0') {

        fast_search = text1;

        fast_type = true;
        return;
    }
}
// this is the callback function for when "Help" button is clicked
void on_help_clicked(GtkButton* , ezgl::application *) {
    
    GObject *window = NULL; // the parent window over which to add the dialog
    GtkWidget *content_area; // the content area of the dialog
   
    GtkWidget *label; // the label we will create to display a message in the content area
   
    GtkWidget *dialog; // the dialog box we will create

    // Create the dialog window.
    // Modal windows prevent interaction with other windows in the same application
    dialog = gtk_dialog_new_with_buttons(
            "Help",
            (GtkWindow*) window,
            GTK_DIALOG_MODAL,
            ("OK"),
            GTK_RESPONSE_ACCEPT,
            ("CANCEL"),
            GTK_RESPONSE_REJECT,
            NULL
            );

    // Create a label and attach it to the content area of the dialog
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("- To find the path between 2 points, you can either click on 2 intersections anywhere on the map or you can type in the names of 2 intersections in the search bar.\n - To write down the names of 2 intersections in the search ba, put in the names of 2 streets. \n -To disable the path that is shown on the map, click on the Disable Button.\n - As you zoom in, the following will appear (in order): secondary roads, buildings, intersections, points of interests, points of interest names. ");
    gtk_container_add(GTK_CONTAINER(content_area), label);

    // The main purpose of this is to show dialog??s child widget, label
    gtk_widget_show_all(dialog);

    // Connecting the "response" signal from the user to the associated callback function
    g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_dialog_response), NULL);
}

void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer ) {
    // For demonstration purposes, this will show the int value of the response type
    std::cout << "response is ";
    switch (response_id) {
        case GTK_RESPONSE_ACCEPT:
            std::cout << "GTK_RESPONSE_ACCEPT ";
            break;
        case GTK_RESPONSE_DELETE_EVENT:
            std::cout << "GTK_RESPONSE_DELETE_EVENT (i.e. ??X?? button) ";
            break;
        case GTK_RESPONSE_REJECT:
            std::cout << "GTK_RESPONSE_REJECT ";
            break;
        default:
            std::cout << "UNKNOWN ";
            break;
    }
    std::cout << "(" << response_id << ")\n";
    /*This will cause the dialog to be destroyed*/
    gtk_widget_destroy(GTK_WIDGET(dialog));

}

// parses the search for intersection input
void parse_search_bar2(std::string text, ezgl::renderer *g) {

    std::string id1, id2, id3, id4;
    std::stringstream line_stream;

    line_stream << text;

    std::vector<StreetIdx> ids1;
    std::vector<StreetIdx> ids2;
    std::vector<StreetIdx> ids3;
    std::vector<StreetIdx> ids4;
    std::vector<IntersectionIdx> start;
    std::vector<IntersectionIdx> destination;


    line_stream >> id1;
    line_stream >> id2;
    line_stream >> id3;
    line_stream >> id4;


    if (checkStreamErrors(line_stream)) {
        std::cout << "Invalid search entry. Please try again." << std::endl;
        invalid_input = true;
    } else {



        ids1 = findStreetIdsFromPartialStreetName(id1);
        ids2 = findStreetIdsFromPartialStreetName(id2);
        ids3 = findStreetIdsFromPartialStreetName(id3);
        ids4 = findStreetIdsFromPartialStreetName(id4);

        if (ids1.empty() || ids2.empty() || ids3.empty() || ids4.empty()) {
            std::cout << "Invalid search entry. One of the intersections does not exist. Please try again." << std::endl;

            invalid_input = true;
        }
        else {

            int k = 0;
            int l = 0;

            for (int i = 0; i < ids1.size(); i++) {
                for (int j = 0; i < ids2.size(); j++) {

                    start = findIntersectionsOfTwoStreets(std::make_pair(ids1[i], ids2[j]));

                    if (!start.empty()) {
                        l++;
                        break;
                    }
                }
                if (l > 0) {
                    break;
                }
            }
            for (int i = 0; i < ids3.size(); i++) {
                for (int j = 0; i < ids4.size(); j++) {

                    destination = findIntersectionsOfTwoStreets(std::make_pair(ids3[i], ids4[j]));

                    if (!destination.empty()) {
                        k++;
                        break;
                    }
                }
                if (k > 0) {
                    break;
                }
            }
            if (start[0] == destination[0]) {
                std::cout << "Invalid search entry. Intersections are the same. Please enter different intersections." << std::endl;

                invalid_input = true;
            }
            else {
                std::cout << "The path between " << id1 << " " << id2 << " and " << id3 << " " << id4 << " is drawn." << std::endl;

                Found_Path = findPathBetweenIntersections(start[0], destination[0], 15);

                draw_fastest_path(Found_Path, g);
                draw_street_name(g);
                print_fastest_path(Found_Path, start[0], destination[0], 15, g);
                //Found_Path.clear();
                start.clear();
                destination.clear();
                ids1.clear();
                ids2.clear();
                ids3.clear();
                ids4.clear();
            }

        }
    }
}

void act_on_mouse_press(ezgl::application *application, GdkEventButton* , double x, double y) {
    application->update_message("Mouse Clicked");

    // Displaying information about intersections and highlighting intersections
    LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));

    int id = findClosestIntersection(pos);

    if (fastest_click_button) {
        click = true;
        std::cout << "Closest Intersection: " << Intersection_Info[id].name << std::endl;
        Intersection_Info[id].highlight = true;

        application->refresh_drawing();
        two_intersections.push_back(id);
        if (two_intersections.size() == 2) {
            std::cout << "Drawing path" << std::endl;

            done = true;
        }
    } else {
        unhighlight_intersection();
    }
    // redrawing the map
    application->refresh_drawing();
}





// drawing intersections on the map

void draw_intersections(ezgl::renderer *g) {

    ezgl::rectangle current_screen = g->get_visible_world();
    if (current_screen.area() < APPEAR_ZOOM_IN) {
        for (int i = 0; i < Intersection_Info.size(); ++i) {
            float x = x_from_lon(Intersection_Info[i].position.longitude());
            float y = y_from_lat(Intersection_Info[i].position.latitude());

            float width = 10;
            float height = width;
            //highlight an intersection in red
            if (Intersection_Info[i].highlight) {
                g->set_color(ezgl::RED);
            }
            else {
                g->set_color(ezgl::GREY_55);
            }
            g->fill_rectangle({x - width / 2, y - height / 2},
            {
                x + width / 2, y + height / 2
            });
        }
    }

}

// highlight all intersections from the vector and display the name

void highlight_intersections(std::vector<IntersectionIdx> intersections) {
    for (int i = 0; i < intersections.size(); i++) {
        std::cout << Intersection_Info[intersections[i]].name << std::endl;
        Intersection_Info[intersections[i]].highlight = true;
    }
}

// unhighlight intersections

void unhighlight_intersection() {
    for (int i = 0; i < getNumIntersections(); i++) {
        if (Intersection_Info[i].highlight == true) {
            Intersection_Info[i].highlight = false;
        }
    }
}


// draw oneWay streets on the map

void draw_OneWay(ezgl::renderer *g, LatLon start, LatLon end) {

    g->set_color(ezgl::KHAKI);
    g->set_line_cap(ezgl::line_cap::round);
    g->set_line_width(5);

    g->draw_line({x_from_lon(start.longitude()), y_from_lat(start.latitude())},
    {
        x_from_lon(end.longitude()), y_from_lat(end.latitude())
    });
}


// draw streets 

void draw_street(ezgl::renderer *g) {

    g->set_line_width(0);
    std::string type1 = "normal";

    ezgl::rectangle current_screen = g->get_visible_world();
    ezgl::point2d max_screen_cordin{current_screen.right(), current_screen.top()};
    ezgl::point2d min_screen_cordin{current_screen.left(), current_screen.bottom()};

    if (current_screen.area() < APPEAR_ZOOM_IN) {
        for (int i = 0; i < getNumStreetSegments(); ++i) {

            StreetSegment info;
            LatLon from, to;

            info = Street_Segment_Info[i];

            from = Intersection_Info[info.from].position;
            to = Intersection_Info[info.to].position;

            double x1, y1;
            double x2, y2;

            x1 = x_from_lon(from.longitude());
            y1 = y_from_lat(from.latitude());
            x2 = x_from_lon(to.longitude());
            y2 = y_from_lat(to.latitude());

            if (max_screen_cordin.x > x1 || max_screen_cordin.x > x2
                    || min_screen_cordin.x < x1 || min_screen_cordin.x < x2
                    || max_screen_cordin.y < y1 || max_screen_cordin.y > y2
                    || min_screen_cordin.y < y1 || min_screen_cordin.y < y2) {

                if (info.oneWay == true) {
                    type1 = "oneWay";
                } else {
                    type1 = "normal";
                }

                if (info.curvePointCount != 0) {

                    std::vector<LatLon> curve = info.curvePointPosition;
                    int size = info.curvePointCount;


                    draw_street(x1, y1, x_from_lon(curve[0].longitude()), y_from_lat(curve[0].latitude()), type1, g);

                    for (int j = 0; j < info.curvePointCount - 1; j++) {

                        draw_street(x_from_lon(curve[j].longitude()), y_from_lat(curve[j].latitude()), x_from_lon(curve[j + 1].longitude()), y_from_lat(curve[j + 1].latitude()), type1, g);

                    }
                    draw_street(x_from_lon(to.longitude()), y_from_lat(to.latitude()), x_from_lon(curve[size - 1].longitude()), y_from_lat(curve[size - 1].latitude()), type1, g);

                } else {
                    draw_street(x_from_lon(from.longitude()), y_from_lat(from.latitude()), x_from_lon(to.longitude()), y_from_lat(to.latitude()), type1, g);
                }

            }
        }

    }
    if (current_screen.area() < STREETS_ZOOM_IN) {
        draw_main_street(g);
    }
    draw_highways(g);

    //draw one-way street segments
    if (current_screen.area() < APPEAR_ZOOM_IN) {
        for (int i = 0; i < getNumStreetSegments(); i++) {

            LatLon from, to;

            from = Intersection_Info[Street_Segment_Info[i].from].position;
            to = Intersection_Info[Street_Segment_Info[i].to].position;

            if (Street_Segment_Info[i].oneWay == true) {


                if (Street_Segment_Info[i].curvePointCount > 0) {

                    draw_OneWay(g, from, Street_Segment_Info[i].curvePointPosition[0]);
                    for (int j = 1; j <= Street_Segment_Info[i].curvePointCount - 1; j++) {
                        draw_OneWay(g, Street_Segment_Info[i].curvePointPosition[j - 1], Street_Segment_Info[i].curvePointPosition[j]);
                    }
                    draw_OneWay(g, Street_Segment_Info[i].curvePointPosition[Street_Segment_Info[i].curvePointCount - 1], to);
                } else {
                    draw_OneWay(g, from, to);
                }
            }
        }
    }
}



//draw streets - helper function

void draw_street(float to_lon, float to_lat, float from_lon, float from_lat, std::string type1, ezgl::renderer *g) {

    if (type1 == "fastest_path") {

        g->set_color(ezgl::RED);
        g->set_line_dash(ezgl::line_dash::asymmetric_5_3);
    }


    if (type1 == "highway") {


        g->set_color(ezgl::ORANGE);
        g->set_line_dash(ezgl::line_dash::none);
    }
    else if (type1 == "main") {

        g->set_color(ezgl::GREY_75);
        g->set_line_dash(ezgl::line_dash::none);
    }
    else if (type1 == "normal") {

        g->set_color(ezgl::GREY_55);
        g->set_line_dash(ezgl::line_dash::none);
    }

    g->set_line_width(4);
    g->draw_line({to_lon, to_lat},
    {
        from_lon, from_lat
    });

}


// draw main streets on the map

void draw_main_street(ezgl::renderer *g) {
    std::string type1 = "main";
    for (std::vector<Node>::iterator it = Main_Street_Nodes.begin(); it < Main_Street_Nodes.end(); it++) {
        std::vector<Node>::iterator it_next = it;
        it_next++;
        int tag1 = it->tag;
        int tag2 = it_next->tag;

        if (tag1 == tag2) {
            LatLon posFrom = it->position;
            LatLon posTo = it_next->position;
            draw_street(x_from_lon(posFrom.longitude()), y_from_lat(posFrom.latitude()), x_from_lon(posTo.longitude()), y_from_lat(posTo.latitude()), type1, g);
        }
    }
}

//get degree from two position
float get_degree(LatLon from, LatLon to) { 
    float degree;
    float x = x_from_lon(to.longitude() - from.longitude());
    float y = y_from_lat(to.latitude() - from.latitude());
    degree = std::fabs((std::atan(y / x)* 180 / PI));


    if (x < 0 && y > 0) {
        degree = (90 - degree) + 90;
    } else if (x < 0 && y < 0) {
        degree = degree + 180;
    } else if (x > 0 && y < 0) {
        degree = (90 - degree) + 270;
    } else {
        degree = degree;
    }
    return degree;
}

// draw street names on the map
void draw_street_name(ezgl::renderer *g) {
    std::string street_name_compare = "INVALID";
    int repetitive_name = 0;

    g->format_font(appropriate_font(), ezgl::font_slant::normal, ezgl::font_weight::normal);
    for (int i = 0; i < Street_Segment_Info.size(); i++) {

        LatLon from = Intersection_Info[Street_Segment_Info[i].from].position;
        LatLon to = Intersection_Info[Street_Segment_Info[i].to].position;
        std::string street_name = Street_Info[Street_Segment_Info[i].streetID].Name;


        float largest_side = std::fabs(y_from_lat(from.latitude() - to.latitude()));

        if (largest_side < std::fabs(x_from_lon(from.longitude() - to.longitude()))) {
            largest_side = std::fabs(x_from_lon(from.longitude() - to.longitude()));
        }

        if (Street_Segment_Info[i].curvePointCount > 0) {
            int median = std::round(Street_Segment_Info[i].curvePointCount / 2);
            from = Street_Segment_Info[i].curvePointPosition[median - 1];
            to = Street_Segment_Info[i].curvePointPosition[median];
        }

        if (repetitive_name == 5) {
            street_name_compare = "INVALID";
            repetitive_name = 0;
        }

        if (street_name != "<unknown>" && street_name != street_name_compare) {

            g->set_font_size(13);
            g->set_color(ezgl::BLACK);
            /*
           double degree = get_degree(from, to);

           // check if text is inverted
           if (degree> 100 && degree< 260){ 
               degree = degree + 180;
               //check if angle is bigger than 360 degrees
           
               if(degree>=360){
                   degree = degree - 360; 
               }
           }
             */

            //else{
            repetitive_name = repetitive_name + 1;
            //}


            //g->set_text_rotation(degree);

            if (Street_Segment_Info[i].oneWay == true) {
                g->set_font_size(18);
                g->draw_text(ezgl::point2d(x_from_lon((from.longitude() + to.longitude()) / 2), y_from_lat((to.latitude() + from.latitude()) / 2)), street_name + " -> ", largest_side, largest_side);
            } else {
                g->draw_text(ezgl::point2d(x_from_lon((from.longitude() + to.longitude()) / 2), y_from_lat((to.latitude() + from.latitude()) / 2)), street_name, largest_side, largest_side);
            }
            street_name_compare = street_name;
        }
    }
    //g->set_text_rotation(0);
}


// draw highways on the map
void draw_highways(ezgl::renderer *g) {
    std::string type1 = "highway";
    for (std::vector<Node>::iterator it = Highway_Nodes.begin(); it < Highway_Nodes.end(); it++) {
        std::vector<Node>::iterator it_next = it;
        it_next++;
        int tag1 = it->tag;
        int tag2 = it_next->tag;

        if (tag1 == tag2) {
            LatLon posFrom = it->position;
            LatLon posTo = it_next->position;
            draw_street(x_from_lon(posFrom.longitude()), y_from_lat(posFrom.latitude()), x_from_lon(posTo.longitude()), y_from_lat(posTo.latitude()), type1, g);
        }
    }
}


// draw hospital symbols
void draw_hospital_symbol(ezgl::renderer *g) {
    ezgl::rectangle current_screen = g->get_visible_world();
    if (std::abs(current_screen.area()) < SUBWAY_ZOOM_IN) {
        const char *path = "libstreetmap/resources/hospital3.png";
        static ezgl::surface* create_surface = g->load_png(path);

        for (unsigned k = 0; k < osm_hospitals.size(); k++) {
            LatLon coords = getNodeCoords(osm_hospitals[k]);
            double x = x_from_lon(coords.longitude());
            double y = y_from_lat(coords.latitude());
            g->draw_surface(create_surface, ezgl::point2d(x, y));
        }
    }


}


// draw restaurant symbols
void draw_restaurants(ezgl::renderer *g) {
    const char *path = "libstreetmap/resources/restaurant2.png";
    static ezgl::surface* create_surface = g->load_png(path);
    ezgl::rectangle current_screen = g->get_visible_world();
    if (current_screen.area() < APPEAR_RESTAURANT) {
        for (unsigned k = 0; k < osm_restaurants.size(); k++) {
            LatLon coords = getNodeCoords(osm_restaurants[k]);
            double x = x_from_lon(coords.longitude());
            double y = y_from_lat(coords.latitude());
            g->draw_surface(create_surface, ezgl::point2d(x + 0.5, y - 0.7));
        }
    }

}

void draw_feature(ezgl::renderer *g) {


    //get current screen information here, since draw_nature is first layer of the map.
    ezgl::rectangle current_screen = g->get_visible_world();
    ezgl::point2d max_screen_cordin = current_screen.top_right();
    ezgl::point2d min_screen_cordin = current_screen.bottom_left();

    g->set_line_width(5);


    //draw  nature features.
    for (int i = 0; i < getNumFeatures(); i++) {

        int p_tot = Feature_Info[i].Point_Count;
        ezgl::point2d a(2, 3);
        std::vector<ezgl::point2d> points(10, a);
        points.resize(p_tot);

        FeatureType type1 = Feature_Info[i].Feature_type;
        switch (type1) {
            case PARK:
                g->set_color(34, 139, 34);
                break;
            case GREENSPACE:
                g->set_color(118, 186, 27);
                break;
            case BEACH:
                g->set_color(29, 151, 193);
                break;
            case STREAM:
                g->set_color(212, 241, 249);
                break;
            case ISLAND:
                g->set_color(246, 228, 173);
                break;
            case LAKE:
                g->set_color(87, 157, 166);
                break;
            case RIVER:
                g->set_color(0, 121, 180);
                break;
            case GOLFCOURSE:
                g->set_color(52, 101, 102);
                break;

            case BUILDING:
                g->set_color(184, 186, 186);
                break;
            case UNKNOWN:
                g->set_color(ezgl::BLACK);
                break;

            default:
                break;
        }


        //store position of each nature to draw polygon
        for (int j = 0; j < p_tot; j++) {
            points[j].x = x_from_lon(Feature_Info[i].Point_Position[j].longitude());
            points[j].y = y_from_lat(Feature_Info[i].Point_Position[j].latitude());
        }

        //draw nature that consists of only line
        if (x_from_lon(Feature_Info[i].Point_Position[0].longitude()) != x_from_lon(Feature_Info[i].Point_Position[p_tot - 1].longitude())
                && y_from_lat(Feature_Info[i].Point_Position[0].latitude()) != y_from_lat(Feature_Info[i].Point_Position[p_tot - 1].latitude())) {

            for (int j = 0; j < p_tot - 1; j++) {
                double temp_lon = x_from_lon(Feature_Info[i].Point_Position[j].longitude());
                double temp_lat = y_from_lat(Feature_Info[i].Point_Position[j].latitude());

                //draw nature only inside window.
                if (max_screen_cordin.x > temp_lon || min_screen_cordin.x < temp_lon
                        || max_screen_cordin.y > temp_lat || min_screen_cordin.y < temp_lat)

                    g->draw_line({x_from_lon(Feature_Info[i].Point_Position[j + 1].longitude()), y_from_lat(Feature_Info[i].Point_Position[j + 1].latitude())},
                {
                    temp_lon, temp_lat
                });
            }
        }
            //draw nature that is polygon shape
        else if (type1 == BUILDING) {
            if (current_screen.area() < BUILDINGS_APPEAR) {
                if (max_screen_cordin.x > points[0].x || min_screen_cordin.x < points[0].x
                        || max_screen_cordin.y > points[0].y || min_screen_cordin.y < points[0].y
                        || max_screen_cordin.x > points[p_tot - 1].x || min_screen_cordin.x < points[p_tot - 1].x
                        || max_screen_cordin.y > points[p_tot - 1].y || min_screen_cordin.y < points[p_tot - 1].y)
                    if (p_tot > 1) {
                        g->fill_poly(points);
                    }

                for (int j = 0; j < (p_tot) - 1; j++) {
                    float temp_lon = x_from_lon(Feature_Info[i].Point_Position[j].longitude());
                    float temp_lat = y_from_lat(Feature_Info[i].Point_Position[j].latitude());

                    g->set_color(ezgl::RED, 0);
                    g->set_line_width(0);

                    if (max_screen_cordin.x > temp_lon || min_screen_cordin.x < temp_lon
                            || max_screen_cordin.y > temp_lat || min_screen_cordin.y < temp_lat)

                        g->draw_line({x_from_lon(Feature_Info[i].Point_Position[j + 1].longitude()), y_from_lat(Feature_Info[i].Point_Position[j + 1].latitude())},
                    {
                        temp_lon, temp_lat
                    });
                }
            }
        } else {

            if (max_screen_cordin.x > points[0].x || min_screen_cordin.x < points[0].x
                    || max_screen_cordin.y > points[0].y || min_screen_cordin.y < points[0].y
                    || max_screen_cordin.x > points[p_tot - 1].x || min_screen_cordin.x < points[p_tot - 1].x
                    || max_screen_cordin.y > points[p_tot - 1].y || min_screen_cordin.y < points[p_tot - 1].y)
                if (p_tot > 1) {
                    g->fill_poly(points);
                }

            for (int j = 0; j < (p_tot) - 1; j++) {
                float temp_lon = x_from_lon(Feature_Info[i].Point_Position[j].longitude());
                float temp_lat = y_from_lat(Feature_Info[i].Point_Position[j].latitude());

                g->set_color(ezgl::RED, 0);
                g->set_line_width(0);

                if (max_screen_cordin.x > temp_lon || min_screen_cordin.x < temp_lon
                        || max_screen_cordin.y > temp_lat || min_screen_cordin.y < temp_lat)

                    g->draw_line({x_from_lon(Feature_Info[i].Point_Position[j + 1].longitude()), y_from_lat(Feature_Info[i].Point_Position[j + 1].latitude())},
                {
                    temp_lon, temp_lat
                });
            }


        }

    }

}


// draw subway stations
void draw_subway_stations(ezgl::renderer *g) {

    ezgl::rectangle current_screen = g->get_visible_world();
    if (std::abs(current_screen.area()) < SUBWAY_ZOOM_IN) {
        const char *path = "libstreetmap/resources/station.png";
        static ezgl::surface* create_surface = g->load_png(path);

        for (int k = 0; k < Subway_Stations.size(); k++) {
            LatLon coords = getNodeCoords(Subway_Stations[k]);
            double x = x_from_lon(coords.longitude());
            double y = y_from_lat(coords.latitude());
            g->draw_surface(create_surface, ezgl::point2d(x, y));
        }
    }

}



// draw POI on the map
void draw_POI(ezgl::renderer *g) {
    ezgl::rectangle current_screen = g->get_visible_world();

    float width = 15;
    float height = 10;
    for (int i = 0; i < POI_Info.size(); i++) {
        if (std::abs(current_screen.area()) < APPEAR_POI) {
            LatLon position = POI_Info[i].position;
            float x = x_from_lon(position.longitude());
            float y = y_from_lat(position.latitude());

            // draw POI rectangle

            ezgl::point2d start = ezgl::point2d({x - width / 2},
            {
                y - height / 2
            });
            ezgl::rectangle rect = ezgl::rectangle(start, width, height);
            g->set_color(75, 0, 130);
            g->draw_rectangle(start, width, height);
            g->set_color(230, 230, 250);
            g->fill_rectangle(rect);

        }

    }
    for (int i = 0; i < POI_Info.size(); i++) {
        ezgl::rectangle zoomed_in_screen = g->get_visible_world();
        if (std::abs(zoomed_in_screen.area()) < APPEAR_TEXT) {
            LatLon position = POI_Info[i].position;
            float x = x_from_lon(position.longitude());
            float y = y_from_lat(position.latitude());

            g->set_color(0, 0, 0);
            g->format_font(appropriate_font(), ezgl::font_slant::normal, ezgl::font_weight::normal, 12);
            g->draw_text(ezgl::point2d(x, y), POI_Info[i].name, width, height);
        }
    }

}

//draw fastest path with given vector that contains street segments IDs.
void draw_fastest_path(std::vector<StreetSegmentIdx> result, ezgl::renderer *g) {
    for (int i = 0; i < result.size(); i++) {

        LatLon from = Intersection_Info[Street_Segment_Info[result[i]].from].position;
        LatLon to = Intersection_Info[Street_Segment_Info[result[i]].to].position;

        //if street segment has curve points
        if (Street_Segment_Info[result[i]].curvePointCount != 0) {
            int size = Street_Segment_Info[result[i]].curvePointCount;
            std::vector<LatLon> curve = Street_Segment_Info[result[i]].curvePointPosition;
            //draw from street_segment.from to first curve point

            draw_street(x_from_lon(from.longitude()), y_from_lat(from.latitude()), x_from_lon(curve[0].longitude()), y_from_lat(curve[0].latitude()), "fastest_path", g);
            //draw curve from point to point
            for (int j = 0; j < Street_Segment_Info[result[i]].curvePointCount - 1; j++) {
                draw_street(x_from_lon(curve[j].longitude()), y_from_lat(curve[j].latitude()), x_from_lon(curve[j + 1].longitude()), y_from_lat(curve[j + 1].latitude()), "fastest_path", g);

            }
            //draw from last curve point to street_segment.to

            draw_street(x_from_lon(curve[size - 1].longitude()), y_from_lat(curve[size - 1].latitude()), x_from_lon(to.longitude()), y_from_lat(to.latitude()), "fastest_path", g);
        }
        else {

            draw_street(x_from_lon(from.longitude()), y_from_lat(from.latitude()), x_from_lon(to.longitude()), y_from_lat(to.latitude()), "fastest_path", g);

        }
    }
}
//helper function for displaying directions
void print_help_fastest_path(ezgl::rectangle rect, ezgl::renderer *g) {
    float width = (rect.top_right().x - rect.bottom_left().x) / 2;
    draw_text_box(rect, width, 4, g);
}

// print all direction of fastest path including total travel time
void print_fastest_path(std::vector<StreetSegmentIdx> result, int start, int end, double turn_penalty, ezgl::renderer *g) {
    ezgl::rectangle rect = g->get_visible_world();

    float width = (rect.top_right().x - rect.bottom_left().x) / 2;
    float error_range = width / 30;
    int count = 0;
    std::string street_name;
    std::stringstream ss;
    std::string temp;

    //draw box for printing path

    print_help_fastest_path(rect, g);

    ezgl::point2d a(rect.bottom_left().x + width / 4, rect.bottom_left().y);
    ezgl::point2d b(rect.top_right().x, rect.top_right().y - width / 6);

    rect = {a, b};
    //print start point intersection info
    g->set_font_size(14);
    g->set_color(ezgl::WHITE);
    g->draw_text({rect.bottom_left().x, rect.top_right().y - error_range}, "Start: " + Intersection_Info[start].name, width, width);

    g->set_font_size(14);
    g->set_color(ezgl::YELLOW);

    LatLon from_next, from_curr;
    LatLon to_next, to_curr;
    //draw first segment street direction and name
    for (int i = 0; i < result.size() - 1; i++) {
        // find out which way is FROM and TO in street segments. all segments direction is set up randomly
        from_next = Intersection_Info[Street_Segment_Info[result[i + 1]].from].position;
        to_next = Intersection_Info[Street_Segment_Info[result[i + 1]].to].position;
        from_curr = Intersection_Info[Street_Segment_Info[result[i]].from].position;
        to_curr = Intersection_Info[Street_Segment_Info[result[i]].to].position;

        if (from_next.longitude() != to_curr.longitude() && from_next.latitude() != to_curr.latitude() && to_next.longitude() != to_curr.longitude() && to_next.latitude() != to_curr.latitude()) {
            to_curr = Intersection_Info[Street_Segment_Info[result[i]].from].position;
            from_curr = Intersection_Info[Street_Segment_Info[result[i]].to].position;
        }
        if (to_curr.longitude() != from_next.longitude() && to_curr.latitude() != from_next.latitude()) {
            to_next = Intersection_Info[Street_Segment_Info[result[i + 1]].from].position;
            from_next = Intersection_Info[Street_Segment_Info[result[i + 1]].to].position;
        }

        //calculate degree of first street segments and next segments to find out which direction need to be turned.
        float degree_cur = get_degree(from_curr, to_curr);
        float degree_next = get_degree(from_next, to_next);
        float degree = degree_cur - degree_next;
        std::string direction = get_direction(degree_cur);
        const int threshold = 20;

        if (i == 0) {//in first street segments no turn direction is required so print which way need to head such as WEST, NORTH.
            street_name = Street_Info[Street_Segment_Info[result[i]].streetID].Name;
            temp = "1. Go " + direction + " on " + street_name;
            count = 2;

            g->draw_text({rect.bottom_left().x, rect.top_right().y - error_range * (count)}, temp, width, width);
            ss.str("");
        }

        if (street_name != Street_Info[Street_Segment_Info[result[i + 1]].streetID].Name) {
            street_name = Street_Info[Street_Segment_Info[result[i + 1]].streetID].Name;
            ss << count << ". ";
            ss >> temp; //find out turn direction from angles difference
            if ((degree > threshold && degree < 180 - threshold) || degree < -180 - threshold) {
                direction = "Turn right";
            } else if ((degree < 0 - threshold && degree > -180 + threshold) || degree > 180 + threshold) {
                direction = "Turn left";
            } else if (degree >= 0 - threshold && degree <= threshold) {
                direction = "Go straight";
            } else {
                direction = "U-turn";
            }
            temp = temp + direction + " on " + street_name;
            g->draw_text({rect.bottom_left().x, rect.top_right().y - error_range * (count + 1)}, temp, width, width);
            count++;
        }

        ss.str("");
    }
    //Print destination intersection info and total time at the end
    g->set_font_size(14);
    g->set_color(ezgl::WHITE);


    g->draw_text({rect.bottom_left().x, rect.top_right().y - error_range * (count + 2)}, "Finish: " + Intersection_Info[end].name, width, width);

    g->set_color(ezgl::YELLOW);
    g->draw_text({rect.bottom_left().x, rect.top_right().y - error_range * (count + 3)}, "________________________________", width, width);
    g->set_color(ezgl::WHITE);

    double total_time = computePathTravelTime(result, turn_penalty);
    std::string print_time = "Total travel time: ";


    int temp3 = ((int) total_time) / 60;
    std::string word = std::to_string(temp3);
    print_time = print_time + " " + word + " " + "min";

    ss.str(""); // clear stringstream

    int temp4 = ((int) total_time) % 60;

    std::string word2 = std::to_string(temp4);
    print_time = print_time + " " + word2 + " " + "sec";


    g->draw_text({rect.bottom_left().x, rect.top_right().y - error_range*(count + 4)}, print_time, width, width);

    //ezgl::rectangle rect2 = initial_world;
   // std::cout << rect2.area() << std::endl;

    //g->set_color(ezgl::RED); 
    /*g->draw_text({rect.bottom_left().x + error_range*(count + 0.5), rect.top_right().y - error_range*(count + 7)}, "West --", width, width);


    g->draw_text({rect.bottom_left().x + error_range*(count + 1.5), rect.top_right().y - error_range*(count + 7)}, "-- East", width, width);

    g->set_text_rotation(270);
    g->draw_text({rect.bottom_left().x + error_range*(count + 1), rect.top_right().y - error_range*(count + 6)}, "-- North", width, width);
    g->set_text_rotation(0);
    g->set_text_rotation(-90);
    g->draw_text({rect.bottom_left().x + error_range*(count + 1), rect.top_right().y - error_range*(count + 8)}, "South --", width, width);
    g->set_text_rotation(0);
*/
}

// helper function to get the direction of 2 streets 
std::string get_direction(double degree) {
    std::string direction;
    if (degree >= 350 && degree <= 10) {
        direction = "East";
    } else if (degree > 10 && degree < 80) {
        direction = "North-East";
    } else if (degree >= 80 && degree <= 100) {
        direction = "North";
    } else if (degree > 100 && degree < 170) {
        direction = "North-West";
    } else if (degree >= 170 && degree <= 190) {
        direction = "West";
    } else if (degree > 190 && degree < 260) {
        direction = "South-West";
    } else if (degree >= 260 && degree <= 280) {
        direction = "South";
    } else if (degree > 280 && degree < 350) {
        direction = "South-East";
    }
    return direction;
}

//if user type wrong input this box will show up for warning.
// if user click ok button it will disappear.
void draw_invalid_input(ezgl::renderer *g) {
    ezgl::rectangle current_screen = g->get_visible_world();

    if (invalid_input == true) {
        invalid_input_box(current_screen.center_x(), current_screen.center_y(), current_screen.width(), g);
    }
}

// this functions is called when the user types an invalid input 
void invalid_input_box(float x, float y, float width, ezgl::renderer *g) {

    ezgl::point2d temp;
    float error_range = width / 200;
    temp.x = x - (width / (5) + error_range * 2) / 2;
    temp.y = y - (width / 20 + error_range) / 2;
    ezgl::rectangle square(temp, width / (5) + error_range * 2, width / 20 + error_range);
    g->set_color(ezgl::PINK);
    g->fill_rectangle(square);
    temp.x = temp.x + error_range;
    temp.y = temp.y + error_range;
    square = ezgl::rectangle(temp, width / (5), width / 20 - error_range);
    g->set_color(ezgl::WHITE);
    g->fill_rectangle(square);

    temp.x = temp.x + (width / (5) + error_range) / 2;
    temp.y = temp.y + (width / 40) + error_range * 2;
    g->set_font_size(9);
    g->set_color(ezgl::BLACK);
    g->draw_text({temp.x, temp.y}, "Invalid Input is typed", width, width);
    g->draw_text({temp.x, temp.y - error_range * 2.5}, "Click 'Disable' button and try again", width, width);
}

//helper function to draw the rectangle that encompasses the printed directions 
void draw_text_box(ezgl::rectangle rect, float width, float length, ezgl::renderer *g) {
    float error_range = width / 25;

    ezgl::point2d a(rect.bottom_left().x + width / 2, rect.top_right().y - error_range * 4);
    ezgl::point2d b(rect.bottom_left().x, rect.bottom_left().y + width / (length) - error_range);
    rect = {b, a};
    g->set_color(ezgl::WHITE);
    g->fill_rectangle(rect);

    g->set_color(40, 40, 40);
    ezgl::point2d a1(rect.top_right().x - error_range / 3, rect.top_right().y - error_range / 3);
    ezgl::point2d b1(rect.bottom_left().x, rect.bottom_left().y + error_range / 3);
    rect = {b1, a1};
    g->fill_rectangle(rect);
}

// function to draw the path on the map
void draw_fastest_path_in_load_map(ezgl::renderer *g) {

    //if fastest_click_button is clicked draw fastest path with two clicked intersections
    if (two_intersections.size() == 2) {
        Found_Path = findPathBetweenIntersections(two_intersections[0], two_intersections[1], 15);
        draw_fastest_path(Found_Path, g);
        draw_street_name(g);
        print_fastest_path(Found_Path, two_intersections[0], two_intersections[1], 15, g);
    }

}

