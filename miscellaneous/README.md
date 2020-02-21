# Miscellaneous

The files in this folder are not maintained and may not work as expected.

1. File default_apache_conf shows a sample configuration for using the calculator behind an apache server.
2. File calculator.service shows a sample service file for auto-start of the calculator. 
3. File sourceforge_index.php contains an old calculator title page. 



### Mailgun setup
At the time of writing, the calculator sends email through the services of 
[mailgun](https://www.mailgun.com/).

The system should run fine without any mailgun setup, however, 
the `signup` and `forgot login` buttons will not work.


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
