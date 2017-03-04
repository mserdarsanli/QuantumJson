# Benchmarks

## Scenario 1 (Reddit)

Parsing a large JSON when only specific fields are of interest. In this scenario, Reddit front page JSON (size: 82K) is parsed while only post URL and scores are extraceted. Besides QuantumJson and StaticJSON, libraries parse the full JSON.

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 0.980443 |
| staticjson | 23.170303 |
| rapidjson | 0.944226 |
| jsoncpp | 5.921002 |
| nlohmann_json | 5.389445 |
| boost_pt | 20.387187 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 4096 |
| staticjson | 8621 |
| rapidjson | 138993 |
| jsoncpp | 401249 |
| nlohmann_json | 246817 |
| boost_pt | 826874 |

## Scenario 2 (OMDB)

A JSON from OMDB (size: 1K) is parsed. OMDB API is using strings for every value, so this benchmark measures only string parsing performance.

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 1.879808 |
| staticjson | 19.167287 |
| rapidjson | 0.805877 |
| jsoncpp | 5.684941 |
| nlohmann_json | 3.072076 |
| boost_pt | 16.247301 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 879 |
| staticjson | 3193 |
| rapidjson | 66883 |
| jsoncpp | 5863 |
| nlohmann_json | 2964 |
| boost_pt | 10217 |

## Scenario 3 (StackOverflow)

StackOverflow top users JSON (size: 20K)

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 1.098934 |
| staticjson | 18.504157 |
| rapidjson | 0.672517 |
| jsoncpp | 6.865401 |
| nlohmann_json | 6.126015 |
| boost_pt | 14.758027 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 16266 |
| staticjson | 19556 |
| rapidjson | 67395 |
| jsoncpp | 115337 |
| nlohmann_json | 74872 |
| boost_pt | 252552 |

## Scenario 4 (TFL accident stats 2015)

A large JSON (18M) from an open API

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 1.503348 |
| staticjson | 17.585032 |
| rapidjson | 0.693207 |
| jsoncpp | 8.159748 |
| nlohmann_json | 5.350087 |
| boost_pt | 15.941202 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 30822082 |
| staticjson | 26663840 |
| rapidjson | 30288258 |
| jsoncpp | 110306241 |
| nlohmann_json | 71926009 |
| boost_pt | 221695565 |

