/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015, IMDEA Networks Institute
 * Author: Hany Assasa <hany.assasa@gmail.com>
 */
#ifndef DMG_ADHOC_WIFI_MAC_H
#define DMG_ADHOC_WIFI_MAC_H

#include "dmg-wifi-mac.h"

#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"

#include "supported-rates.h"
#include "amsdu-subframe-header.h"

namespace ns3  {

class MgtAddBaRequestHeader;
class UniformRandomVariable;

/**
 * \ingroup wifi
 *
 * The Wifi MAC high model for a non-AP STA in a BSS.
 */
class DmgAdhocWifiMac : public DmgWifiMac
{
public:
  static TypeId GetTypeId (void);

  DmgAdhocWifiMac ();
  virtual ~DmgAdhocWifiMac ();

  /**
   * \param address the current address of this MAC layer.
   */
  virtual void SetAddress (Mac48Address address);
  /**
   * \param packet the packet to send.
   * \param to the address to which the packet should be sent.
   *
   * The packet should be enqueued in a tx queue, and should be
   * dequeued as soon as the channel access function determines that
   * access is granted to this MAC.
   */
  virtual void Enqueue (Ptr<const Packet> packet, Mac48Address to);

  Ptr<MultiBandElement> GetMultiBandElement (void) const;
  /**
   * Add manual Antenna Config for a specific DMG STA.
   * \param txSectorID
   * \param txAntennaID
   * \param rxSectorID
   * \param rxAntennaID
   * \param address The address of the peer DMG STA.
   */
  void AddAntennaConfig (SECTOR_ID txSectorID, ANTENNA_ID txAntennaID,
                         SECTOR_ID rxSectorID, ANTENNA_ID rxAntennaID,
                         Mac48Address address);

protected:
  virtual void DoDispose (void);
  virtual void DoInitialize (void);

  void StartBeaconInterval (void);
  void StartBeaconTransmissionInterval (void);
  void StartAssociationBeamformTraining (void);
  void StartAnnouncementTransmissionInterval (void);
  void StartDataTransmissionInterval (void);
  void FrameTxOk (const WifiMacHeader &hdr);
  void BrpSetupCompleted (Mac48Address address);
  virtual void NotifyBrpPhaseCompleted (void);

private:
  virtual void Receive (Ptr<Packet> packet, const WifiMacHeader *hdr);
  /**
   * Return the DMG capability of the current STA.
   *
   * \return the DMG capability that we support
   */
  Ptr<DmgCapabilities> GetDmgCapabilities (void) const;

};

} // namespace ns3

#endif /* DMG_ADHOC_WIFI_MAC_H */
