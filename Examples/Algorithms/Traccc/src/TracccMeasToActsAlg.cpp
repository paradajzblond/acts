#include "ActsExamples/Traccc/TracccMeasToActsAlg.hpp"

#include "ActsPlugins/Traccc/MeasurementConversion.hpp"
#include "ActsExamples/EventData/IndexSourceLink.hpp"
#include "ActsExamples/Framework/AlgorithmContext.hpp"

#include <traccc/edm/measurement_collection.hpp>

namespace ActsExamples {

TracccMeasToActsAlg::TracccMeasToActsAlg(
    const Config& cfg, std::unique_ptr<const Acts::Logger> logger)
    : IAlgorithm("TracccMeasToActsAlg", std::move(logger)), m_cfg(cfg) {

  if (m_cfg.inputTracccMeasurements.empty()) {
    throw std::invalid_argument(
        "TracccMeasToActsAlg: inputTracccMeasurements is empty");
  }

  m_inputTracccMeasurements.initialize(m_cfg.inputTracccMeasurements);
  m_inputDetrayToActsMap.initialize(m_cfg.inputDetrayToActsMap);
  m_outputActsMeasurements.initialize(m_cfg.outputActsMeasurements);
  m_outputMeasurementSimHitsMap.initialize(m_cfg.outputMeasurementSimHitsMap);
}

ProcessCode TracccMeasToActsAlg::execute(
    const AlgorithmContext& ctx) const {

  const auto& tracccMeasurements = m_inputTracccMeasurements(ctx);
  const auto& detrayToActsMap = m_inputDetrayToActsMap(ctx);
  std::vector<Acts::TracccPlugin::ConvertedMeasurement> converted;
  converted.reserve(tracccMeasurements.size());

  const std::size_t nConverted = Acts::TracccPlugin::convertMeasurements(
      tracccMeasurements, detrayToActsMap, converted);

  ACTS_INFO("TracccMeasToActsAlg: converted "
             << nConverted << " / " << tracccMeasurements.size()
             << " measurements");

  MeasurementContainer measurements;
  measurements.reserve(nConverted);

  for (const auto& cm : converted) {
    auto proxy = measurements.makeMeasurement(cm.dimensions, cm.geometryId);

    proxy.subspaceIndexVector()[0] = Acts::eBoundLoc0;
    proxy.parameters()[0]          = cm.loc0;
    proxy.covariance()(0, 0)       = cm.var0;

    if (cm.dimensions == 2u) {
      proxy.subspaceIndexVector()[1] = Acts::eBoundLoc1;
      proxy.parameters()[1]          = cm.loc1;
      proxy.covariance()(1, 1)       = cm.var1;
    }
  }

  m_outputActsMeasurements(ctx, std::move(measurements));
  m_outputMeasurementSimHitsMap(ctx, IndexMultimap<Index>{});


  return ProcessCode::SUCCESS;
}

}  // namespace ActsExamples