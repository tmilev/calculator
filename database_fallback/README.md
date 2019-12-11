# Folder to contain fallback database

At the time of writing, this folder 
contains a .json file to serve as a database-fallback if
a mongoDB is not available. Please note that every read/(respectively, write) 
instruction to the fallback database is, 
at the time of writing, 
a file read/(respectively, read+write) operation combined 
with JSON parse.

