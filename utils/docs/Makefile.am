dist_man_MANS = gsch2pcb.1 grenum.1 gxyrs.1 olib.1 pads_backannotate.1 \
	pcb_backannotate.1 refdes_renum.1 schdiff.1 sw2asc.1 tragesym.1 \
	smash_megafile.1 convert_sym.1 garchive.1 gmk_sym.1 gschlas.1 \
	gsymfix.1 sarlacc_sym.1 sarlacc_schem.1

dist_noinst_MANS = gnet_hier_verilog.1

EXTRA_DIST = README.gsch2pcb README.olib README.tragesym README.grenum \
	TODO.grenum why.grenum logo.png utils-main.txt \
	what-is-geda.html what-is-utils.html utils.dox \
	gsch2pcb.1.in grenum.1.in gxyrs.1.in olib.1.in \
	pads_backannotate.1.in pcb_backannotate.1.in refdes_renum.1.in \
	schdiff.1.in sw2asc.1.in tragesym.1.in smash_megafile.1.in \
	convert_sym.1.in garchive.1.in gmk_sym.1.in gschlas.1.in \
	gsymfix.1.in sarlacc_sym.1.in sarlacc_schem.1.in \
	gnet_hier_verilog.1.in

docsreadmedir = $(docdir)/readmes
docsreadme_DATA = README.gsch2pcb README.olib README.tragesym README.grenum

doxygen: utils.dox
	@echo 'Creating doxygen documentation for utils...'
if ENABLE_DOXYGEN
	$(DOXYGEN) utils.dox
else
	@echo 'configure was not run with the --enable-doxygen switch.'
	@echo 'The documentation cannot be built from the sources.'
endif

.1.in.1:
	d=`LANG=C date -d $(DATE_VERSION) "+%B %-d, %Y"` ; \
	v="$(DOTTED_VERSION).$(DATE_VERSION)" ; \
	sed -e"s:[@]DATE@:$$d:" -e"s:[@]VERSION@:$$v:" < $< > $@

# Rules for converting man->html
html_man_files = $(dist_man_MANS:%.1=%.html)
SUFFIXES = .1.in .1 .html

docsmandir=$(docdir)/man
dist_docsman_DATA=$(html_man_files)
if ENABLE_GROFF_HTML
.1.html:
	rm -rf $@.tmp && \
	LC_NUMERIC=C $(GROFF) -man -T html  $< >$@.tmp && \
	mv -f $@.tmp $@
else
.1.html:
	@echo "No working groff available: cannot create $@"
	@! :
endif

MOSTLYCLEANFILES = *.log *~
CLEANFILES = *.log *~ 
DISTCLEANFILES = *.log core FILE *~
MAINTAINERCLEANFILES = *.log *~ Makefile.in $(html_man_files)

distclean-local:
	-rm -rf html latex

maintainer-clean-local:
	-rm -rf html latex 
