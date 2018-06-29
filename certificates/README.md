# Folder for all site secrets


## SSL certificates
To generate a certificate signing request, do the following. 

openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout calculator-algebra.key


## Certbot commands for getting a certificate
The assumed base folder is the calculator server base. 

```
cd certificates
sudo ./path/to/certbot-auto certonly --standalone -d calculator-algebra.org -d www.calculator-algebra.org
```
## Recaptcha secret. 
Put the secret in the file: recaptcha-secret.txt

## Mailgun secret
Put the secret in mailgun-api.txt

Put the mailgun hostname in the file mailgun-hostname.txt. 
The mailgun hostname should contain your website, for example

calculator-algebra.org
