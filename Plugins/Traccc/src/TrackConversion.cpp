// This file is part of the Acts project.
//
// Copyright (C) 2024 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsPlugins/Traccc/TrackConversion.hpp"

#include <stdexcept>
#include <string>

namespace Acts::TracccPlugin {

std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>
buildSurfaceMap(const Acts::TrackingGeometry& trackingGeometry) {
  std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*> map;
  trackingGeometry.visitSurfaces([&](const Acts::Surface* surface) {
    if (surface != nullptr) {
      map.emplace(surface->geometryId(), surface);
    }
  });
  return map;
}

const Acts::Surface& detrayIdToActsSurface(
    std::uint64_t detrayId,
    const std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>&
        detrayToActsMap,
    const std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>&
        surfaceMap) {

  auto actsIdIt = detrayToActsMap.find(detrayId);
  if (actsIdIt == detrayToActsMap.end()) {
    throw std::out_of_range(
        "Acts::TracccPlugin: no Acts geometry id for detray id " +
        std::to_string(detrayId));
  }
  auto surfIt = surfaceMap.find(actsIdIt->second);
  if (surfIt == surfaceMap.end()) {
    throw std::out_of_range(
        "Acts::TracccPlugin: no surface for Acts geometry id " +
        std::to_string(actsIdIt->second.value()));
  }
  return *surfIt->second;
}

std::optional<Acts::BoundTrackParameters> convertGlobalParams(
    std::uint64_t detrayId,
    const traccc::bound_track_parameters<traccc::default_algebra>& trkParams,
    const std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>&
        detrayToActsMap,
    const std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>&
        surfaceMap) {

  if (trkParams.bound_local()[0] == 0.f && trkParams.bound_local()[1] == 0.f &&
      trkParams.phi() == 0.f && trkParams.theta() == 0.f &&
      trkParams.qop() == 0.f && trkParams.time() == 0.f) {
    return std::nullopt;
  }

  auto surface =
      detrayIdToActsSurface(detrayId, detrayToActsMap, surfaceMap)
          .getSharedPtr();

  Acts::BoundVector params;
  params << trkParams.bound_local()[0], trkParams.bound_local()[1],
      trkParams.phi(), trkParams.theta(), trkParams.qop(), trkParams.time();

  constexpr double kGeVToMeV = 1000.0;
  Acts::BoundMatrix cov = Acts::BoundMatrix::Identity();
  cov *= (kGeVToMeV * kGeVToMeV);
  const auto& tracccCov = trkParams.covariance();
  for (unsigned i = 0; i < 5; ++i) {
    for (unsigned j = 0; j < 5; ++j) {
      cov(i, j) = tracccCov[i][j];
    }
  }

  return Acts::BoundTrackParameters(std::move(surface), params, cov,
                                    Acts::ParticleHypothesis::pion());
}

}  // namespace Acts::TracccPlugin