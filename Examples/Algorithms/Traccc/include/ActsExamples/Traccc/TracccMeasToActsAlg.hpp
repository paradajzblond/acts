// This file is part of the Acts project.
//
// Copyright (C) 2024 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ActsExamples/EventData/Measurement.hpp"
#include "ActsExamples/Framework/DataHandle.hpp"
#include "ActsExamples/Framework/IAlgorithm.hpp"
#include "ActsExamples/Framework/ProcessCode.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#include <traccc/edm/measurement_collection.hpp>

namespace ActsExamples {

class TracccMeasToActsAlg final : public IAlgorithm {
 public:
  struct Config {
    std::string inputTracccMeasurements = "traccc-measurements";
    std::string inputDetrayToActsMap      = "detray-to-acts-map";

    std::string outputActsMeasurements          = "acts-measurements";
    std::string outputMeasurementSimHitsMap = "measurement-simhits-map";
  };

  TracccMeasToActsAlg(const Config& cfg,
                         std::unique_ptr<const Acts::Logger> logger);

  ProcessCode execute(const AlgorithmContext& ctx) const final;

  const Config& config() const { return m_cfg; }

 private:
  Config m_cfg;

  ReadDataHandle<traccc::edm::measurement_collection::host>
    m_inputTracccMeasurements{this, "InputTracccMeasurements"};
  ReadDataHandle<std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>>
    m_inputDetrayToActsMap{this, "InputDetrayToActsMap"};
  WriteDataHandle<MeasurementContainer> m_outputActsMeasurements{
      this, "OutputActsMeasurements"};
  WriteDataHandle<IndexMultimap<Index>> m_outputMeasurementSimHitsMap{
      this, "OutputMeasurementSimHitsMap"};
};

}  // namespace ActsExamples