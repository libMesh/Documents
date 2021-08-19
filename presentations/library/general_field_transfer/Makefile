# Makefile for LaTeX files

# Original Makefile from http://www.math.psu.edu/elkin/math/497a/Makefile

# Please check http://www.acoustics.hut.fi/u/mairas/UltimateLatexMakefile
# for new versions.

# Copyright (c) 2005,2006 (in order of appearance):
#	Matti Airas <Matti.Airas@hut.fi>
# 	Rainer Jung
#	Antoine Chambert-Loir
#	Timo Kiravuo

# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:

# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# $Id: Makefile,v 1.18 2006-06-19 10:58:11 mairas Exp $

# Cause LaTeX to wrap lines after 1000 characters instead of the default.
# Makes using grep easier.
export max_print_line=1000

LATEX	= latexmk -g -pdf

MUTE=@
#MUTE=
#SILENT=> /dev/null
SILENT=
VERYSILENT=1> /dev/null 2>/dev/null
#VERYSILENT=

DATE=$(shell date +%Y-%m-%d)

SRC    := $(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
INPUTS := $(wildcard *.tex)
PDF	= $(SRC:%.tex=%.pdf)

define notify_latex
	echo "\nRunning LaTeX ..."
endef

define run-latex
	$(MUTE)$(notify_latex);$(LATEX) $<
	# Display relevant warnings
	$(MUTE)if ! egrep -i "((Reference|Citation).*undefined)|(Label.*multiply defined)" $(<:%.tex=%.log); then true; else false; fi
endef

all 	: $(PDF)

.PHONY	: all clean pdf veryclean

clean	:
	  $(MUTE)-rm -f $(PDF) $(PSF) $(PDF) $(PDF:%.pdf=%.aux) $(PDF:%.pdf=%.bbl) $(PDF:%.pdf=%.blg) $(PDF:%.pdf=%.log) $(PDF:%.pdf=%.out) $(PDF:%.pdf=%.idx) $(PDF:%.pdf=%.ilg) $(PDF:%.pdf=%.ind) $(PDF:%.pdf=%.toc) $(PDF:%.pdf=%.fdb_latexmf) $(PDF:%.pdf=%.fls)

veryclean	: clean
	  $(MUTE)-rm -f *.log *.aux *.dvi *.bbl *.blg *.ilg *.toc *.lof *.lot *.idx *.ind *.ps *.vrb *.nav *.snm *.fdb_latexmk *~

$(PDF) : %.pdf : %.tex $(INPUTS)
	$(MUTE)$(run-latex)

pdf	: $(PDF)
