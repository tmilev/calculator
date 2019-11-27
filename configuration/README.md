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
