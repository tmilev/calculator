# calculator


An online computer algebra system and homework server. 
Our mission is to construct a free and open source computer 
algebra system that serves the computational needs of 
both researchers and teachers.
We aspire to get our system to be as powerful and useful as Mathematica and SAGE.

## Programming the calculator
The calculator runs on a math expression substitution and interpretation engine capable of expressing arbitrarily complex mathematical constructs.

More details on the calculator substitution engine can be found [here](wiki/README.md).

A list of all calculator handlers can be found [here](examples/README.md).

## Check out our math equation editor

More details [here](calculator_html/equation_editor/README.md).

Demo: https://calculator-algebra.org:8166/calculator_html/equation_editor/test_equation_editor.html


## calculator installation 
1. Download the `courses_calculator` repository:
```
git clone --recurse-submodules -j8 https://github.com/tmilev/courses_calculator.git
```
The repository `courses_calculator` contains the present repository as a sub-module, 
as well as the educational materials in the `freecalc` project

https://github.com/tmilev/freecalc.git


2. Compile the calculator.

2.1. Default build.

```
cd courses_calculator/calculator
make -j10
```
The -j10 flag tells the compiler to use 10 parallel compilation jobs. Adjust the number 10 to whatever is appropriate for your system (-j10 should work on a 4Gb RAM laptop).


The calculator-algebra.org site runs, at the time of writing, with 
```
make -j10 optimize=1
```
which turns on level -02 compiler optimizations. This speeds up computations - measured to be up to 40% when computing 2019! - at the cost of tripling build times and occasionally generating obscure linker errors.

### Run the calculator
Start the calculator as a web server:
```
calculator server
```
The terminal window will shows web server traffic log, 
and includes a link to the web browser interface.

http://localhost:8155
 
The default ports are 8155 (http) and 8166 (https). 
There is also a default computation timeout
(computations that take longer than a default amount of seconds will crash).

These can be overridden of course:

```
./calculator server 10000 portHTTPSOpenSSL 12345 portHTTP 12346
```

runs the server on https port `12345`, http port `12346` with a timeout of `10000` seconds. 

While running for first time, you may be prompted to fill in an openSSL certificate to be used when running locally; 
the data of course stays on your computer (in folder `calculator/certificates`).

### Run the calculator in development mode.

Run 
```
./calculator daemon
```
will start the calculator in daemon mode. 
The server will restart when you rebuild the executable successfully.


### Run the calculator tests.

Run
```
./calculator test calculator
```

to test all calculator functions (some experimental ones are excluded).

Run 
```
./calculator test
```
to run the full test suite that is ran before deploying calculator-algebra.org. This will test database setup, presence of .pdf files that are part of the frontend homework files, etc. This will also check that your executable compiles cleanly without ssl and database support.

### Prerequisites
1. **OS**. For now, we support **Linux only**. Windows and Mac OS support could be added in the future (depending on resources). 
- Ubuntu (all systems using the apt package manager should work too, but that hasn't been tested). The site
calculator-algebra.org runs Ubuntu and compiles out of the box. We expect all versions after 14.04 to work out-of-the-box. 
- calculator-algebra.org was ran on CentOS 6 for a while and worked out-of-the-box.
- The calculator was successfully built and ran on a Windows 10 Linux subsystem. This was a single run on localhost.
- We will add your Linux flavor upon request. We may need your help with your package manager. 
2. **git**. needed to download the source. 
3. [Optional] **openssl development libraries**. Without [openSSL](https://www.openssl.org/), the calculator will not be able to provide any login services, and some "administator-only" functions may be missing.

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
