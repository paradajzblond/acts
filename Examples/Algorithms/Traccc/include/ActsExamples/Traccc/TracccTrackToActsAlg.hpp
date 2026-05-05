// This file is part of the Acts project.
//
// Copyright (C) 2024 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ActsExamples/EventData/Track.hpp"
#include "ActsExamples/Framework/DataHandle.hpp"
#include "ActsExamples/Framework/IAlgorithm.hpp"
#include "ActsExamples/Framework/ProcessCode.hpp"

#include <memory>
#include <string>
#include <unordered_map>

#include <traccc/edm/track_container.hpp>

namespace ActsExamples {

class TracccTrackToActsAlg final : public IAlgorithm {
 public:
  struct Config {
    std::string inputTracccTracks;
    std::string outputActsTracks = "traccc-acts-tracks";
    std::string inputDetrayToActsMap = "detray-to-acts-map";
    std::string inputActsToTracccIndexMap = "acts-to-traccc-index-map";

    /// Acts tracking geometry — used to build the surface map.
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry;
  };

  TracccTrackToActsAlg(const Config& cfg,
                         std::unique_ptr<const Acts::Logger> logger);

  ProcessCode execute(const AlgorithmContext& ctx) const final;

  const Config& config() const { return m_cfg; }

 private:
  Config m_cfg;

  /// GeometryIdentifier → surface* built once at construction.
  std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>
      m_surfaceMap;

  ReadDataHandle<traccc::edm::track_container<traccc::default_algebra>::buffer>
    m_inputTracccTracks{this, "InputTracccTracks"};
  ReadDataHandle<std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>>
    m_inputDetrayToActsMap{this, "InputDetrayToActsMap"};
  ReadDataHandle<std::unordered_map<std::size_t, std::size_t>>
    m_inputActsToTracccIndexMap{this, "InputActsToTracccIndexMap"};
  WriteDataHandle<Acts::TrackContainer<Acts::ConstVectorTrackContainer,
                                     Acts::ConstVectorMultiTrajectory,
                                     std::shared_ptr>>
    m_outputActsTracks{this, "OutputActsTracks"};
};

}  // namespace ActsExamples