#pragma once

#include "Acts/EventData/ParticleHypothesis.hpp"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/EventData/TrackStatePropMask.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"


#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <traccc/definitions/primitives.hpp>
#include <traccc/edm/track_container.hpp>
#include <traccc/edm/track_state_collection.hpp>

namespace Acts::TracccPlugin {

/// Build a GeometryIdentifier → surface* map from the tracking geometry.
/// Declared here, defined in TrackConversion.cpp.
std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>
buildSurfaceMap(const Acts::TrackingGeometry& trackingGeometry);

/// Translate a detray geometry id to an Acts surface.
const Acts::Surface& detrayIdToActsSurface(
    std::uint64_t detrayId,
    const std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>&
        detrayToActsMap,
    const std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>&
        surfaceMap);

template <typename state_backend_t, typename measurement_backend_t>
std::optional<Acts::BoundTrackParameters> convertState(
    const traccc::edm::track_state<state_backend_t>& state,
    const traccc::edm::measurement<measurement_backend_t>& meas,
    const std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>&
        detrayToActsMap,
    const std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>&
        surfaceMap) {

  const auto& smoothed = state.smoothed_params();
  if (smoothed.bound_local()[0] == 0.f && smoothed.bound_local()[1] == 0.f &&
      smoothed.phi() == 0.f && smoothed.theta() == 0.f &&
      smoothed.qop() == 0.f && smoothed.time() == 0.f) {
    return std::nullopt;
  }

  const std::uint64_t detrayId = smoothed.surface_link().value();
  std::shared_ptr<const Acts::Surface> surface;
  try {
    surface = detrayIdToActsSurface(detrayId, detrayToActsMap, surfaceMap)
                      .getSharedPtr();
  } catch (const std::exception& e) {
    std::cerr << "convertState: surface lookup failed for detrayId="
              << detrayId << ": " << e.what() << "\n";
    return std::nullopt;
  }

  Acts::BoundVector params;
  params << meas.local_position()[0], meas.local_position()[1],
      smoothed.phi(), smoothed.theta(), smoothed.qop(), smoothed.time();

  constexpr double kGeVToMeV = 1000.0;
  Acts::BoundMatrix cov = Acts::BoundMatrix::Identity();
  cov *= (kGeVToMeV * kGeVToMeV);
  const auto& tracccCov = smoothed.covariance();
  for (unsigned i = 0; i < 5; ++i) {
    for (unsigned j = 0; j < 5; ++j) {
      cov(i, j) = tracccCov[i][j];
    }
  }

  return Acts::BoundTrackParameters(std::move(surface), params, cov,
                                    Acts::ParticleHypothesis::pion());
}

std::optional<Acts::BoundTrackParameters> convertGlobalParams(
    std::uint64_t detrayId,
    const traccc::bound_track_parameters<traccc::default_algebra>& trkParams,
    const std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>&
        detrayToActsMap,
    const std::unordered_map<Acts::GeometryIdentifier, const Acts::Surface*>&
        surfaceMap);


}  // namespace Acts::TracccPlugin