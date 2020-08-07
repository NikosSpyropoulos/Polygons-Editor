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

The user by right-clicking, selecting the ACTION submenu and then the POLYGON option has the ability to create a polygon. By left-clicking(mouse)
,where the user wants, determines the vertices of the polygon. 
##### ATTENTION
if a vertex is positioned in such a way as to create a Self-intersecting polygon the polygon will not appear on the screen and the user will
have to recreate a polygon from scratch
Once the user "enters his last vertex" he must right-click to display the polygon he created (automatically by right-clicking(mouse) will
unite the last vertex with the first one).
##### COLORING
Before selecting the POLYGON option the user can select the colors he wants his polygon to have from the options LINE_COLOR (concerning the border color) and 
FILL_COLOR (concerning the color of the interior of the polygon). If no color is selected and the POLYGON option is selected immediately, the polygon 
that the user will create will have WHITE color inside and BLACK outside.

The user can create as many polygons as he wants, curved and non-curved, with whatever colors he wants (following of course the above instructions)
