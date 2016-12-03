[![CG22016_DevSession](http://i.imgur.com/WGjqrAz.jpg)](http://cg2.chaosgroup.com/conf2016)
# CG2 2016 - Getting your hands dirty with GPUs
___
### CUDA Demo
This repository holds the full code for the DEMO showed at CG2 2016 - Getting your hands dirty with GPUs
If you did not attend the conference, I suggest you check out the recorded videos. Just click the banner above.

___
# WARNING!
#### This demo is platform dependent and is designed to work on Windows
It should be relatively easy to make it run on other OSes as well, but I just didn't have the time to port it.
If you would like to contribute, be my guest.
___
### Requirements

Before this demo can be run, there are a few prereqisites:
 - MS Windows
 - NVidia GPU
 - [wxWidgets]
 - [CUDA Toolkit]
 - [CMake] v3.7 or later
 - Visual Studio 2010 or later

Click on the links provided and download and install the latest versions.
`NOTE` After setting up wxWidgets, make sure there is a WXWIN env var set with the location where wxWidgets is installed.
 
---
### Building a Solution

We are ready to create our build solution.
Go to the location where you have downloaded the sources and run **build.bat**
It will create a folder named ***CG2demo_Build*** in the same directory and if all goes well create a solution.

If everything was set up properly and no errors occur, inside the ***CG2demo_Build*** folder you should find **cg2_Demo.sln**
Open it with Visual Studio and hit `F7`.

`NOTE` CMake sets the default startup project to ALL_BUILD. If you want to run your program from Visual Studio, set the default project to *cg2_demo*.
`NOTE` It is possible that your solution does not compile on the first try and complain for not being able to find **demoFilters.h** . In this case just hit F7 again.
# Enjoy!

You can send your questions to a7az0th@gmail.com

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)
   [wxwidgets]: <http://www.wxwidgets.org/>
   [cuda toolkit]: <https://developer.nvidia.com/cuda-downloads>
   [cmake]: <https://cmake.org/>