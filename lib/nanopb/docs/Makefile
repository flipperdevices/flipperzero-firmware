INPUTS = index.md concepts.md reference.md security.md migration.md whats_new.md

all: $(INPUTS:.md=.html)

tmp_menu.html: $(INPUTS)
	echo '<div id="index">' > $@
	(echo '<h2>Documentation index</h2>'; \
	 for file in $^; do echo -n '1. ['; sed -n '1 s!^# Nanopb: !! p' $$file; \
	 echo -n "]("; echo $$file | sed 's/.md/.html)/' ; done;) | \
	 pandoc -f markdown -t html5 >> $@
	echo '</div>' >> $@

%.html: %.md tmp_menu.html
	sed '1 s!#!%!' $< | \
	pandoc -s -f markdown -t html5 -c lsr.css --toc --toc-depth=4 \
	 --variable 'header-includes=<link href="favicon.ico" type="image/x-icon" rel="shortcut icon" />' \
	 --indented-code-classes=c \
	 -o $@
	sed -i '/<nav/e cat feedback.html' $@
	sed -i 's/doc_page_name_placeholder/$</' $@
	sed -i 's!<nav[^>]*>!\0<b>Contents:</b>!' $@
	sed -i '/<nav/e cat tmp_menu.html' $@

