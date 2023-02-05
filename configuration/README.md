# Configration file for the calculator

This folder is to contain the file configuration.json.
If deleted, the file is auto-generated with reasonable defaults.

The file is overwritten every when the settings change. 
More precisely, here's the configuration's workflow.

- The configuration file is loaded.
- All missing entries are populated with reasonable defaults.
- The configuration file's contents are recomputed. Values may be sanitized or corrected. 
- If the configuration content does not perfectly match the configuration file, 
the configuration file is overwritten.


## Configuration entries

### Ports

To configure the ports, the  `"portHTTP":"8155"` and `"portHTTPSOpenSSL":"8166"` entries.
The `portHTTPSOpenSSL` is your primary port and is used through the standard openSSL library. 
All http traffic (portHTTP) will be forwarded to https traffic (portHTTPSOpenSSL), unless 
we had issues configuring openSSL.
Entry `"portHTTPSBuiltIn":"8177"` is reserved: we have a TODO task to write our own implementation of the ssl protocol. 
This is a large task and we have some progress on it, which is what port portHTTPSBuiltIn is for.
However, our implementation does not yet work end-to-end. 
Please, don't hold your breath: don't expect a full implementation any time soon.

### Stand alone webserver

You can make the calculator act as a stand-alone web server. In this mode, the calculator will not run any computer algebra functions. 

To do so, expand the actAsWebServerForTheseHosts entry in your configuration:

```
actAsWebServerForTheseHosts: {
  "127.0.0.1": {
    "portHTTP":"",
    "portHTTPS":"",
    "baseFolder":"public_html/",
    "landingPage":""
  }
}
```

Here, "127.0.0.1" is the hostname. All connections that report having that hostname will be served as a stand-alone webserver.
All computer algebra system functions will be off for these hostnames.
The other configuration entries should have self-explanatory names. 
We are currently using this API and are learning what is needed to separate two websites while sharing the executable and server. 
We will add more documentation and configuration settings as they are needed.




