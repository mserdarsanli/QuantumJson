# Benchmarks

## Scenario 1 (Reddit)

Parsing a large JSON when only specific fields are of interest. In this scenario, Reddit front page JSON (size: 82K) is parsed while only post URL and scores are extraceted. Besides QuantumJson and StaticJSON, libraries parse the full JSON.

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 0.691758 |
| staticjson | 22.715060 |
| rapidjson | 0.994481 |
| jsoncpp | 7.689204 |
| nlohmann_json | 7.776564 |
| boost_pt | 17.386719 |

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
| quantumjson | 1.807872 |
| staticjson | 20.913615 |
| rapidjson | 1.199518 |
| jsoncpp | 5.084865 |
| nlohmann_json | 5.332749 |
| boost_pt | 6.261509 |

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
| quantumjson | 1.448972 |
| staticjson | 21.454365 |
| rapidjson | 0.948761 |
| jsoncpp | 5.739544 |
| nlohmann_json | 6.400174 |
| boost_pt | 10.775744 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 15498 |
| staticjson | 19556 |
| rapidjson | 67395 |
| jsoncpp | 115337 |
| nlohmann_json | 74872 |
| boost_pt | 219879 |

## Scenario 4 (TFL accident stats 2015)

A large JSON (18M) from an open API

| Library | Runtime (seconds) |
| ------- | ----------------- |
| quantumjson | 1.601327 |
| staticjson | 17.845755 |
| rapidjson | 0.939199 |
| jsoncpp | 7.385636 |
| nlohmann_json | 7.521255 |
| boost_pt | 13.388425 |

| Library | Memory Allocations (bytes) |
| ------- | -------------------------- |
| quantumjson | 26661191 |
| staticjson | 26663840 |
| rapidjson | 30288258 |
| jsoncpp | 110306241 |
| nlohmann_json | 71926009 |
| boost_pt | 192770058 |

