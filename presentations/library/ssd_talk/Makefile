# You must build the slides using one of the rules below.  They automatically
# include the correct documentclass.

all: ssd_talk

ssd_talk: ssd_talk.tex
	nice pdflatex "\input{standard_dc.tex}\input{${<}}"
	mv standard_dc.pdf $@.pdf


ssd_talk_handout: ssd_talk.tex
	nice pdflatex "\input{handout_dc.tex}\input{${<}}"
	mv handout_dc.pdf $@.pdf

# Convert the finished handout into a compressed 2x2 version for printing 
ssd_talk_handout4: ssd_talk_handout.pdf
	pdfnup --frame true --nup 2x2 $<
