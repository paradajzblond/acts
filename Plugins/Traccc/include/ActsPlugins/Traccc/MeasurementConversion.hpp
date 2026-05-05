// This file is part of the Acts project.
//
// Copyright (C) 2024 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Geometry/GeometryIdentifier.hpp"

#include <cstdint>
#include <functional>
#include <unordered_map>

#include <traccc/edm/measurement_collection.hpp>

namespace Acts::TracccPlugin {

struct ConvertedMeasurement {
  Acts::GeometryIdentifier geometryId;
  std::uint8_t             dimensions{};    // 1 or 2
  float                    loc0{};
  float                    loc1{};
  float                    var0{};
  float                    var1{};
};

std::size_t convertMeasurements(
    const std::vector<ConvertedMeasurement>& measurements,
    const std::unordered_map<Acts::GeometryIdentifier, std::uint64_t>& actsToDetrayMap,
    traccc::edm::measurement_collection::host& out);

std::size_t convertMeasurements(
    const traccc::edm::measurement_collection::host& measurements,
    const std::unordered_map<std::uint64_t, Acts::GeometryIdentifier>&
        detrayToActsMap,
    std::vector<ConvertedMeasurement>& out);

}  // namespace Acts::TracccPlugin