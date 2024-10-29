NAME=paper

all: $(NAME).pdf

# We have a custom .latexmkrc file in this directory which tells latexmk how to make the glossary.
# https://tex.stackexchange.com/questions/1226/how-to-make-latexmk-use-makeglossaries
$(NAME).pdf: $(NAME).tex $(NAME).bib
	latexmk -g -pdf -logfilewarnings $<

# Removes all temporary files including output files (dvi/ps/pdf).
# Note: "latexmk -c" doesn't remove output files.
clean:
	latexmk -C
	rm -f *~ .*~
