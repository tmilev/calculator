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
4. Run the calculator for the first time. This may prompt you for further setup (mongoDB, openssl, and initial setup). 
```
cd bin
./calculator
```
Follow the prompts. If asked for sudo password, that means the calculator is installing libraries, most likely mongoDB, curl, or another reasonable dependency. 
Bravely enter your sudo password, it shouldn't break anything on your system.
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
2. **git**. needed to download the source. 
3. **sudo**. The initial calculator setup uses the sudo command, make sure that is installed on your system (should be there out of the box on most machines). 
4. **unzip**. If not already present, the calculator will attempt to install that on first run.
5. [Optional] **mongoDB development libraries**. If not already present, the calculator will attempt to install them on first run. 
Without mongoDB, the calculator will not be able to provide any login services, and some "admin-only" functions may be missing.
6. [Optional] **openssl development libraries**. If not already present, the calculator will attempt to install them on first run.
Without openSSL, the calculator will not be able to provide any login services.

### Port redirect with ip tables

To redirect port 80 to port 8155:
```
sudo iptables -A INPUT -i eth0 -p tcp --dport 80 -j ACCEPT
sudo iptables -A INPUT -i eth0 -p tcp --dport 8155 -j ACCEPT
sudo iptables -A PREROUTING -t nat -i eth0 -p tcp --dport 80 -j REDIRECT --to-port 8155
```
Similarly for redirecting port 443 to 8166:
```
sudo iptables -A INPUT -i eth0 -p tcp --dport 443 -j ACCEPT
sudo iptables -A INPUT -i eth0 -p tcp --dport 8166 -j ACCEPT
sudo iptables -A PREROUTING -t nat -i eth0 -p tcp --dport 443 -j REDIRECT --to-port 8166
```
Finally, save the IP table rules. For ubuntu:

```
sudo apt-get install iptables-persistent
```
The package above should automagically save your ip table rules. If not, run
```
su
iptables-save > /etc/iptables/rules.v4
exit
```

### Recaptcha secret. 
Get your recaptcha client secret and public info from:

```
https://www.google.com/recaptcha/admin#site/
```

Put the secret in a file:

```
certificates/recaptcha-secret.txt
```
and the public info in:

```
certificates/recaptcha-public.txt
```

To do that just copy and paste the secrets in plain text into the files.

### Mailgun setup

Put your mailgun secret in file:

```
certificates/mailgun-api.txt
```

Put the domain name from which you want emails sent in:

```
certificates/mailgun-hostname.txt
```
in format such as:

localhost

or:

calculator-algebra.org

## Authors
The following people have contributed code directly.
- **Todor Milev** (project mastermind)
- **Thomas-Folz Donahue**
