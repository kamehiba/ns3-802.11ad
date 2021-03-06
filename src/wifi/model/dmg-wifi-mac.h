/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015, 2016 IMDEA Networks Institute
 * Author: Hany Assasa <hany.assasa@gmail.com>
 */
#ifndef DMG_WIFI_MAC_H
#define DMG_WIFI_MAC_H

#include "regular-wifi-mac.h"
#include "dmg-ati-dca.h"
#include "dmg-capabilities.h"

namespace ns3 {

#define dot11MaxBFTime                  10
#define dot11BFTXSSTime                 10
#define dot11MaximalSectorScan          10
#define dot11ABFTRTXSSSwitch            10
#define dot11BFRetryLimit               10
// Frames duration precalculated using MCS0 and reported in nanoseconds
#define sswTxTime     NanoSeconds (4291) + NanoSeconds (4654) + NanoSeconds (5964)
#define sswFbckTxTime NanoSeconds (4291) + NanoSeconds (4654) + NanoSeconds (9310)
#define sswAckTxTime  NanoSeconds (4291) + NanoSeconds (4654) + NanoSeconds (9310)
#define aTSFResolution MicroSeconds (1)
// Size of different DMG Control Frames in Bytes
#define POLL_FRAME_SIZE           22
#define SPR_FRAME_SIZE            27
#define GRANT_FRAME_SIZE          27
// Association Identifier
#define AID_AP                    0
#define AID_BROADCAST             255
// Antenna Configuration
#define NO_ANTENNA_CONFIG         255
// Allocation of SPs and CBAPs
#define BROADCAST_CBAP            0

typedef enum {
  CHANNEL_ACCESS_BTI = 0,
  CHANNEL_ACCESS_ABFT,
  CHANNEL_ACCESS_ATI,
  CHANNEL_ACCESS_BHI,
  CHANNEL_ACCESS_DTI
} ChannelAccessPeriod;

/** Type definitions **/
typedef uint8_t SECTOR_ID;                    /* Typedef for Sector ID */
typedef uint8_t ANTENNA_ID;                   /* Typedef for Antenna ID */

class BeamRefinementElement;

/**
 * \brief Wi-Fi DMG
 * \ingroup wifi
 *
 * Abstract class for DMG support.
 */
class DmgWifiMac : public RegularWifiMac
{
public:
  static TypeId GetTypeId (void);

  DmgWifiMac ();
  virtual ~DmgWifiMac ();

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
  virtual void Enqueue (Ptr<const Packet> packet, Mac48Address to) = 0;
  /**
   * \param stationManager the station manager attached to this MAC.
   */
  virtual void SetWifiRemoteStationManager(Ptr<WifiRemoteStationManager> stationManager);
  /**
   * Steer the directional antenna towards specific station.
   * \param address The MAC address of the peer station.
   */
  void SteerAntennaToward (Mac48Address address);

  /* Temporary Function to store AID mapping */
  void MapAidToMacAddress (uint16_t aid, Mac48Address address);
  void SetupBlockAck (uint8_t tid, Mac48Address recipient);

protected:
  friend class MacLow;

