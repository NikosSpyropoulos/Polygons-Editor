# Polygons-Editor

# Purpsose

The purpose of this project is to develop an editor that allows the user to create and modify polygons. The user will be able to "cut" polygons
by using the algorithm Hodgeman-Sutherland and modify polygons from 2 dimensions to 3 dimensions. (In the files there is a demoVideo that includes all 
the functionalities of the app). 
The application is programmed in C++, OpenGl

# User Manual

First of all , to open the main window the user should run in the terminal ( Linux os) the command “g++ polygEditor.cpp  -o a -lglut -lGLU -lGL” and after this to run the 
executable file "./a"

The applications contains one main Window (white color) . When the user right-clicks the mouse, a menu is displayed which contains the following three submenus:
- Action (This submenu includes 4 options which we will analyze below. Exit, POLYGON, CLIPPING, EXTRUDE. 
- LINE_COLOR (This submenu contains 16 options with different colors that the user can choose to color the outline of the polygon)
- FILL_COLR (this button consists a submenu with 16 colors that the user can choose to fill the polygon that he is  going to create)


### We will now analyze the steps the user needs to take to use the above options.

## POLYGON CREATION

The user by right-clicking, selecting the ACTION submenu and then the POLYGON option has the ability to create multiple convex or non-convex polygons.
By left-clicking(mouse),where the user wants, determines the vertices of the polygon. 
##### ATTENTION
if a vertex is positioned in such a way as to create a Self-intersecting polygon the polygon will not appear on the screen and the user will
have to recreate a polygon from scratch
Once the user "enters his last vertex" he must right-click to display the polygon he created (automatically by right-clicking(mouse) will
unite the last vertex with the first one).
##### COLORING
Before selecting the POLYGON option the user can select the colors he wants his polygon to have from the options LINE_COLOR (concerning the border color) and 
FILL_COLOR (concerning the color of the interior of the polygon). If no color is selected and the POLYGON option is selected immediately, the polygon 
that the user will create will have WHITE color inside and BLACK outside.

## CLIPPING

With this option the user can create an "invisible" rectangle that will "cut" all the pieces of polygons that are outside its boundaries.
To create this rectangle you have to left-click and select the upper left corner and the lower right corner of the rectangle. This will create an "invisible" rectangle that will cut the parts of the polygons that are outside its boundaries and in its place will create new polygons of smaller size.
The user can CLIPPING as many times as he wants

## EXTRUDE

With the EXTRUDE option the user can modify the polygon that exist in his window from 2d to 3d. Once the user selects this option, the message 
"Give the length of extrusion:" will appear in the terminal and he will have to enter a value for the length of the extrusion he wants to do in the polygonσ he created. 
Hence, the polygons on the screen will have changed shape. To see the shapes better from all their edges, it is enough to use the "arrows" on the keyboard with which he will be able to rotate the shapes and see them from all sides.

## Τriangulation

In addition to the "arrows" that the user uses to observe the 3d shape, he can use the ‘T’, since of course there is even a polygon in the window, to display the polygon triangular with the outline of each triangle to be erased in green. As soon as the user presses the ‘T’ again, the triangulation disappears and the shape returns to its original form. (the user can press the ‘T’ as many times as he wants, as long as the shapes are in 2d format)


### We will now describe some parts of the code.

## POLYGON CREATION

To create the polygon I have implemented a function named makepolygon(x, y) which has as arguments 2 coordinates. This function is called inside the mouse() function when the user left-clicks and takes as arguments the coordinates that the mouse has that exact moment.
Inside makePolygon() is also called the linesIntersect() function which passes 8 arguments (8 coordinates) and checks if the lines created by these coordinates intersect.

## Τriangulation

For the implementation of triangulation my source code was from FlipCode.com by John W. Ratcliff (jratcliff@verant.com) on July 22, 2000  

## CLIPPING

For clipping there is a clip_window() function which is called when the user selects CLIPPINGC option to create the "invisible" rectangle. The intesect_point_x() intesect_point_y() clipping() and hodg_suth_algorithm() functions are then used to cut the polygons. (source code from geekforgeeks.com used for this implemantation)

## EXTRUDE

As soon as the EXTRUDE option is selected on the display, a polygon is created with a different Z value from the previous one and its edges. To rotate the polygon I created the function windowSpecial() in which I increase 2 variables by pressing each "arrow" in order to rotate the polygon.
