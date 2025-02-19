diasources := $(wildcard diafigs/*.dia)
pngsources := $(wildcard pngfigs/*.png)
epssources := $(wildcard epsfigs/*.eps)
pdfsources := $(wildcard pdffigs/*.pdf)
figsources := $(wildcard figfigs/*.fig)


diafigures := $(patsubst diafigs/%.dia, figs/%.pdf, $(diasources))
pngfigures := $(patsubst pngfigs/%.png, figs/%.png, $(pngsources))
epsfigures := $(patsubst epsfigs/%.eps, figs/%.pdf, $(epssources))
pdffigures := $(patsubst pdffigs/%.pdf, figs/%.pdf, $(pdfsources))
figfigures := $(patsubst figfigs/%.fig, figs/%.pdf, $(figsources))
figures := $(diafigures) $(pngfigures) $(epsfigures) $(pdffigures) $(figfigures)

all: main.pdf

main.pdf: *.tex $(figures)
	pdflatex main && pdflatex main || (rm -f main.pdf; false)

clean: cleanslides cleanfigs

clobber: clean
	rm -f main.pdf

cleanslides:
	rm -f main.aux main.bbl main.blg main.log main.dvi main.nav main.out main.snm main.toc main.vrb
	rm -f *.aux

cleanfigs:
	rm -f figs/*.pdf figs/*.png


figs/%.pdf: diafigs/%.dia
	dia -t cairo-eps -e $*.eps $?
	epstopdf $*.eps -o=$@
	rm -f $*.eps

figs/%.png: pngfigs/%.png
	cp $? $@

figs/%.pdf: epsfigs/%.eps
	epstopdf $? -o=$@

figs/%.pdf: pdffigs/%.pdf
	cp $? $@

figs/%.pdf: figfigs/%.fig
	fig2dev -L pdf $? $@

run:

.depend:
