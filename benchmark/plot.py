# The MIT License (MIT)
#
# Copyright (c) 2016 Mustafa Serdar Sanli
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#!/usr/bin/env python2
# Plots benchmark data

import argparse
from collections import OrderedDict

import numpy as np
import matplotlib as mpl
mpl.use('SVG')
import matplotlib.pyplot as plt
import sys

# TODO fix colors, from http://stackoverflow.com/a/28222930
from itertools import cycle
cycol = cycle('bgrcmk').next

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--benchmark_data_file", required=True)
	parser.add_argument("--benchmark_mode", required=True)
	parser.add_argument("--out", required=True)
	args = parser.parse_args()

	# The file defines `SCENARIO_COUNT` and `data`
	benchmark_data = {}
	execfile( args.benchmark_data_file, benchmark_data )

	data = benchmark_data['data'][ args.benchmark_mode ]

	ind = np.arange(benchmark_data['SCENARIO_COUNT'])  # the x locations for the groups
	width = 0.8 / len(data)

	fig, ax = plt.subplots()

	librects = dict()
	i = 0
	xticklabels = None
	for lib, benchmark in data.iteritems():
		bm_data = OrderedDict(sorted(benchmark.iteritems()))

		if xticklabels == None:
			xticklabels = bm_data.keys()
		else:
			# Make sure that we plot the bars in correct order
			assert( xticklabels == bm_data.keys() )

		rects = ax.bar(ind + i * width, bm_data.values(), width, color=cycol())

		i = i+1
		librects[lib] = rects

	# add some text for labels, title and axes ticks
	ax.set_ylabel( benchmark_data['ylabel'][ args.benchmark_mode ] )
	ax.set_title(args.benchmark_mode)

	ax.set_xticks(ind + 0.4 ) # half of width constant
	ax.set_xticklabels(xticklabels)

	max_height = max( [ rect.get_height() for rects in librects.values() for rect in rects ] )
	ax.set_ylim([0, max_height * 1.6])

	ax.legend( librects.values(), librects.keys() )

	def autolabel(rects):
		# attach some text labels
		for rect in rects:
			height = rect.get_height()
			ax.text(rect.get_x() + rect.get_width()/2.0, height + 0.02 * max_height,
			    str(height), ha='center', va='bottom')

	for rect in librects.values():
		autolabel(rect)

	plt.savefig( args.out )

if __name__ == "__main__":
	main()
