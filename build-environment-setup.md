### **The file contains a tutorial how to build and configure "Argos"**

**To build "Argos", the following tools are needed:**  

* Git
* Qt
* CMake
* *Build Tools for Visual Studio (Only for Windows)*

**Installing required tools for Windows**  
  
1\. (Git) Open the following link, choose your platform (x86 or x64) and download Git:   
   https://git-scm.com/download/win
        
2\. (Qt, CMake) Follow the next link and download Qt:   
   https://www.qt.io/download    
   Next steps: Downloads for open source users -> Go open source -> Download the Qt Online Installer -> Download     
   Open Qt Online Installer and choose Qt version (Qt6 or later)  
   CMake will be upload with Qt (CMake is contained in "Qt Developers and Designer Tools")
   
3\. *(Only for Windows - Build Tools for Visual Studio, MSVC) Open the next link:   
    https://visualstudio.microsoft.com/downloads/   
    Next steps: Tools for Visual Studio -> Build Tools for Visual Studio -> Download    
    Open Visual Studio Installer and choose the menu item "Desktop development with C++"*  

**Installing required tools for Linux**  

1\. (Git) For ubuntu or other debian-based distributions, you should enter following command in terminal:    
   # apt install git    
   For arch-based distributions, such as manjaro:    
   # pacman -S git    
   (Please, note, that '#' means to run a command with super user permissions, you may do it with adding sudo before command.)   
        
2\. (Qt, CMake) To install Qt you may follow official instructions:
    https://doc.qt.io/qt-6/get-and-install-qt.html    
    To check if cmake is installed you may enter:    
    $ cmake --version    
    If command will be not found, you should install it with following command:    
    # apt install cmake (for debian-based) or:    
    # pacman -S cmake (for arch-based)    

**Getting Sources**   

1\. You can get the latest stable source packages from the following page:    
    https://bitbucket.org/vpogorilyi/argos.git    
    
2\. Run Git and enter the command:  
    "git clone https://bitbucket.org/vpogorilyi/argos.git"

**Launching**

1\. Run QtCreator -> Open project -> Find a folder "Argos" and choose CMakeLists.txt -> Configure Project -> Run Project    
*Note for Windows users: You will need to configure your project with MSVC*