  virtual void DoDispose (void);
  virtual void DoInitialize (void);
  virtual void Configure80211ad (void);
  virtual void ConfigureAggregation (void);
  virtual void EnableAggregation (void);
  virtual void DisableAggregation (void);
  /**
   * \param sbifs the value of Short Beamforming Interframe Space (SBIFS).
   */
  void SetSbifs (Time sbifs);
  /**
   * \param mbifs the value of Medium Beamforming Interframe Space (MBIFS).
   */
  void SetMbifs (Time mbifs);
  /**
   * \param lbifs the value of Large Beamforming Interframe Space (LBIFS).
   */
  void SetLbifs (Time lbifs);
  /**
   * \param brpifs the value of Beam Refinement Protocol Interframe Spacing (BRPIFS).
   */
  void SetBrpifs (Time brpifs);
  /**
   * \return The value of of Short Beamforming Interframe Space (SBIFS)
   */
  Time GetSbifs (void) const;
  /**
   * \return The the value of Medium Beamforming Interframe Space (MBIFS).
   */
  Time GetMbifs (void) const;
  /**
   * \return The value of Large Beamforming Interframe Space (LBIFS).
   */
  Time GetLbifs (void) const;
  /**
   * \return The value of Beam Refinement Protocol Interframe Spacing (BRPIFS).
   */
  Time GetBrpifs (void) const;
  /**
   * Set whether PCP Handover is supported or not.
   * \param value
   */
  void SetPcpHandoverSupport (bool value);
  /**
   * \return Whether the PCP handover is supported or not.
   */
  bool GetPcpHandoverSupport (void) const;
  /**
   * Map received SNR value to a specific address and TX antenna configuration (The Tx of the peer station).
   * \param address The address of the receiver.
   * \param sectorID The ID of the transmitting sector.
   * \param antennaID The ID of the transmitting antenna.
   * \param snr The received Signal to Noise Ration in dBm.
   */
  void MapTxSnr (Mac48Address address, SECTOR_ID sectorID, ANTENNA_ID antennaID, double snr);
  /**
   * Map received SNR value to a specific address and RX antenna configuration (The Rx of the calling station).
   * \param address The address of the receiver.
   * \param sectorID The ID of the receiving sector.
   * \param antennaID The ID of the receiving antenna.
   * \param snr The received Signal to Noise Ration in dB.
   */
  void MapRxSnr (Mac48Address address, SECTOR_ID sectorID, ANTENNA_ID antennaID, double snr);
  /**
   * Send Information Request frame.
   * \param to The MAC address of the receiving station.
   * \param request Pointer to the Request Element.
   */
  void SendInformationRequest (Mac48Address to, ExtInformationRequest &requestHdr);
  /**
   * Send Information Response to the station that requested the information.
   * \param to The MAC address of the station requested the information.
   * \param responseHdr Pointer to the Response Element.
   */
  void SendInformationResponse (Mac48Address to, ExtInformationResponse &responseHdr);
  /**
   * Get the remaining time for the current allocation period.
   * \return The remaining time for the current allocation period.
   */
  Time GetRemainingAllocationTime (void) const;
  /**
   * Return the DMG capability of the current STA.
   * \return the DMG capability that we support
   */
  virtual Ptr<DmgCapabilities> GetDmgCapabilities (void) const = 0;
  /**
   * Send SSW FeedBack after RSS.
   * \param receiver The address of the responding station.
   */
  void SendSswFbckAfterRss (Mac48Address receiver);
  /**
   * The BRP setup subphase is used to set up beam refinement transactions.
   * \param address The MAC address of the receiving station.
   */
  void InitiateBrpSetupSubphase (Mac48Address receiver);
  /**
   * Send BRP Frame to a specific station.
   * \param receiver The MAC address of the receiving station.
   * \param requestField The BRP Request Field.
   * \param element The Beam Refinement Element.
   */
  void SendBrpFrame (Mac48Address receiver, BRP_Request_Field &requestField, BeamRefinementElement &element);
  /**
   * Send BRP Frame to a specific station.
   * \param receiver The MAC address of the receiving station.
   * \param requestField The BRP Request Field.
   * \param element The Beam Refinement Element.
   */
  void SendBrpFrame (Mac48Address receiver, BRP_Request_Field &requestField, BeamRefinementElement &element,
                     bool requestBeamRefinement, PacketType packetType, uint8_t trainingFieldLength);
  /**
   * Initiate BRP Transaction with a peer sation.
   * \param receiver The address of the peer station.
   */
  void InitiateBrpTransaction (Mac48Address receiver);
  /**
   * This function is called by dervied call to notify that BRP Phase has completed.
   */
  virtual void NotifyBrpPhaseCompleted (void) = 0;

  /* Typedefs for Recording SNR Value per Antenna Configuration */
  typedef double SNR;                                                   /* Typedef SNR */
  typedef std::pair<SECTOR_ID, ANTENNA_ID>      ANTENNA_CONFIGURATION;  /* Typedef for antenna Config (SectorID, AntennaID) */
  typedef std::map<ANTENNA_CONFIGURATION, SNR>  SNR_MAP;                /* Typedef for Map between Antenna Config and SNR */
  typedef SNR_MAP                               SNR_MAP_TX;             /* Typedef for SNR TX for each antenna configuration */
  typedef SNR_MAP                               SNR_MAP_RX;             /* Typedef for SNR RX for each antenna configuration */
  typedef std::pair<SNR_MAP_TX, SNR_MAP_RX>     SNR_PAIR;               /* Typedef for SNR RX for each antenna configuration */
  typedef std::map<Mac48Address, SNR_PAIR>      STATION_SNR_PAIR_MAP;   /* Typedef for Map between stations and their SNR Table. */

