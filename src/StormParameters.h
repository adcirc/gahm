//
// Created by Zach Cobell on 7/1/21.
//

#ifndef GAHM_SRC_STORMPARAMETERS_H_
#define GAHM_SRC_STORMPARAMETERS_H_

class StormParameters {
 public:
  StormParameters()
      : m_cycle(0),
        m_wtratio(0.0),
        m_latitude(0.0),
        m_longitude(0.0),
        m_central_pressure(Constants::backgroundPressure()),
        m_background_pressure(Constants::backgroundPressure()),
        m_vmax(0.0),
        m_utrans(0.0),
        m_vtrans(0.0),
        m_uvtrans(0.0),
        m_corio(0.0){};

  int cycle() const { return m_cycle; }
  void setCycle(int cycle) { m_cycle = cycle; }
  double wtratio() const { return m_wtratio; }
  void setWtratio(double wtratio) { m_wtratio = wtratio; }
  double latitude() const { return m_latitude; }
  void setLatitude(double latitude) { m_latitude = latitude; }
  double longitude() const { return m_longitude; }
  void setLongitude(double longitude) { m_longitude = longitude; }
  double centralPressure() const { return m_central_pressure; }
  void setCentralPressure(double centralPressure) {
    m_central_pressure = centralPressure;
  }
  double backgroundPressure() const { return m_background_pressure; }
  void setBackgroundPressure(double backgroundPressure) {
    m_background_pressure = backgroundPressure;
  }
  double vmax() const { return m_vmax; }
  void setVmax(double vmax) { m_vmax = vmax; }
  double utrans() const { return m_utrans; }
  void setUtrans(double utrans) { m_utrans = utrans; }
  double vtrans() const { return m_vtrans; }
  void setVtrans(double vtrans) { m_vtrans = vtrans; }
  double uvtrans() const { return m_uvtrans; }
  void setUvtrans(double uvtrans) { m_uvtrans = uvtrans; }
  double corio() const { return m_corio; }
  void setCorio(double corio) { m_corio = corio; }

 private:
  int m_cycle;
  double m_wtratio;
  double m_latitude;
  double m_longitude;
  double m_central_pressure;
  double m_background_pressure;
  double m_vmax;
  double m_utrans;
  double m_vtrans;
  double m_uvtrans;
  double m_corio;
};

#endif  // GAHM_SRC_STORMPARAMETERS_H_
