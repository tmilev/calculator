<VirtualHost *:80>
	ServerAdmin webmaster@localhost

	ScriptAlias /vpf/cgi-bin/ /home/todor/math/vectorpartition/trunk/Debug/
	Alias /vpf/ /home/todor/math/vectorpartition/trunk/
	<Directory /home/todor/math/vectorpartition/trunk/output/>
		Options Indexes FollowSymLinks MultiViews
		AllowOverride None
		Order allow,deny
		allow from all
	</Directory>
	<Directory /home/todor/math/vectorpartition/trunk/Debug/>
		AllowOverride None
		Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
		Order allow,deny
		Allow from all
	</Directory>

	ErrorLog ${APACHE_LOG_DIR}/error.log

	# Possible values include: debug, info, notice, warn, error, crit,
	# alert, emerg.
	LogLevel warn

	CustomLog ${APACHE_LOG_DIR}/access.log combined

</VirtualHost>
