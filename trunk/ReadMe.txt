1 The files are: 

1.1. polyhedra.h
1.2. polyhedra.cpp: The big guns. All the algorithms are here. No external libraries used except for std!
1.3. example.cpp: An example file, in development.
1.4. wxWidgets_GUI.cpp: wxWidgets GUI. For notes how to install wxWidgets see point 2.
1.5. License_LGPL-3.0.txt: the license!
1.6. License_GPL3.0.txt: supplementary to License_LGPL-3.0.txt. Given for reference.

2 Installing wxWidgets.

2.1 wxWidgets for Windows. Warning: this may turn out to be a nightmare, as it did for me. 
2.1.1 Go to wxWidgets main site.
2.1.2 Download installer & install.
2.1.3 Download Visual C++ express latest version (Microsoft site) and install.
2.1.4 Go to the wxWidgets folder->build->msw. Open file wx.dsw. 
2.1.5 As asked by the compiler, covert files to newer format.
2.1.6 Go through each individual project and set to Multithreaded compile Debug. 
			To do that right click from within the Visual studio each of the individual projects, 
			select properties->Configurations->C/C++->code generation-> Laufzeitbibliotek 
			(Running library or something of the sort in english). 
			There, you select option /MTd = multithreaded debug.
2.1.7 Make sure you did this for all the projects.
2.1.8 Build all projects (build menu).
2.1.9 Close VS and open wxWidget folder->samples-> any sample you wish. 
			Open and build. If it is running, you are good to go. The battle is not yet won.
2.1.10	Set up properly all paths. This will require more detailed description. 
				Will write it up (I am not sure how to do that properly yet).

2.2 wxWidgets for Linux (Ubuntu).
2.2.1 Install Code::Blocks C++ compiler. It should come with everything needed. 
			Will write a more detailed desription in the future.
