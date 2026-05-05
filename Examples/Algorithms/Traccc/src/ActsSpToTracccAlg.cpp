#include "ActsExamples/Traccc/ActsSpToTracccAlg.hpp"
#include "ActsExamples/EventData/IndexSourceLink.hpp"
#include "ActsExamples/Framework/AlgorithmContext.hpp"
#include <stdexcept>

namespace ActsExamples {

ActsSpToTracccAlg::ActsSpToTracccAlg(
    const Config& cfg, std::unique_ptr<const Acts::Logger> logger)
    : IAlgorithm("ActsSpToTracccAlg", std::move(logger)),
      m_cfg(cfg) {
  m_inputSpacePoints.initialize(m_cfg.inputSpacePoints);
  m_inputActsMeasurements.initialize(m_cfg.inputActsMeasurements);
  m_inputActsToTracccIndexMap.initialize(m_cfg.inputActsToTracccIndexMap);
  m_outputTracccSpacepoints.initialize(m_cfg.outputTracccSpacepoints);
}

ProcessCode ActsSpToTracccAlg::execute(
    const AlgorithmContext& ctx) const {

  const auto& actsSpacePoints    = m_inputSpacePoints(ctx);
  const auto& actsMeasurements   = m_inputActsMeasurements(ctx);
  const auto& actsToTracccMap    = m_inputActsToTracccIndexMap(ctx);

  traccc::edm::spacepoint_collection::host tracccSpacepoints{m_mr};
  std::size_t nConverted = 0;
  std::size_t nSkipped   = 0;

  for (const auto& sp : actsSpacePoints) {
    const auto sourceLinks = sp.sourceLinks();
    if (sourceLinks.empty()) {
      ++nSkipped;
      continue;
    }

    // Get first source link — pixel or first strip of pair
    const auto& sl1 = sourceLinks[0].get<IndexSourceLink>();
    const std::size_t actsIdx1 = sl1.index();

    auto it1 = actsToTracccMap.find(actsIdx1);
    if (it1 == actsToTracccMap.end()) {
      ++nSkipped;
      continue;
    }
    const unsigned int tracccIdx1 = static_cast<unsigned int>(it1->second);

    // For strip spacepoints, get second measurement index
    unsigned int tracccIdx2 = tracccIdx1;
    if (sourceLinks.size() >= 2) {
      const auto& sl2 = sourceLinks[1].get<IndexSourceLink>();
      auto it2 = actsToTracccMap.find(sl2.index());
      if (it2 != actsToTracccMap.end()) {
        tracccIdx2 = static_cast<unsigned int>(it2->second);
      }
    }

    tracccSpacepoints.push_back({});
    auto tsp = tracccSpacepoints.at(tracccSpacepoints.size() - 1);
    tsp.measurement_index_1() = tracccIdx1;
    tsp.measurement_index_2() = tracccIdx2;
    tsp.global()[0]           = sp.x();
    tsp.global()[1]           = sp.y();
    tsp.global()[2]           = sp.z();
    tsp.z_variance()          = sp.varianceZ();
    tsp.radius_variance()     = sp.varianceR();

    ++nConverted;
  }

  ACTS_INFO("ActsSpToTracccAlg: converted " << nConverted
            << " / " << actsSpacePoints.size() << " spacepoints"
            << " (skipped " << nSkipped << ")");

  m_outputTracccSpacepoints(ctx, std::move(tracccSpacepoints));
  return ProcessCode::SUCCESS;
}

}  // namespace ActsExamples