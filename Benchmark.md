# Benchmarks

## Scenario 1 (Reddit)

Parsing a large JSON when only specific fields are of interest. In this scenario, Reddit front page JSON (size: 82K) is parsed while only post URL and scores are extraceted. Besides QuantumJson and StaticJSON, libraries parse the full JSON.

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 0.135952 |
| staticjson | 4.799339 |
| rapidjson | 0.196754 |
| jsoncpp | 1.749077 |
| nlohmann_json | 1.649837 |
| boost_pt | 2.871581 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 3311 |
| staticjson | 8621 |
| rapidjson | 138993 |
| jsoncpp | 401249 |
| nlohmann_json | 246817 |
| boost_pt | 703932 |

## Scenario 2 (OMDB)

A JSON from OMDB (size: 1K) is parsed. OMDB API is using strings for every value, so this benchmark measures only string parsing performance.

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 0.004052 |
| staticjson | 0.074205 |
| rapidjson | 0.002850 |
| jsoncpp | 0.011770 |
| nlohmann_json | 0.024251 |
| boost_pt | 0.018409 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 698 |
| staticjson | 3193 |
| rapidjson | 66883 |
| jsoncpp | 5863 |
| nlohmann_json | 2964 |
| boost_pt | 7677 |

## Scenario 3 (StackOverflow)

StackOverflow top users JSON (size: 20K)

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 0.080496 |
| staticjson | 1.722847 |
| rapidjson | 0.049284 |
| jsoncpp | 0.495034 |
| nlohmann_json | 0.511687 |
| boost_pt | 1.128603 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 15498 |
| staticjson | 19556 |
| rapidjson | 67395 |
| jsoncpp | 115337 |
| nlohmann_json | 74872 |
| boost_pt | 219879 |

