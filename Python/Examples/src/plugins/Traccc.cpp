// This file is part of the Acts project.
//
// Copyright (C) 2024 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ActsExamples/Traccc/ActsMeasToTracccAlg.hpp"
#include "ActsExamples/Traccc/TracccMeasToActsAlg.hpp"
#include "ActsExamples/Traccc/TracccSeqAlg.hpp"
#include "ActsExamples/Traccc/TracccTrackToActsAlg.hpp"
#include "ActsExamples/Traccc/ActsSpToTracccAlg.hpp"

#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(ActsExamplesPythonBindingsTraccc, traccc) {

  // ── TracccSeqAlgorithm ──────────────────────────────────────────────────
  {
    using Alg = ActsExamples::TracccSeqAlgorithm;
    using Cfg = Alg::Config;

    auto cls = py::class_<Alg, ActsExamples::IAlgorithm,
                        std::shared_ptr<Alg>>(traccc, "TracccSeqAlgorithm")
    .def(py::init([](const Cfg& cfg, Acts::Logging::Level level) {
      return std::make_shared<Alg>(
          cfg, Acts::getDefaultLogger("TracccSeqAlgorithm", level));
    }));

    py::class_<Cfg>(cls, "Config")
      .def(py::init<>())
      .def_readwrite("detectorFile",       &Cfg::detectorFile)
      .def_readwrite("digitizationFile",   &Cfg::digitizationFile)
      .def_readwrite("conditionsFile",     &Cfg::conditionsFile)
      .def_readwrite("materialFile",       &Cfg::materialFile)
      .def_readwrite("gridFile",           &Cfg::gridFile)
      .def_readwrite("bfieldFile",         &Cfg::bfieldFile)
      .def_readwrite("dataDirectory",      &Cfg::dataDirectory)
      .def_readwrite("outputMeasurements", &Cfg::outputMeasurements)
      .def_readwrite("outputTracks",       &Cfg::outputTracks)
      .def_readwrite("inputMeasurements", &Cfg::inputMeasurements)
      .def_readwrite("inputSpacepoints",        &Cfg::inputSpacepoints)
      .def_readwrite("outputDetrayToActsMap", &Cfg::outputDetrayToActsMap);
  }

  // ── MeasurementConversionAlg ─────────────────────────────────────────────
  {
    using Alg = ActsExamples::ActsMeasToTracccAlg;
    using Cfg = Alg::Config;

    auto cls = py::class_<Alg, ActsExamples::IAlgorithm,
                        std::shared_ptr<Alg>>(traccc, "ActsMeasToTracccAlg")
    .def(py::init([](const Cfg& cfg, Acts::Logging::Level level) {
      return std::make_shared<Alg>(
          cfg, Acts::getDefaultLogger("ActsMeasToTracccAlg", level));
    }));

    py::class_<Cfg>(cls, "Config")
      .def(py::init<>())
      .def_readwrite("detectorFile",             &Cfg::detectorFile)
      .def_readwrite("inputActsMeasurements",   &Cfg::inputActsMeasurements)
      // .def_readwrite("inputDetrayToActsMap", &Cfg::inputDetrayToActsMap)
      .def_readwrite("outputDetrayToActsMap", &Cfg::outputDetrayToActsMap)
      .def_readwrite("trackingGeometry", &Cfg::trackingGeometry)
      .def_readwrite("outputTracccMeasurements",         &Cfg::outputTracccMeasurements)
      .def_readwrite("outputActsToTracccIndexMap",&Cfg::outputActsToTracccIndexMap);
      // .def_readwrite("outputTracccSpacepoints",   &Cfg::outputTracccSpacepoints);
  }
  {
    using Alg = ActsExamples::ActsSpToTracccAlg;
    using Cfg = Alg::Config;

    auto cls = py::class_<Alg, ActsExamples::IAlgorithm,
                        std::shared_ptr<Alg>>(traccc, "ActsSpToTracccAlg")
    .def(py::init([](const Cfg& cfg, Acts::Logging::Level level) {
      return std::make_shared<Alg>(
          cfg, Acts::getDefaultLogger("ActsSpToTracccAlg", level));
    }));

    py::class_<Cfg>(cls, "Config")
      .def(py::init<>())
      .def_readwrite("inputSpacePoints",   &Cfg::inputSpacePoints)
      .def_readwrite("inputActsMeasurements",   &Cfg::inputActsMeasurements)
      .def_readwrite("inputActsToTracccIndexMap", &Cfg::inputActsToTracccIndexMap)
      .def_readwrite("outputTracccSpacepoints",         &Cfg::outputTracccSpacepoints);
  }
  {
    using Alg = ActsExamples::TracccMeasToActsAlg;
    using Cfg = Alg::Config;

    auto cls = py::class_<Alg, ActsExamples::IAlgorithm,
                        std::shared_ptr<Alg>>(traccc, "TracccMeasToActsAlg")
    .def(py::init([](const Cfg& cfg, Acts::Logging::Level level) {
      return std::make_shared<Alg>(
          cfg, Acts::getDefaultLogger("TracccMeasToActsAlg", level));
    }));

    py::class_<Cfg>(cls, "Config")
      .def(py::init<>())
      .def_readwrite("inputTracccMeasurements",   &Cfg::inputTracccMeasurements)
      .def_readwrite("inputDetrayToActsMap", &Cfg::inputDetrayToActsMap)
      .def_readwrite("outputActsMeasurements",         &Cfg::outputActsMeasurements)
      .def_readwrite("outputMeasurementSimHitsMap",&Cfg::outputMeasurementSimHitsMap);
  }

  // ── TrackConversionAlg ───────────────────────────────────────────────────
  {
    using Alg = ActsExamples::TracccTrackToActsAlg;
    using Cfg = Alg::Config;

    auto cls = py::class_<Alg, ActsExamples::IAlgorithm,
                        std::shared_ptr<Alg>>(traccc, "TracccTrackToActsAlg")
    .def(py::init([](const Cfg& cfg, Acts::Logging::Level level) {
      return std::make_shared<Alg>(
          cfg, Acts::getDefaultLogger("TracccTrackToActsAlg", level));
    }));

    py::class_<Cfg>(cls, "Config")
      .def(py::init<>())
      .def_readwrite("inputTracccTracks", &Cfg::inputTracccTracks)
      .def_readwrite("inputDetrayToActsMap", &Cfg::inputDetrayToActsMap)
      .def_readwrite("trackingGeometry",     &Cfg::trackingGeometry)
      .def_readwrite("outputActsTracks",      &Cfg::outputActsTracks);
  }
}