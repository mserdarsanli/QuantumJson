import argparse
import csv

BENCHMARK_MODES = [
	{
		"id": "benchmark_speed",
		"name": "Speed Benchmark",
		"unit": "Runtime (seconds)",
	},
	{
		"id": "benchmark_memory",
		"name": "Memory Benchmark",
		"unit": "Memory Allocations (bytes)",
	},
]

BENCHMARK_SCENARIOS = [
	{
		"id": "scenario1",
		"name": "Scenario 1 (Reddit)",
		"description": "Parsing a large JSON when only specific fields"
		  + " are of interest. In this scenario, Reddit front page JSON (size: 82K)"
		  + " is parsed while only post URL and scores are extraceted."
		  + " Besides QuantumJson and StaticJSON, libraries parse the"
		  + " full JSON.",
	},
	{
		"id": "scenario2",
		"name": "Scenario 2 (OMDB)",
		"description": "A JSON from OMDB (size: 1K) is parsed. OMDB API is using"
		  + " strings for every value, so this benchmark measures only"
		  + " string parsing performance.",
	},
	{
		"id": "scenario3",
		"name": "Scenario 3 (StackOverflow)",
		"description": "StackOverflow top users JSON (size: 44K)",
	},
]

BENCHMARK_LIBRARIES = [
	{
		"id": "quantumjson",
		"url": "https://github.com/mserdarsanli/QuantumJson",
	},
	{
		"id": "staticjson",
		"url": "https://github.com/netheril96/StaticJSON",
	},
	{
		"id": "rapidjson",
		"url": "https://github.com/miloyip/rapidjson",
	},
	{
		"id": "jsoncpp",
		"url": "https://github.com/open-source-parsers/jsoncpp",
	},
	{
		"id": "nlohmann_json",
		"url": "https://github.com/nlohmann/json",
	},
	{
		"id": "boost_pt",
		"url": "http://www.boost.org/doc/libs/release/libs/property_tree/",
	},
]

BENCHMARK_DATA = {}

def render_md():
	print('# Benchmarks')
	print('')

	for sc in BENCHMARK_SCENARIOS:
		print('##', sc['name'])
		print('')
		print(sc['description'])
		print('')

		for mode in BENCHMARK_MODES:
			print('| Library | {} |'.format(mode['unit']))
			print('| ------- | {} |'.format('-' * len(mode['unit'])))

			for lib in BENCHMARK_LIBRARIES:
				res = BENCHMARK_DATA[ (sc['id'], lib['id'], mode['id']) ]
				print('| {} | {} |'.format(lib['id'], res))
			print('')

BAR_WIDTH_MAX = 300
def benchmark_bar(ratio, color):
	return '<div style="width: {}px; background-color: {};">&#65279;</div>'.format(
	    round(BAR_WIDTH_MAX*ratio), color)

def libUrl(lib):
	return '<a href="{}">{}</a>'.format(lib['url'], lib['id'])

def render_html():
	print('<h1>Benchmarks</h1>')

	for sc in BENCHMARK_SCENARIOS:
		print('<h2>{}</h2>'.format(sc['name']))
		print(sc['description'])

		for mode in BENCHMARK_MODES:
			print('<table>')
			print('<tr><th align="left">Library</th><th align="left" colspan="2">{}</th></tr>'.format(mode['unit']))

			maxRes = 0
			for lib in BENCHMARK_LIBRARIES:
				res = BENCHMARK_DATA[ (sc['id'], lib['id'], mode['id']) ]
				maxRes = max(maxRes, float(res))

			for lib in BENCHMARK_LIBRARIES:
				res = BENCHMARK_DATA[ (sc['id'], lib['id'], mode['id']) ]
				print('<tr><td>{}</td><td align="right">{}</td><td>{}</td></tr>'.format(
				    libUrl(lib), res, benchmark_bar(float(res) / maxRes, lib['color'])
				))
			print('</table>')

def main():
	parser = argparse.ArgumentParser(description='Render benchmark results')
	parser.add_argument('--input_tsv', required=True)
	parser.add_argument('--render_target', choices=['md', 'html'], required=True)
	args = parser.parse_args()

	# Use uniqe color for each libraries
	assert BENCHMARK_LIBRARIES[0]['id'] == 'quantumjson'
	BENCHMARK_LIBRARIES[0]['color'] = '#0000FF'

	for idx, lib in enumerate(BENCHMARK_LIBRARIES[1:]):
		col = round( 128 + 127 * idx/(len(BENCHMARK_LIBRARIES) - 1))
		code = hex(col)[2:]
		lib['color'] = '#{}00{}'.format(code, code)

	with open(args.input_tsv,'r') as inp:
		tsv_reader = csv.reader(inp, delimiter='\t')

		for row in tsv_reader:
			assert len(row) == 4

			BENCHMARK_DATA[ (row[0], row[1], row[2]) ] = row[3]

	assert len(BENCHMARK_DATA) == len(BENCHMARK_MODES) * len(BENCHMARK_SCENARIOS) * len(BENCHMARK_LIBRARIES)

	if args.render_target == 'md':
		render_md()
	else:
		render_html()

if __name__ == "__main__":
	main()