  /* Typedefs for Recording Best Antenna Configuration per Station */
  typedef ANTENNA_CONFIGURATION ANTENNA_CONFIGURATION_TX;  /* Typedef for best TX antenna Config */
  typedef ANTENNA_CONFIGURATION ANTENNA_CONFIGURATION_RX;  /* Typedef for best RX antenna Config */
  typedef std::pair<ANTENNA_CONFIGURATION_TX, ANTENNA_CONFIGURATION_RX> BEST_ANTENNA_CONFIGURATION;
  typedef std::map<Mac48Address, BEST_ANTENNA_CONFIGURATION>            STATION_ANTENNA_CONFIG_MAP;

  /**
   * Obtain antenna configuration for the highest received SNR to feed it back
   * \param stationAddress The MAC address of the station.
   * \param isTxConfiguration Is the Antenna Tx Configuration we are searching for or not.
   */
  ANTENNA_CONFIGURATION GetBestAntennaConfiguration (const Mac48Address stationAddress, bool isTxConfiguration);
  /**
   * Obtain antenna configuration for the highest received SNR to feed it back
   * \param stationAddress The MAC address of the station.
   * \param isTxConfiguration Is the Antenna Tx Configuration we are searching for or not.
   * \param maxSnr The SNR value corresponding to the BEst Antenna Configuration.
   */
  ANTENNA_CONFIGURATION GetBestAntennaConfiguration (const Mac48Address stationAddress, bool isTxConfiguration, double &maxSnr);
  /**
   * Get Relay Capabilities Informaion for this DMG STA.
   * \return
   */
  RelayCapabilitiesInfo GetRelayCapabilitiesInfo (void) const;
  /**
   * Get DMG Relay Capabilities Element supported by this DMG STA.
   * \return The DMG Relay Capabilities Information Element.
   */
  Ptr<RelayCapabilitiesElement> GetRelayCapabilitiesElement (void) const;
  /**
   * Send Relay Search Response.
   * \param to
   * \param token The dialog token.
   */
  void SendRelaySearchResponse (Mac48Address to, uint8_t token);
  /**
   * This method can be called to accept a received ADDBA Request. An
   * ADDBA Response will be constructed and queued for transmission.
   *
   * \param reqHdr a pointer to the received ADDBA Request header.
   * \param originator the MAC address of the originator.
   */
  virtual void SendAddBaResponse (const MgtAddBaRequestHeader *reqHdr,
                                  Mac48Address originator);
  /**
   * Start Beacon Interval (BI)
   */
  virtual void StartBeaconInterval (void) = 0;
  /**
   * Start Beacon Transmission Interval (BTI)
   */
  virtual void StartBeaconTransmissionInterval (void) = 0;
  /**
   * Start Association Beamform Training (A-BFT).
   */
  virtual void StartAssociationBeamformTraining (void) = 0;
  /**
   * Start Announcement Transmission Interval (ATI).
   */
  virtual void StartAnnouncementTransmissionInterval (void) = 0;
  /**
   * Start Data Transmission Interval (DTI).
   */
  virtual void StartDataTransmissionInterval (void) = 0;
  /**
   * Get current access period in the current beacon interval
   * \return The type of the access period (BTI/A-BFT/ATI or DTI).
   */
  ChannelAccessPeriod GetCurrentAccessPeriod (void) const;
  /**
   * Get allocation type for the current period.
   * \return The type of the current allocation (SP or CBAP).
   */
  AllocationType GetCurrentAllocation (void) const;
  /**
   * Start contention based access period (CBAP).
   * \param contentionDuration The duration of the contention period.
   */
  void StartContentionPeriod (AllocationID allocationID, Time contentionDuration);
  /**
   * End Contention Period.
   */
  void EndContentionPeriod (void);
  /**
   * Start service  period (SP) allocation period.
   * \param length The length of the allocation period.
   * \param peerAid The AID of the peer station in this service period.
   * \param peerStation The MAC Address of the peer station in this service period.
   * \param isSource Whether we are the initiator of the service period.
   */
  void StartServicePeriod (AllocationID allocationID, Time length, uint8_t peerAid, Mac48Address peerStation, bool isSource);
  /**
   * Resume transmission for the current service period.
   */
  void ResumeServicePeriodTransmission (void);
  /**
   * Suspend ongoing transmission for the current service period.
   */
  void SuspendServicePeriodTransmission (void);
  /**
   * Terminate service period.
   */
  void EndServicePeriod (void);
  /**
   * This function is excuted upon the transmission of frame.
   * \param hdr The header of the transmitted frame.
   */
  virtual void FrameTxOk (const WifiMacHeader &hdr) = 0;
  virtual void Receive (Ptr<Packet> packet, const WifiMacHeader *hdr);
  virtual void BrpSetupCompleted (Mac48Address address) = 0;
  /**
   * The packet we sent was successfully received by the receiver
   * (i.e. we received an ACK from the receiver).  If the packet
   * was an association response to the receiver, we record that
   * the receiver is now associated with us.
   *
   * \param hdr the header of the packet that we successfully sent
   */
  virtual void TxOk (Ptr<const Packet> packet, const WifiMacHeader &hdr);

protected:
  STATION_SNR_PAIR_MAP m_stationSnrMap;           //!< Map between stations and their SNR Table.
  STATION_ANTENNA_CONFIG_MAP m_bestAntennaConfig; //!< Map between remote stations and the best antenna configuration.
  ANTENNA_CONFIGURATION m_feedbackAntennaConfig;  //!< Temporary variable to save the best antenna config;

