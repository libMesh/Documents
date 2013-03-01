diasources := $(wildcard diafigs/*.dia)
pngsources := $(wildcard pngfigs/*.png)
epssources := $(wildcard epsfigs/*.eps)
pdfsources := $(wildcard pdffigs/*.pdf)
figsources := $(wildcard figfigs/*.fig)


diafigures := $(patsubst diafigs/%.dia, figs/%.pdf, $(diasources))
pngfigures := $(patsubst pngfigs/%.png, figs/%.pdf, $(pngsources))
epsfigures := $(patsubst epsfigs/%.eps, figs/%.pdf, $(epssources))
pdffigures := $(patsubst pdffigs/%.pdf, figs/%.pdf, $(pdfsources))
figfigures := $(patsubst figfigs/%.fig, figs/%.pdf, $(figsources))
figures := $(diafigures) $(pngfigures) $(epsfigures) $(pdffigures) $(figfigures)

all: handout.pdf main.pdf

handout.pdf: *.tex $(figures)
	pdflatex handout && pdflatex handout || (rm -f handout.pdf; false)

main.pdf: *.tex $(figures)
	pdflatex main && pdflatex main || (rm -f main.pdf; false)

clean: cleanslides cleanfigs

clobber: clean
	rm -f main.pdf handout.pdf

cleanslides:
	rm -f main.{aux,bbl,blg,log,dvi,nav,out,snm,toc,vrb}
	rm -f handout.{aux,bbl,blg,log,dvi,nav,out,snm,toc,vrb}
	rm -f *.aux

cleanfigs:
	rm -f figs/*.pdf


figs/%.pdf: diafigs/%.dia
	dia -t eps-builtin -e $*.eps $?
	epstopdf $*.eps -o=$@
	rm -f $*.eps

figs/%.pdf: pngfigs/%.png
	convert $? $@

figs/%.pdf: epsfigs/%.eps
	epstopdf $? -o=$@

figs/%.pdf: pdffigs/%.pdf
	cp $? $@

figs/%.pdf: figfigs/%.fig
	fig2dev -L pdf $? $@

run:

.depend:
