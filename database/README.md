# Folder that holds all local databases.

This is work in progress. Every database is in a separate directory. The test/ subdirectory is used for unit tests. The database implementation is work in progress. I plan on making it a concurrent read/write database, in which each user data is recorded in a separate json file. Reads/writes for different users can be done in parallel. A lock for individual user data will be available, to avoid races between read+write pairs. This is all work in progress and not implemented yet.
