#!/usr/bin/env python3

from pathlib import Path
import argparse
import pathlib
import sys

import acts
import acts.examples
from acts.examples.traccc import *
from acts.examples.odd import getOpenDataDetector, getOpenDataDetectorDirectory

u = acts.UnitConstants


def make_sequencer(
    s: acts.examples.Sequencer,
    detectorFile: Path,
    digitizationFile: Path,
    bfieldFile: Path,
    dataDirectory: Path,
    conditionsFile: Path,
    materialFile: Path = Path(),
    gridFile: Path = Path(),
    logLevel=acts.logging.INFO,
):
    # ── Traccc chain ──────────────────────────────────────────────────────────
    cfg = acts.examples.traccc.TracccSeqAlgorithm.Config()
    cfg.detectorFile      = str(detectorFile)
    cfg.digitizationFile  = str(digitizationFile)
    cfg.bfieldFile        = str(bfieldFile)
    cfg.dataDirectory     = str(dataDirectory)
    cfg.conditionsFile    = str(conditionsFile)
    cfg.materialFile      = str(materialFile)
    cfg.gridFile          = str(gridFile)
    cfg.outputMeasurements    = "traccc-measurements"
    cfg.outputTracks          = "traccc-tracks"
    cfg.outputDetrayToActsMap = "detray-to-acts-map"

    s.addAlgorithm(
        acts.examples.traccc.TracccSeqAlgorithm(cfg, logLevel)
    )

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--events", "-n", type=int, default=10)
    parser.add_argument("--skip", type=int, default=0)
    parser.add_argument("--output-dir", type=Path,
                        default=Path.cwd() / "traccc_output")
    parser.add_argument("--data-dir", type=Path, required=True)
    parser.add_argument("--detector-file", type=Path, required=True)
    parser.add_argument("--digitization-file", type=Path, required=True)
    parser.add_argument("--bfield-file", type=Path, required=True)
    parser.add_argument("--conditions-file", type=Path, default=None)
    parser.add_argument("--material-file", type=Path, default=None)
    parser.add_argument("--grid-file", type=Path, default=None)
    parser.add_argument("--odd-material-map", type=Path, default=None,
                        help="ODD material map for Acts geometry (optional)")
    parser.add_argument("--log-level",
                        choices=["VERBOSE","DEBUG","INFO","WARNING","ERROR","FATAL"],
                        default="INFO")
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    logLevel = getattr(acts.logging, args.log_level)

    s = acts.examples.Sequencer(
        events=args.events,
        skip=args.skip,
        numThreads=1,
        logLevel=logLevel,
        outputDir=str(args.output_dir),
        trackFpes=False,
    )

    make_sequencer(
        s,
        detectorFile=args.detector_file,
        digitizationFile=args.digitization_file,
        bfieldFile=args.bfield_file,
        dataDirectory=args.data_dir,
        conditionsFile=args.conditions_file or Path(),
        materialFile=args.material_file or Path(),
        gridFile=args.grid_file or Path(),
        logLevel=logLevel,
    )

    s.run()


if __name__ == "__main__":
    main()