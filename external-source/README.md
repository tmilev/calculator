#Notes on external project installation


##MongoDB C driver

Instructions taken from: http://mongoc.org/libmongoc/1.1.5/installing.html

1. Install the mongo c driver.

1.1. Download the mongoc driver source, for example from:

https://github.com/mongodb/mongo-c-driver/releases

1.2. Extract the driver and navigate to the source code folder.

1.3. Configure mongo.

```./configure --prefix=/usr --libdir=/usr/lib --includedir=/usr/include --disable-automatic-init-and-cleanup ```

1.4. Build and install.

```
make
sudo make install
```

2. Install bson.

2.1. Download the bson library, for example from:

https://github.com/mongodb/libbson/releases/download/1.9.3/libbson-1.9.3.tar.gz

2.2. Extract the driver and navigate to the source folder. 

2.3. Configure.

```./configure --prefix=/usr```

2.4. Build and install.

```
make
sudo make install
```
