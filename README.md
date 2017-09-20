# calculator

An online computer algebra system and homework server. 
Our mission is to construct a computer algebra system as powerful and useful as Mathematica and SAGE.

## Calculator installation 
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
Please note that **the files you create with the calculator will be placed in baseFolder**, i.e., inside the first folder you created and where you checked out the calculator trunk. You can put **baseFolder** in a repository of your own. 

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
You do not close the terminal window, leave it open to see what's going on. The calculator will tell you on which port it got bound. The defaults are 8155 (http) and 8166 (https). 
6. Open your favorite browser and navigate to http://localhost:8155

This completes the setup and your first run.

### Prerequisites
1. **OS**. For now, we support Linux only. Windows and Mac OS support could be added in the future (depending on resources). We have explicit support for:
- Ubuntu   (should work on all systems using the apt-get package manager);
- CentOS   (should work on all systems using the yum package manager).
- We will add your Linux flavor upon request. We may need your help with your package manager. 
2. **subversion/git**. needed to download the source. 
3. **sudo**. The initial calculator setup uses the sudo command, make sure that is installed on your system (should be there out of the box on most machines). 
4. [Optional] **mysql development libraries**. If you don't have these, the calculator will attempt to install them on first run.
5. [Optional] **openssl development libraries**. If you don't have these, the calculator will attempt to install them on first run.

## Authors
The following people have contributed code directly:
- **Todor Milev** (project mastermind)
- **Thomas-Folz Donahue**

## License
This project is licensed under the LGPL license (see the license file in the source code.
