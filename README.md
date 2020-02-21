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
git clone https://github.com/tmilev/calculator.git

```
Please note that **the files you create with the calculator will be placed in baseFolder**, i.e., inside the first folder you created and where you checked out the calculator trunk, but outside of that calculator trunk. In this way, the files you create via the calculator are kept outside of the calculator folder, and so, you can put **baseFolder** in a repository of your own. 

3. Compile the calculator.

3.1. Default build.

- This build assumes you have openssl and mongoDB installed on your system. 
- However, if you don't have these installed, the build should auto-detect this, turn off mongoDB and openSSL and not fail. If, contrary to promised, the build does fail, please file a bug.

```
cd calculator
make -j10
```
The -j10 flag tells the compiler to use 10 parallel compilation jobs. Adjust the number 10 to whatever is appropriate for your system (-j10 should work on a 4G RAM laptop).

3.2. To explicitly request a build without mongoDB and openSSL, use the following.
```
make -j10 nossl=1 nomongo=1
```

### Running the calculator
5. Start the calculator as a web server:
```
calculator server 10000
```
The number 10000 gives you computation timeout (computations that take longer than 10000 seconds will crash).
Leave the terminal window open if you'd like to monitor what's going on. 
The calculator will try to bind on ports 8155 (http) and 8166 (https). 

6. Open your favorite browser and navigate to 

http://localhost:8155

This completes the setup and your first run.

### Prerequisites
1. **OS**. For now, we support **Linux only**. Windows and Mac OS support could be added in the future (depending on resources). We have explicit support for the following distros.
- Ubuntu   (all systems using the apt package manager should work too, but that hasn't been tested).
- I no longer support CentOS as I no longer have access to a CentOS system. However, I did run calculator-algebra.org on CentOS for a while, and, last I tested, the system worked on CentOS out-of-the-box.
- We will add your Linux flavor upon request. We may need your help with your package manager. 
2. **git**. needed to download the source. 
3. **unzip**. Used to unzip [Mathjax](https://www.mathjax.org/), a method to display math formulas in the web browser. Without this, math formulas will not display correctly.
4. [Optional] **mongoDB development libraries**. Without [mongoDB](https://www.mongodb.com/), the calculator will not be able to provide any login services, and some "administator-only" functions may be missing.
5. [Optional] **openssl development libraries**. Without [openSSL](https://www.openssl.org/), the calculator will not be able to provide any login services, and some "administator-only" functions may be missing.

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
The calculator login system uses google's recapcha. 
To find out more about reCAPTCHA, google `google recaptcha`.

The system should run fine without reCAPTCHA setup, however,
the `forgot login` button will not work.


You can get your recaptcha client secret and public info from:

```
https://www.google.com/recaptcha/admin#site/
```

Put the secret in a file (copy+paste the secret contents into the file):

```
certificates/recaptcha-secret.txt
```
and the public info in (copy+paste contents):

```
certificates/recaptcha-public.txt
```

## Authors
The following people have contributed code directly.
- **Todor Milev** (project mastermind)
- **Thomas-Folz Donahue**
