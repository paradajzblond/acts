// This file is part of the ACTS project.
//
// Copyright (C) 2016 CERN for the benefit of the ACTS project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Utilities/Logger.hpp"
#include "ActsExamples/Framework/IAlgorithm.hpp"
#include "ActsExamples/Framework/ProcessCode.hpp"
#include "ActsExamples/Framework/DataHandle.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"

#include <traccc/edm/measurement_collection.hpp>
#include <traccc/edm/spacepoint_collection.hpp>
#include "traccc/geometry/host_detector.hpp"
#include <traccc/edm/track_container.hpp>

#include <memory>
#include <string>

// Forward-declare to keep traccc cuda headers out of this header
// You can imagine having a separate header with includes for a different
// backend (e.g. CPU-only)
namespace ActsExamples {
struct TracccChain;
}

namespace ActsExamples {

class TracccSeqAlgorithm final : public IAlgorithm {
 public:
  struct Config {
    /// Path to the detector geometry file
    std::string detectorFile;
    /// Path to the digitization config file
    std::string digitizationFile;
    /// Path to the detector conditions file (optional)
    std::string conditionsFile;
    /// Path to the material file (optional)
    std::string materialFile;
    /// Path to the grid file (optional)
    std::string gridFile;
    /// Path to the magnetic field file
    std::string bfieldFile;
    /// Directory containing per-event cell CSV files
    std::string dataDirectory;
    // Whiteboard output keys
    std::string outputMeasurements = "traccc-measurements";
    std::string outputTracks       = "traccc-tracks";
    std::string outputDetrayToActsMap = "detray-to-acts-map";
    std::string inputMeasurements = "";  // if empty, read from dataDirectory
    std::string inputSpacepoints = "";         // if empty, read from dataDirectory

  };

  std::unordered_map<std::uint64_t, Acts::GeometryIdentifier> detrayToActsMap;

  explicit TracccSeqAlgorithm(
      const Config& cfg, std::unique_ptr<const Acts::Logger> logger = nullptr);

  ProcessCode execute(const AlgorithmContext& ctx) const override;
  ProcessCode initialize() override;
  ProcessCode finalize() override;

  const Config& config() const { return m_cfg; }

 private:
  Config m_cfg;
  std::shared_ptr<TracccChain> m_chain;

  ReadDataHandle<traccc::edm::measurement_collection::host>
    m_inputMeasurements{this, "inputMeasurements"};
  ReadDataHandle<traccc::edm::spacepoint_collection::host>
      m_inputSpacepoints{this, "inputSpacepoints"};

  WriteDataHandle<traccc::edm::measurement_collection::host>
    m_outputMeasurements{this, "OutputMeasurements"};

  WriteDataHandle<traccc::edm::track_container<traccc::default_algebra>::buffer>
    m_outputTracks{this, "OutputTracks"};

  WriteDataHandle<std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>>
    m_outputDetrayToActsMap{this, "OutputDetrayToActsMap"};
};

}  // namespace ActsExamples
