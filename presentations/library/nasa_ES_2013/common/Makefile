vectorsources := $(shell find rawfigs/ ./common/rawfigs/ -name '*.dia' -o -name '*.eps' -o -name '*.m' -o -name '*.ps' -o -name '*.pdf' -o -name '*.svg' 2>/dev/null)
rastersources := $(shell find rawfigs/ ./common/rawfigs/ -name '*.gif' 2>/dev/null)
readysources := $(shell find rawfigs/ ./common/rawfigs/ -name '*.jpg' -o -name '*.png' -o -name '*.pdf' 2>/dev/null)
texsource := $(wildcard talk.tex paper.tex poster.tex proposal.tex)

vectorfigs := $(shell echo ' ' $(vectorsources) ' ' | sed -e 's> \(./common/\)*raw> >g' -e 's/\.[^. ]* /.pdf /g')
rasterfigs := $(shell echo ' ' $(rastersources) ' ' | sed -e 's> \(./common/\)*raw> >g' -e 's/\.[^. ]* /.png /g')
readyfigs := $(shell echo ' ' $(readysources) ' ' | sed -e 's> \(./common/\)*raw> >g')
figures := $(vectorfigs) $(rasterfigs) $(readyfigs)
texroot := $(patsubst %.tex, %, $(texsource))
#texfinal := $(texroot).pdf
dirname  := $(shell pwd)
texfinal := $(shell basename $(dirname)).pdf

bibfiles  := $(wildcard *.bib)
styfiles  := $(wildcard *.sty ./common/*.sty)
clsfiles  := $(wildcard *.cls ./common/*.cls)
codefiles := $(shell find snippets/ -name '*.cxx' -o -name '*.out')

PDFLATEX = TEXINPUTS=./common:$$TEXINPUTS pdflatex -halt-on-error
LATEX = TEXINPUTS=./common:$$TEXINPUTS latex -halt-on-error

.PHONY: all figures clean cleanlatex cleanfigs

all: $(texfinal)

figures: $(figures)

$(texfinal): *.tex $(bibfiles) $(figures) $(clsfiles) $(styfiles) $(codefiles)
	if (test -f *.bib); then $(PDFLATEX) $(texroot) && bibtex $(texroot); fi
	$(PDFLATEX) $(texroot) && $(PDFLATEX) $(texroot) && $(PDFLATEX) $(texroot)
	if [ $(texroot).pdf != "$(texfinal)" ]; then cp $(texroot).pdf "$(texfinal)" && rm -f $(texroot).pdf; fi

clean: cleanlatex cleanfigs

cleanlatex:
	rm -f $(patsubst %.tex, %.aux, $(wildcard *.tex)) 
	rm -f $(texroot).bbl $(texroot).blg $(texroot).log $(texroot).dvi $(texroot).nav $(texroot).out $(texroot).pdf $(texroot).snm $(texroot).toc $(texroot).vrb 

cleanfigs:
	rm -f $(figures)

figs/%.pdf: rawfigs/%.dia
	@mkdir -p $(dir $@)
	dia -t cairo-eps -e $?_roytemp.eps $? && epstopdf $?_roytemp.eps -o=$@
	@rm -f $?_roytemp.eps

figs/%.pdf: rawfigs/%.eps
	@mkdir -p $(dir $@)
	epstopdf $? -o=$@

figs/%.pdf: rawfigs/%.m
	@mkdir -p $(dir $@)
	@cd $(dir $?) && octave $(notdir $?)
	@epstopdf $?.eps -o=$@ && rm -f $?.eps

figs/%.pdf: rawfigs/%.pdf
	@mkdir -p $(dir $@)
	@reldir=`echo $(dir $@) | sed -e 's>[^/]*/*>../>g'`; ln -sf $${reldir}$? $@

figs/%.pdf: rawfigs/%.ps
	@mkdir -p $(dir $@)
	ps2pdf $? $@

#figs/%.pdf: rawfigs/%.png
#       convert $? $@

figs/%.pdf: rawfigs/%.svg
	@mkdir -p $(dir $@)
	inkscape $? -z --export-pdf=$@

#figs/%.png: rawfigs/%.jpg
#	@mkdir -p $(dir $@)
#	convert $? $@
figs/%.jpg: rawfigs/%.jpg
	@mkdir -p $(dir $@)
	@reldir=`echo $(dir $@) | sed -e 's>[^/]*/*>../>g'`; ln -sf $${reldir}$? $@

figs/%.png: ./common/rawfigs/%.jpg
	@mkdir -p $(dir $@)
	convert $? $@

figs/%.png: rawfigs/%.png
	@mkdir -p $(dir $@)
	@reldir=`echo $(dir $@) | sed -e 's>[^/]*/*>../>g'`; ln -sf $${reldir}$? $@

figs/%.png: ./common/rawfigs/%.png
	@mkdir -p $(dir $@)
	@reldir=`echo $(dir $@) | sed -e 's>[^/]*/*>../>g'`; ln -sf $${reldir}$? $@

figs/%.pdf: ./common/rawfigs/%.pdf
	@mkdir -p $(dir $@)
	@reldir=`echo $(dir $@) | sed -e 's>[^/]*/*>../>g'`; ln -sf $${reldir}$? $@

figs/%.pdf: ./common/rawfigs/%.eps
	@mkdir -p $(dir $@)
	epstopdf $? -o=$@                

figs/%.pdf: ./common/rawfigs/%.ps
	@mkdir -p $(dir $@)
	ps2pdf $? $@

figs/%.pdf: ./common/rawfigs/%.svg
	@mkdir -p $(dir $@)
	inkscape $? -z --export-pdf=$@
