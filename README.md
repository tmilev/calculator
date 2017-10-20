# calculator

An online computer algebra system and homework server. 
Our mission is to construct a free and open source computer 
algebra system that serves the computational needs of 
both researchers and teachers.
We aspire to get our system to be as powerful and useful as Mathematica and SAGE.

## calculator installation 
At the moment, you can only install the calculator from source. Here's what you need to do get started.
1. Create a folder where you want the calculator project to be.
```
mkdir baseFolder
```
2. Download the source code **inside** that folder. 
```
cd baseFolder
svn checkout https://github.com/tmilev/calculator.git/trunk

```
Please note that **the files you create with the calculator will be placed in baseFolder**, i.e., inside the first folder you created and where you checked out the calculator trunk, but outside of that calculator trunk. In this way, the files you create via the calculator are kept outside of the calculator folder, and so, you can put **baseFolder** in a repository of your own. 

3. Compile the calculator.
```
cd trunk
make
```
4. Run the calculator for the first time. This may prompt you for further setup (mysql, openssl, and initial setup).
```
cd bin
./calculator
```
Follow the prompts - you **will be asked for your sudo password**.
### Running the calculator
5. Once the installation steps are complete start the calculator as a web server:
```
./calculator server 10000
```
Leave the terminal window open if you'd like to monitor what's going on. The calculator will tell you on which port it got bound. The defaults are 8155 (http) and 8166 (https). 

6. Open your favorite browser and navigate to 

http://localhost:8155/cgi-bin/calculator

This completes the setup and your first run.

### Prerequisites
1. **OS**. For now, we support **Linux only**. Windows and Mac OS support could be added in the future (depending on resources). We have explicit support for the following distros.
- Ubuntu   (all systems using the apt package manager should work too, but that hasn't been tested).
- CentOS   (all systems using the yum package manager should work too, but that hasn't been tested).
- We will add your Linux flavor upon request. We may need your help with your package manager. 
2. **subversion/git**. needed to download the source. 
3. **sudo**. The initial calculator setup uses the sudo command, make sure that is installed on your system (should be there out of the box on most machines). 
4. **unzip**. If not already present, the calculator will attempt to install that on first run.
5. **mysql development libraries**. If not already present, the calculator will attempt to install them on first run.
6. [Optional] **openssl development libraries**. If not already present, the calculator will attempt to install them on first run.

## Authors
The following people have contributed code directly.
- **Todor Milev** (project mastermind)
- **Thomas-Folz Donahue**

## License
This project is licensed under the LGPL license - see the license file in the source code.
