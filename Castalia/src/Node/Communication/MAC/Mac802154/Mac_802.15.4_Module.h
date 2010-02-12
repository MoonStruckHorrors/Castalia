/********************************************************************************
 *  Copyright: National ICT Australia,  2009 - 2010				*
 *  Developed at the ATP lab, Networked Systems theme				*
 *  Author(s): Athanassios Boulis, Yuri Tselishchev				*
 *  This file is distributed under the terms in the attached LICENSE file.	*
 *  If you do not find this file, copies can be found by writing to:		*
 *										*
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia			*
 *      Attention:  License Inquiry.						*
 ********************************************************************************/

#ifndef MAC_802154_MODULE
#define MAC_802154_MODULE

#include <map>
#include <vector>

#include "VirtualMacModule.h"
#include "Mac802154Packet_m.h"

using namespace std;

enum MacStates {
    MAC_STATE_SETUP = 1000,
    MAC_STATE_SLEEP = 1001,
    MAC_STATE_IDLE = 1002,
    MAC_STATE_CSMA_CA = 1003,
    MAC_STATE_CCA = 1004,
    MAC_STATE_IN_TX = 1005,
    MAC_STATE_WAIT_FOR_ASSOCIATE_RESPONSE = 1006,
    MAC_STATE_WAIT_FOR_DATA_ACK = 1007,
    MAC_STATE_WAIT_FOR_BEACON = 1008,
    MAC_STATE_WAIT_FOR_GTS = 1009,
    MAC_STATE_PROCESSING = 1011

};

enum Timers {
    PERFORM_CCA = 1,
    ATTEMPT_TX = 2,
    BEACON_TIMEOUT = 3,
    GTS_START = 4,
    START_SLEEPING = 5,
    FRAME_START = 6,
};

class Mac802154Module : public VirtualMacModule {

    private: 
    // parameters and variables

    /*--- A map from int value of state to its description (used in debug) ---*/
    map <int, string> stateDescr;

    int sentBeacons;
    int recvBeacons;

    /*--- A map for packet breakdown statistics ---*/
    map <string, int> packetBreak;

    /*--- The .ned file's parameters ---*/
    bool printStateTransitions;
    bool isPANCoordinator;
    bool isFFD;
    bool batteryLifeExtention;
    bool enableSlottedCSMA;
    bool enableCAP;

    int macMinBE;
    int macMaxBE;
    int macMaxCSMABackoffs;
    int macMaxFrameRetries;
    int maxLostBeacons;
    int minCAPLength;
    int unitBackoffPeriod;
    int baseSlotDuration;
    int numSuperframeSlots;
    int baseSuperframeDuration;
    int beaconOrder;
    int frameOrder;
    int requestGTS;

    /*--- General MAC variable ---*/
    int nextPacketTry;
    double nextPacketTime;
    bool lockedGTS;
    simtime_t phyDelayForValidCS;      	// delay for valid CS
    simtime_t phyDelayForSleep2Listen;	// delay to switch from sleep state to listen state
    simtime_t phyDelayForListen2Tx;
    int phyLayerOverhead;
    int phyBitsPerSymbol;
    double phyDataRate;

    /*--- 802154Mac state variables  ---*/
    int associatedPAN;			// ID of current PAN (-1 if not associated)
    int macState;			// current MAC state
    int nextMacState;			// next MAC state (will be switched after CSMA-CA algorithm ends)
    int CAPlength;			// duration of CAP interval (in number of superframe slots)
    int macBSN;				// beacon sequence number (unused)
    int nextPacketRetries;		// number of retries left for next packet to be sent
    int lostBeacons;			// number of consequitive lost beacon packets
    int frameInterval;			// duration of active part of the frame (in symbols)
    int beaconInterval;			// duration of the whole frame (in symbols)

    int NB, CW, BE;			// CSMA-CA algorithm parameters (Number of Backoffs, 
					// Contention Window length and Backoff Exponent)

    simtime_t CAPend;			// Absolute time of end of CAP period for current frame
    simtime_t currentFrameStart;	// Absolute recorded start time of the current frame
    simtime_t GTSstart;
    simtime_t GTSend;
    simtime_t GTSlength;

    simtime_t lastCarrierSense;		// Absolute recorded time of last carrier sense message from the radio
    simtime_t nextPacketResponse;	// Duration of timeout for receiving a reply after sending a packet
    simtime_t ackWaitDuration;		// Duration of timeout for receiving an ACK
    simtime_t symbolLen;		// Duration of transmittion of a single symbol

    string nextPacketState;
    simtime_t desyncTime;
    simtime_t desyncTimeStart;

    map <int, bool> associatedDevices;	// map of assoicated devices (for PAN coordinator)

    /*--- 802154Mac packet pointers (sometimes packet is created not immediately before sending) ---*/
    Mac802154Packet *beaconPacket;
    Mac802154Packet *associateRequestPacket;
    Mac802154Packet *nextPacket;

    /*--- 802154Mac GTS list --- */
    vector <GTSspec> GTSlist;		// list of GTS specifications (for PAN coordinator, currently unused)

    protected:
	virtual void startup();
	
	void fromNetworkLayer(cPacket *, int);
	void fromRadioLayer(cPacket *, double, double);
	
	virtual void finishSpecific();
	void readIniFileParameters(void);
	void setMacState(int newState);
	void handleAckPacket(Mac802154Packet*);
	void carrierIsClear();
	void initiateCSMACA(int, int, simtime_t);
	void initiateCSMACA();
	void continueCSMACA();
	void attemptTransmission();
	void transmitNextPacket();
	void issueGTSrequest();
	void timerFiredCallback(int);
};

#endif //MAC_802154_MODULE