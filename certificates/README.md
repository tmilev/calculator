# Folder for all site secrets


## SSL certificates
To generate a certificate signing request, do the following. 

openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout calculator-algebra.key


## Recaptcha secret. 
Put the secret in the file: recaptcha-secret.txt

## Mailgun secret
Put the secret in mailgun-api.txt

Put the mailgun hostname in the file mailgun-hostname.txt. 
The mailgun hostname should contain your website, for example

calculator-algebra.org
