#ifndef HEADER_DSO_CONF_H
# define HEADER_DSO_CONF_H
/*{-  # The DSO code currently always implements all functions so that no
    # applications will have to worry about that from a compilation point
    # of view. However, the "method"s may return zero unless that platform
    # has support compiled in for them. Currently each method is enabled
    # by a define "DSO_<name>" ... we translate the "dso_scheme" config
    # string entry into using the following logic;
    my $scheme = uc $target{dso_scheme};
    if (!$scheme) {
        $scheme = "NONE";
    }
    my @macros = ( "DSO_$scheme" );
    if ($scheme eq 'DLFCN') {
        @macros = ( "DSO_DLFCN", "HAVE_DLFCN_H" );
    } elsif ($scheme eq "DLFCN_NO_H") {
        @macros = ( "DSO_DLFCN" );
    }
    join("\n", map { "# define $_" } @macros); -}*/
# define DSO_EXTENSION
#endif