#

SUBDIRS = gentypes los libltest test

pkgconfig_DATA = los.pc

vgsupp_DATA = glib.supp los.supp

.PHONY: listdirs
listdirs:
	@printf '====  INSTALLATION DIRECTORIES  ====\n'
	@printf '%-16s : %-20s\n' 'prefix' "$(prefix)"
	@printf '%-16s : %-20s\n' 'exec_prefix' "$(exec_prefix)"
	@printf '%-16s : %-20s\n' 'bindir' "$(bindir)"
	@printf '%-16s : %-20s\n' 'libdir' "$(libdir)"
	@printf '%-16s : %-20s\n' 'pkglibdir' "$(pkglibdir)"
	@printf '%-16s : %-20s\n' 'vgsuppdir' "$(vgsuppdir)"
	@printf '%-16s : %-20s\n' 'datadir' "$(datadir)"
	@printf '%-16s : %-20s\n' 'pkgdatadir' "$(pkgdatadir)"
	@printf '====================================\n'