  Time m_sbifs;                         //!< Short Beamformnig Interframe Space.
  Time m_mbifs;                         //!< Medium Beamformnig Interframe Space.
  Time m_lbifs;                         //!< Long Beamformnig Interframe Space.
  Time m_brpifs;                        //!< Beam Refinement Protocol Interframe Spacing.
  ChannelAccessPeriod m_accessPeriod;   //!< The type of the current channel access period.
  Time m_btiDuration;                   //!< The length of the Beacon Transmission Interval (BTI).
  Time m_atiDuration;                   //!< The length of the ATI period.
  Time m_biStartTime;                   //!< The start time of the BI Interval.
  Time m_beaconInterval;		//!< Interval between beacons.
  bool m_supportRdp;                    //!< Flag to indicate whether we support RDP.
  TracedCallback<Mac48Address, ChannelAccessPeriod, SECTOR_ID, ANTENNA_ID> m_slsCompleted;  //!< Trace callback for SLS completion.
  bool m_atiPresent;                    //!< Flag to indicate if ATI period is present in the current BI.

  /** BTI Access Period Variables **/
  uint8_t m_nextBeacon;                 //!< The number of BIs following the current beacon interval during which the DMG Beacon is not be present.
  uint8_t m_btiPeriodicity;             //!< The periodicity of the BTI in BI.

  /** A-BFT Access Period Variables **/
  Time m_abftDuration;                  //!< The duration of the A-BFT access period.
  uint8_t m_ssSlotsPerABFT;             //!< Number of Sector Sweep Slots Per A-BFT.
  uint8_t m_ssFramesPerSlot;            //!< Number of SSW Frames per Sector Sweep Slot.
  uint8_t m_nextAbft;                   //!< The value of the next A-BFT in DMG Beacon.

  uint8_t m_sectorId;                   //!< Current Sector ID.
  uint8_t m_antennaId;                  //!< Current Antenna ID.
  uint16_t m_totalSectors;              //!< Total number of sectors remaining to cover.
  bool m_pcpHandoverSupport;            //!< Flat to indicate if we support PCP Handover.
  std::map<Mac48Address, bool> m_sectorFeedbackSent;  //!< Map to indicate whether we sent SSW Feedback to a station or not.

  /** ATI Period Variables **/
  Ptr<DmgAtiDca> m_dmgAtiDca;           //!< Dedicated DcaTxop for ATI.

  /* BRP Phase Variables */
  std::map<Mac48Address, bool> m_isBrpResponder;      //!< Map to indicate whether we are BRP Initiator or responder.
  std::map<Mac48Address, bool> m_isBrpSetupCompleted; //!< Map to indicate whether BRP Setup is completed with station.
  std::map<Mac48Address, bool> m_raisedBrpSetupCompleted;
  bool m_recordTrnSnrValues;                          //!< Flag to indicate if we should record reported SNR Values by TRN Fields.
  bool m_requestedBrpTraining;                        //!< Flag to indicate whether BRP Training has been performed.

