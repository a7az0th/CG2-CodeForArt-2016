CG2 DEMO

A simple demo project designed for the purpose of CG2 Code for Art 2016 - Dev Session

!!!!! THIS DEMO PROJECT IS PLATFORM DEPENDENT AND ONLY WORKS ON WINDOWS !!!!!
!!!!! If you do not have a NVidia GPU you will not be able to fully     !!!!!
!!!!! utilize this demo as its purpose is to familiarize the user with  !!!!!
!!!!! the CUDA API                                                      !!!!!


<<<<<<<< How to set it up on your Windows machine? >>>>>>>>

First and foremost, download the sources (if you already haven't)
Download, install and if necessary, build wxWidgets from http://www.wxwidgets.org/downloads/
Download and install CUDA Runtime 
Download and install CMAKE (version 3.7 required)

The environment variable WXWIN must be defined and set to the location of the wxWidgets installation folder.
Check whether the env var is present and if not, set it accordingly.

We are ready to create our build solution.
Go to the location where you have downloaded your sources and run build.bat
It will create a folder named CG2demo_Build in the same directory and if all goes well create a solution for the project

If no errors occur, inside the CG2demo_Build folder you should find cg2_Demo.sln
Open it with visual studio and compile.

NOTE: CMake sets the default startup project to ALL_BUILD. If you want to run your program from visual studio,
set the default project to cg2_demo.

Enjoy!