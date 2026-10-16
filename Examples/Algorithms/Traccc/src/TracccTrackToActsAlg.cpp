// This file is part of the ACTS project.
//
// Copyright (C) 2016 CERN for the benefit of the ACTS project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ActsExamples/Traccc/TracccTrackToActsAlg.hpp"

#include "Acts/EventData/SourceLink.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "ActsExamples/EventData/IndexSourceLink.hpp"
#include "ActsExamples/Framework/AlgorithmContext.hpp"
#include "ActsPlugins/Traccc/TrackConversion.hpp"

#include <stdexcept>

namespace ActsExamples {

TracccTrackToActsAlg::TracccTrackToActsAlg(
    const Config& cfg, std::unique_ptr<const Acts::Logger> logger)
    : IAlgorithm("TracccTrackToActsAlg", std::move(logger)), m_cfg(cfg) {
  if (m_cfg.detector == nullptr) {
    throw std::invalid_argument("TracccTrackToActsAlg: detector is not set");
  }
  m_inputTracks.initialize(m_cfg.inputTracks);
  m_inputMeasurements.initialize(m_cfg.inputMeasurements);
  m_outputTracks.initialize(m_cfg.outputTracks);
}

ProcessCode TracccTrackToActsAlg::execute(const AlgorithmContext& ctx) const {
  const TracccDetector& detector = *m_cfg.detector;
  const Acts::TrackingGeometry& trackingGeometry = detector.trackingGeometry();

  const TracccHostTracks& hostTracks = m_inputTracks(ctx);
  const TracccHostMeasurements& measurements = m_inputMeasurements(ctx);

  auto trackBackend = std::make_shared<Acts::VectorTrackContainer>();
  auto trajectoryBackend = std::make_shared<Acts::VectorMultiTrajectory>();
  TrackContainer actsTracks(trackBackend, trajectoryBackend);

  // The traccc measurement identifier is the ACTS measurement index
  auto makeSourceLink = [&](unsigned int measurementIndex,
                            const Acts::Surface& surface) {
    return Acts::SourceLink{IndexSourceLink(
        surface.geometryId(), measurements.at(measurementIndex).identifier())};
  };

  const TracccTrackContainer::const_data tracksData(hostTracks);
  const ActsPlugins::Traccc::TrackConversionStatistics stats =
      ActsPlugins::Traccc::convertTracks(
          TracccTrackContainer::const_view(tracksData),
          detector.hostDetector(), trackingGeometry, makeSourceLink,
          actsTracks);

  ACTS_DEBUG("Traccc track conversion: " << stats);

  ConstTrackContainer constTracks{
      std::make_shared<Acts::ConstVectorTrackContainer>(
          std::move(*trackBackend)),
      std::make_shared<Acts::ConstVectorMultiTrajectory>(
          std::move(*trajectoryBackend))};

  m_outputTracks(ctx, std::move(constTracks));
  return ProcessCode::SUCCESS;
}

}  // namespace ActsExamples