  uint8_t m_antennaCount;
  uint8_t m_sectorCount;

  /* DMG Relay Variables */
  RelayDuplexMode m_relayDuplexMode;            //!< The duplex mode of the relay.
  bool m_relayMode;                             //!< Flag to indicate we are in relay mode.
  bool m_redsActivated;                         //!< DMG Station supports REDS.
  bool m_rdsActivated;                          //!< DMG Station supports RDS.
  RelayCapableStaList m_rdsList;                //!< List of Relay STAs (RDS) in the currernt DMG BSS.
  TracedCallback<Mac48Address> m_rlsCompleted;  //!< Flag to indicate we are completed RLS setup.

  /* Information Request/Response */
  typedef std::vector<Ptr<WifiInformationElement> > WifiInformationElementList;
  typedef std::pair<Ptr<DmgCapabilities>, WifiInformationElementMap> StationInformation;
  typedef std::map<Mac48Address, StationInformation> InformationMap;
  typedef InformationMap::iterator InformationMapIterator;
  InformationMap m_informationMap;

  /* DMG Parameteres */
  bool m_isCbapOnly;                            //!< Flag to indicate whether the DTI is allocated to CBAP.
  bool m_isCbapSource;                          //!< Flag to indicate that PCP/AP has higher priority for transmission.

  /* Access Period Allocations */
  AllocationID m_currentAllocationID;           //!< The ID of the current allocation.
  AllocationType m_currentAllocation;           //!< The current access period allocation.
  Time m_allocationStarted;                     //!< The time we initiated the allocation.
  Time m_currentAllocationLength;               //!< The length of the current allocation period in MicroSeconds.
  AllocationFieldList m_allocationList;         //!< List of access periods allocation in DTI.
  uint8_t m_peerStationAid;                     //!< The AID of the peer DMG STA in the current SP.
  Mac48Address m_peerStationAddress;            //!< The MAC address of the peer DMG STA in the current SP.
  Time m_suspendedPeriodDuration;               //!< The remaining duration of the suspended SP.
  bool m_spSource;                              //!< Flag to indicate if we are the source of the SP.
  bool m_beamformingTxss;                       //!< Flag to inidicate if we perform TxSS during the beamforming service period.

  /* Service Period Channel Access */
  Ptr<ServicePeriod> m_sp;                      //!< Pointer to current service period channel access pbject.

  /**
   * TracedCallback signature for BeamLink Maintenance Timer expiration.
   *
   * \param aid The AID of the peer station.
   * \param address The MAC address of the peer station.
   * \param transmissionLink The new transmission link.
   */
  typedef void (* BeamLinkMaintenanceTimerExpired)(uint8_t aid, Mac48Address address, Time timeLeft);
  TracedCallback<uint8_t, Mac48Address, Time> m_beamLinkMaintenanceTimerExpired;

  /* AID to MAC Address mapping */
  typedef std::map<uint16_t, Mac48Address> AID_MAP;
  typedef std::map<Mac48Address, uint16_t> MAC_MAP;
  AID_MAP m_aidMap;
  MAC_MAP m_macMap;

  /**
   * TracedCallback signature for service period initiation/termination.
   *
   * \param srcAddress The MAC address of the source station.
   * \param dstAddress The MAC address of the destination station.
   */
  typedef void (* ServicePeriodCallback)(Mac48Address srcAddress, Mac48Address dstAddress);
  TracedCallback<Mac48Address, Mac48Address> m_servicePeriodStartedCallback;
  TracedCallback<Mac48Address, Mac48Address> m_servicePeriodEndedCallback;

private:
  /**
   * This function is called upon transmission of a 802.11 Management frame.
   * \param hdr The header of the management frame.
   */
  void ManagementTxOk (const WifiMacHeader &hdr);
  /**
   * Report SNR Value, this is a callback to be hooked with the WifiPhy.
   * \param sectorID
   * \param antennaID
   * \param snr
   */
  void ReportSnrValue (SECTOR_ID sectorID, ANTENNA_ID antennaID, uint8_t fieldsRemaining, double snr, bool isTxTrn);

  Mac48Address m_peerStation;     /* The address of the station we are waiting BRP Response from */
  uint8_t m_dialogToken;

};

} // namespace ns3

#endif /* DMG_WIFI_MAC_H */
