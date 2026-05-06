// This file is part of the ACTS project.
//
// Copyright (C) 2016 CERN for the benefit of the ACTS project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ActsExamples/Traccc/TracccSeqAlg.hpp"

#include "ActsExamples/Framework/AlgorithmContext.hpp"

// This is a separate file that maintains all the cuda headers
#include "ActsExamples/Traccc/TracccChain.hpp"

#include <stdexcept>

namespace ActsExamples {

TracccSeqAlgorithm::TracccSeqAlgorithm(
    const Config& cfg, std::unique_ptr<const Acts::Logger> logger)
    : IAlgorithm("TracccSeqAlgorithm", std::move(logger)), m_cfg(cfg) {
  if (m_cfg.detectorFile.empty()) {
    throw std::invalid_argument("Detector geometry file is not configured");
  }
  if (m_cfg.conditionsFile.empty()) {
    throw std::invalid_argument("Detector conditions file is not configured");
  }
  if (m_cfg.digitizationFile.empty()) {
    throw std::invalid_argument("Detector digitization file is not configured");
  }
  if (m_cfg.bfieldFile.empty()) {
    throw std::invalid_argument("Magnetic field file is not configured");
  }
  if (m_cfg.inputMeasurements.empty() && m_cfg.dataDirectory.empty()) {
      throw std::invalid_argument(
          "TracccSeqAlgorithm: either dataDirectory or inputTracccMeasurements must be set");
  }

  m_inputMeasurements.initialize(m_cfg.inputMeasurements);
  m_inputSpacepoints.initialize(m_cfg.inputSpacepoints);
  m_outputMeasurements.initialize(m_cfg.outputMeasurements);
  m_outputTracks.initialize(m_cfg.outputTracks);
  m_outputDetrayToActsMap.initialize(m_cfg.outputDetrayToActsMap);

}

ProcessCode TracccSeqAlgorithm::initialize() {
    m_chain = std::make_shared<TracccChain>(
        m_cfg.detectorFile, m_cfg.digitizationFile, m_cfg.conditionsFile,
        m_cfg.materialFile, m_cfg.gridFile, m_cfg.bfieldFile);
    return ProcessCode::SUCCESS;
}

ProcessCode TracccSeqAlgorithm::execute(const AlgorithmContext& ctx) const {
  const std::size_t eventId = ctx.eventNumber;

  ACTS_INFO("Processing event " << eventId);

  EventResult result; // = processEvent(m_chain, m_cfg.dataDirectory, ctx.eventNumber);

  if (!m_cfg.inputMeasurements.empty()) {
      const auto& meas = m_inputMeasurements(ctx);
      const auto& sp = m_inputSpacepoints(ctx);
      result = processEvent(m_chain, std::move(
          const_cast<traccc::edm::measurement_collection::host&>(meas)), std::move(
          const_cast<traccc::edm::spacepoint_collection::host&>(sp)));
  } else {
      result = processEvent(m_chain, m_cfg.dataDirectory, ctx.eventNumber);
  }

  ACTS_INFO("Event information:");
  ACTS_INFO("read " << result.n_cells << " cells,");
  ACTS_INFO("created " << result.n_measurements << " measurements,");
  ACTS_INFO("created " << result.n_spacepoints << " spacepoints,");
  ACTS_INFO("reconstructed " << result.n_seeds << " seeds,");
  ACTS_INFO("found " << result.n_found_tracks << " tracks,");
  ACTS_INFO("resolved " << result.n_resolved_tracks << " tracks,");
  ACTS_INFO("and fitted " << result.n_fitted_tracks << " tracks.");

  m_outputMeasurements(ctx, std::move(result.measurements.value()));
  m_outputTracks(ctx, std::move(result.tracks.value()));
  m_outputDetrayToActsMap(ctx, std::move(result.detrayToActsMap));

  return ProcessCode::SUCCESS;
}

ProcessCode TracccSeqAlgorithm::finalize() {
  ACTS_INFO("Finalizing traccc GPU sequence algorithm");
  return ProcessCode::SUCCESS;
}
}  // namespace ActsExamples
