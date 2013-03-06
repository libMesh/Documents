diasources := $(wildcard diafigs/*.dia)
pngsources := $(wildcard pngfigs/*.png)
epssources := $(wildcard epsfigs/*.eps)
pdfsources := $(wildcard pdffigs/*.pdf)


diafigures := $(patsubst diafigs/%.dia, figs/%.pdf, $(diasources))
pngfigures := $(patsubst pngfigs/%.png, figs/%.png, $(pngsources))
epsfigures := $(patsubst epsfigs/%.eps, figs/%.pdf, $(epssources))
pdffigures := $(patsubst pdffigs/%.pdf, figs/%.pdf, $(pdfsources))
johnfigures := $(wildcard figures/*)
figures := $(diafigures) $(pngfigures) $(epsfigures) $(pdffigures)

all: main.pdf

main.pdf: *.tex $(figures)
	pdflatex main && pdflatex main || (rm -f main.pdf; false)

clean: cleanslides cleanfigs

clobber: clean
	rm -f main.pdf

cleanslides:
	rm -f main.{aux,bbl,blg,log,dvi,nav,out,snm,toc,vrb}
	rm -f *.aux

cleanfigs:
	rm -f figs/*.pdf figs/*.png


figs/%.pdf: diafigs/%.dia
	dia -t eps-builtin -e $*.eps $?
	epstopdf $*.eps -o=$@
	rm -f $*.eps

figs/%.pdf: pngfigs/%.png
	convert $? $@

figs/%.png: pngfigs/%.png
	cp $? $@

figs/%.pdf: epsfigs/%.eps
	epstopdf $? -o=$@

figs/%.pdf: pdffigs/%.pdf
	cp $? $@

run:

.depend:
