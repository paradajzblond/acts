// This file is part of the Acts project.
//
// Copyright (C) 2024 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsPlugins/Traccc/MeasurementConversion.hpp"

namespace Acts::TracccPlugin {

std::size_t convertMeasurements(
    const std::vector<ConvertedMeasurement>& measurements,
    const std::unordered_map<Acts::GeometryIdentifier, std::uint64_t>& actsToDetrayMap,
    traccc::edm::measurement_collection::host& out) {

  std::size_t nConverted = 0;
  for (const auto& cm : measurements) {
    auto it = actsToDetrayMap.find(cm.geometryId);
    if (it == actsToDetrayMap.end()) {
      continue;
    }
    out.push_back({});
    auto meas = out.at(out.size() - 1);
    meas.surface_link() = detray::geometry::identifier{it->second};
    meas.local_position()[0] = cm.loc0;
    meas.local_position()[1] = (cm.dimensions == 2u) ? cm.loc1 : 0.f;
    meas.local_variance()[0] = cm.var0;
    meas.local_variance()[1] = (cm.dimensions == 2u) ? cm.var1 : 0.f;
    meas.dimensions() = cm.dimensions;
    meas.time() = 0.f;
    meas.diameter() = 0.f;
    meas.identifier() = nConverted;  // use as index for matching later
    meas.subspace()[0] = 0u;
    meas.subspace()[1] = (cm.dimensions == 2u) ? 1u : 0u;
    meas.cluster_index() = 0u;
    ++nConverted;
  }
  return nConverted;
}

std::size_t convertMeasurements(
    const traccc::edm::measurement_collection::host& measurements,
    const std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>&
        detrayToActsMap,
    std::vector<ConvertedMeasurement>& out) {

  std::size_t nConverted = 0;

  for (std::size_t i = 0; i < measurements.size(); ++i) {
    const auto meas = measurements.at(i);

    const std::uint64_t detrayId = meas.surface_link().value();
    auto it = detrayToActsMap.find(detrayId);
    if (it == detrayToActsMap.end()) {
      continue;
    }

    const std::uint8_t dims = static_cast<std::uint8_t>(meas.dimensions());
    if (dims != 1u && dims != 2u) {
      continue;
    }

    ConvertedMeasurement cm;
    cm.geometryId = it->second;
    cm.dimensions = dims;
    cm.loc0       = meas.local_position()[0];
    cm.loc1       = (dims == 2u) ? meas.local_position()[1] : 0.f;
    cm.var0       = meas.local_variance()[0];
    cm.var1       = (dims == 2u) ? meas.local_variance()[1] : 0.f;

    out.push_back(cm);
    ++nConverted;
  }

  return nConverted;
}

}  // namespace Acts::TracccPlugin