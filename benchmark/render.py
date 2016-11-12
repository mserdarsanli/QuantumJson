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
	},
	{
		"id": "scenario2",
		"name": "Scenario 2 (OMDB)",
	},
	{
		"id": "scenario3",
		"name": "Scenario 3 (StackOverflow)",
	},
]

BENCHMARK_LIBRARIES = [
	{
		"id": "nlohmann_json",
		"url": "https://github.com/nlohmann/json",
	},
	{
		"id": "quantumjson",
		"url": "https://github.com/mserdarsanli/QuantumJson",
	},
	{
		"id": "jsoncpp",
		"url": "https://github.com/open-source-parsers/jsoncpp",
	},
	{
		"id": "rapidjson",
		"url": "https://github.com/miloyip/rapidjson",
	},
	{
		"id": "boost_pt",
		"url": "http://www.boost.org/doc/libs/release/libs/property_tree/",
	},
	{
		"id": "staticjson",
		"url": "https://github.com/netheril96/StaticJSON",
	},
]

BENCHMARK_DATA = {}

def render_md():
	print('# Benchmarks')
	print('')

	for sc in BENCHMARK_SCENARIOS:
		print('##', sc['name'])
		print('')

		for mode in BENCHMARK_MODES:
			print('| Library | {} |'.format(mode['unit']))
			print('| ------- | {} |'.format('-' * len(mode['unit'])))

			for lib in BENCHMARK_LIBRARIES:
				res = BENCHMARK_DATA[ (sc['id'], lib['id'], mode['id']) ]
				print('| {} | {} |'.format(lib['id'], res))
			print('')

def render_html():
	assert 0 == 'not implemented'

def main():
	parser = argparse.ArgumentParser(description='Render benchmark results')
	parser.add_argument('--input_tsv', required=True)
	parser.add_argument('--render_target', choices=['md', 'html'], required=True)
	args = parser.parse_args()

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
