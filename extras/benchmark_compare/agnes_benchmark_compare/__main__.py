# SPDX-FileCopyrightText: 2026 Jeff Lafitte
# SPDX-License-Identifier: AGPL-3.0-or-later

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import argparse
import tomllib
import json

from dataclasses import dataclass
from pathlib import Path

import plotly.graph_objects as go

@dataclass
class SeriesConfig:
    display_name : str
    path         : str
    family       : str
    series       : str

@dataclass
class GraphConfig:
    title      : str
    xaxis_title: str
    output     : str
    series     : list[SeriesConfig]

@dataclass
class Config:
    graphs: list[GraphConfig]

@dataclass
class Benchmark:
    count: int
    time : float

class SeriesDict    (dict[str, list[Benchmark]]): pass # {series: [Benchmark]}
class FamilyDict    (dict[str, SeriesDict     ]): pass # {family: SeriesDict }
class BenchmarksDict(dict[str, FamilyDict     ]): pass # {path  : FamilyDict }

def _main() -> None:
    parser = argparse.ArgumentParser(
        prog        = "benchmark_compare",
        description = "Generates tables and graphs comparing results from agnes benchmarks.")
    parser.set_defaults(apply=_benchmark_compare)
    parser.add_argument("config", type=_existing_path_type, help="Path to an existing configuration file.")
    args = parser.parse_args()
    args.apply(args)

def _existing_path_type(value: str) -> str:
    if not Path(value).is_file():
        raise argparse.ArgumentTypeError(f"'{value}' does not exist.")
    return value

def _benchmark_compare(args: argparse.Namespace) -> None:
    with Path(args.config).open("rb") as config_file:
        config_dict = tomllib.load(config_file)
    config = Config([GraphConfig(
        graph["title"],
        graph["xaxis_title"],
        graph["output"],
        [SeriesConfig(**series_) for series_ in graph["series"]]) for graph in config_dict["graphs"]])
    benchmarks = _load_benchmarks(config)
    for graph_config in config.graphs:
        _save_graph(graph_config, benchmarks)

def _load_benchmarks(config: Config) -> BenchmarksDict:
    benchmarks = BenchmarksDict()
    for graph in config.graphs:
        for series in graph.series:
            benchmarks.setdefault(
                series.path, dict()).setdefault(
                    series.family, dict()).setdefault(
                        series.series, list())
    for path, families in benchmarks.items():
        with Path(path).open() as benchmarks_file:
            benchmarks_json = json.load(benchmarks_file)
        for benchmark_json in benchmarks_json["benchmarks"]:
            family_name, count = benchmark_json["run_name"].split("/")
            series_name        = benchmark_json["aggregate_name"]
            series = families.get(family_name, dict()).get(series_name)
            if series is not None:
                series.append(Benchmark(int(count), benchmark_json["cpu_time"]/1000))
        for family in benchmarks.values():
            for series in family.values():
                for benchmarks_ in series.values():
                    benchmarks_.sort(key=lambda benchmark: benchmark.count)
    return benchmarks

def _save_graph(graph_config: GraphConfig, benchmarks: BenchmarksDict) -> None:
    traces: list[go.Scatter] = []
    for series_ in graph_config.series:
        benchmarks_ = benchmarks[series_.path][series_.family][series_.series]
        traces.append(go.Scatter(
            x=[benchmark.count for benchmark in benchmarks_],
            y=[benchmark.time for benchmark in benchmarks_],
            name=series_.display_name))
    fig = go.Figure(data=traces)
    fig.update_layout(title=graph_config.title, xaxis_title=graph_config.xaxis_title, yaxis_title="Time (μs)")
    fig.update_xaxes(type="log")
    fig.update_yaxes(type="log")
    fig.write_html(graph_config.output, full_html=True)
    fig.write_image(file=graph_config.output + ".png", format="png")


"""
class Table:
    def __init__(self, benchmark_collections: dict, table_config: dict):
        self.__series: dict[str, Series] = dict()
        for series_config in table_config["series"]:
            series = Series(series_config["display_name"])
            for benchmark in benchmark_collections[series_config["benchmark_collection"]]["benchmarks"]:
                if benchmark["aggregate_name"] != table_config["aggregate"]:
                    continue
                benchmark_family, count = benchmark["run_name"].split("/")
                if benchmark_family != series_config["benchmark_family"]:
                    continue
                series.samples.append(Sample(int(count), benchmark["cpu_time"]))
            series.samples = list({sample.count: sample for sample in series.samples}.values())
            series.samples.sort(key=lambda sample: sample.count)
            self.__series[series_config["name"]] = series
        self.__output = Path(table_config["output"])

    @property
    def series(self):
        return self.__series.values()

    def save(self):
        header_row = ["Count"]
        row_list: list[(int, list[str])] = list()
        for series in self.__series.values():
            header_row.append(series.display_name)
            for sample in series.samples:
                row_list.append((sample.count, [str(sample.count)]))
        rows = dict(sorted(row_list))
        for series in self.__series.values():
            times = {sample.count: sample.time for sample in series.samples}
            for count, row in rows.items():
                row.append(times.get(count, 0.0))
        with self.__output.open("w") as file:
            file.write(",".join(header_row) + "\n")
            for row in rows.values():
                file.write(",".join(map(str, row)) + "\n")
"""

if __name__ == "__main__":
    _main()
