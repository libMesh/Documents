FILENAME := talk

all: draft

draft: $(FILENAME).tex
	pdflatex "\def\UseOption{draft}\input{$(FILENAME).tex}"

final: $(FILENAME).tex
	pdflatex "\def\UseOption{final}\input{$(FILENAME).tex}"

ps: $(FILENAME).tex
	dvips $(FILENAME).dvi -o $(FILENAME).ps

dvi: $(FILENAME).tex
	latex2dvi $(FILENAME).tex

pdf: $(FILENAME).tex
	latex2pdf $(FILENAME).tex

bib: $(FILENAME).bib
	bibtex $(FILENAME)

clean: 
	rm -rf *.aux *.dvi *.log *.out *~
