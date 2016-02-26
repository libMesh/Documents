#!/usr/bin/python

import sys
import os
import glob

import matplotlib
from matplotlib import rc
rc('text',usetex=True)

# Different modes
# By default, you can "show()" the figure which gives you an interactive window
# and it will save a .png when you call savefig().
# PDF produces a .pdf file, but show() doesn't seem to work.
matplotlib.use("PDF")

import matplotlib.pyplot as plot
from numpy import *

data_2d_lock = loadtxt("../data/libmesh/mic_2d_p1_lock.dat", comments="%")
data_2d_nolock = loadtxt("../data/libmesh/mic_2d_p1_nolock.dat", comments="%")
data_3d_lock = loadtxt("../data/libmesh/mic_3d_p1_lock.dat", comments="%")
data_3d_nolock = loadtxt("../data/libmesh/mic_3d_p1_nolock.dat", comments="%")

threads_2d_400_lock = data_2d_lock[0:4,2]
speedup_2d_400_lock = data_2d_lock[0,3]/data_2d_lock[0:4,3]
threads_2d_1600_lock = data_2d_lock[4:10,2]
speedup_2d_1600_lock = data_2d_lock[4,3]/data_2d_lock[4:10,3]
threads_2d_6400_lock = data_2d_lock[10:16,2]
speedup_2d_6400_lock = data_2d_lock[10,3]/data_2d_lock[10:16,3]
threads_2d_25600_lock = data_2d_lock[16:22,2]
speedup_2d_25600_lock = data_2d_lock[16,3]/data_2d_lock[16:22,3]
threads_2d_102400_lock = data_2d_lock[22:27,2]
speedup_2d_102400_lock = data_2d_lock[22,3]/data_2d_lock[22:27,3]

threads_2d_400_nolock = data_2d_nolock[0:7,2]
speedup_2d_400_nolock = data_2d_nolock[0,3]/data_2d_nolock[0:7,3]
threads_2d_1600_nolock = data_2d_nolock[7:14,2]
speedup_2d_1600_nolock = data_2d_nolock[7,3]/data_2d_nolock[7:14,3]
threads_2d_6400_nolock = data_2d_nolock[14:21,2]
speedup_2d_6400_nolock = data_2d_nolock[14,3]/data_2d_nolock[14:21,3]
threads_2d_25600_nolock = data_2d_nolock[21:28,2]
speedup_2d_25600_nolock = data_2d_nolock[21,3]/data_2d_nolock[21:28,3]
threads_2d_102400_nolock = data_2d_nolock[28:33,2]
speedup_2d_102400_nolock = data_2d_nolock[28,3]/data_2d_nolock[28:33,3]

threads_3d_1000_lock = data_3d_lock[0:6,2]
speedup_3d_1000_lock = data_3d_lock[0,3]/data_3d_lock[0:6,3]
threads_3d_8000_lock = data_3d_lock[7:13,2]
speedup_3d_8000_lock = data_3d_lock[7,3]/data_3d_lock[7:13,3]

threads_3d_1000_nolock = data_3d_nolock[0:7,2]
speedup_3d_1000_nolock = data_3d_nolock[0,3]/data_3d_nolock[0:7,3]
threads_3d_8000_nolock = data_3d_nolock[7:14,2]
speedup_3d_8000_nolock = data_3d_nolock[7,3]/data_3d_nolock[7:14,3]

fsize=32

tick_label_fontsize=fsize
axis_label_fontsize=fsize
matplotlib.rc('xtick', labelsize=tick_label_fontsize )
#matplotlib.rc(('xtick.major','xtick.minor'),  pad=10)
matplotlib.rc('ytick', labelsize=tick_label_fontsize)

plot.xlabel(r"\# of Threads", fontsize=fsize)
plot.ylabel(r"Speedup Factor",fontsize=fsize)
plot.title("MIC Speedup",fontsize=fsize)

lw=3
facecolor="black"
marksize=7

plot.plot( threads_2d_400_nolock, speedup_2d_400_nolock, "b-*", label="2D, no mutex", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_1600_nolock, speedup_2d_1600_nolock, "b-*", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_6400_nolock, speedup_2d_6400_nolock, "b-*", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_25600_nolock, speedup_2d_25600_nolock, "b-*", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_102400_nolock, speedup_2d_102400_nolock, "b-*", linewidth=lw, markerfacecolor=facecolor, ms=marksize )

plot.plot( threads_2d_400_lock, speedup_2d_400_lock, "b--o", label="2D, mutex", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_1600_lock, speedup_2d_1600_lock, "b--o", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_6400_lock, speedup_2d_6400_lock, "b--o", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_25600_lock, speedup_2d_25600_lock, "b--o", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_2d_102400_lock, speedup_2d_102400_lock, "b--o", linewidth=lw, markerfacecolor=facecolor, ms=marksize )

plot.plot( threads_3d_1000_nolock, speedup_3d_1000_nolock, "r-*", label="3D, no mutex", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_3d_8000_nolock, speedup_3d_8000_nolock, "r-*", linewidth=lw, markerfacecolor=facecolor, ms=marksize )

plot.plot( threads_3d_1000_lock, speedup_3d_1000_lock, "r--o", label="3D, mutex", linewidth=lw, markerfacecolor=facecolor, ms=marksize )
plot.plot( threads_3d_8000_lock, speedup_3d_8000_lock, "r--o", linewidth=lw, markerfacecolor=facecolor, ms=marksize )

plot.plot( [0, 60], [0, 60], "k-", label="Ideal", linewidth=lw, markerfacecolor=facecolor )

#plot.axis('scaled')

plot.legend(loc="upper left",prop={'size':24})
plot.grid(True)

plot.axes().set_aspect(1.0)

plot.savefig( "threaded_speedup.pdf", bbox_inches='tight' )
plot.show()
