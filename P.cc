/*
 * P.cc
 *
 *  Created on: Jul 12, 2017
 *      Author: chuyi
 */

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Txc17 : public cSimpleModule{
    private:
        cLongHistogram hopCountStats;
        cOutVector hopCountVector;
    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
};
//
Define_Module(Txc17);

void Txc17::initialize(){
    //counter = par("limit")
    double p = par("srp");
    hopCountStats.setName("hopCountStats");
    hopCountVector.setName("HopCount");
    if(p>0.5){
        cPacket *msg = new cPacket("s");
        msg->setByteLength(256);
        int n = gateSize("out");
        int k;
        do{
             k = intuniform(0,n-1);
        }while (k == getIndex());
        EV << "Forwarding message " << msg << " on port out[" << k << "]\n";
        scheduleAt(simTime()+exponential(1.0), msg);
    }
}
//scheduleAt(simTime()+1.0, event); specifying when it should be delivered back to the module.

void Txc17::handleMessage(cMessage* msg){

    hopCountVector.record(msg->getArrivalTime()-msg->getSendingTime());
    hopCountStats.collect(msg->getArrivalTime()-msg->getSendingTime());
    cMessage* cmsg = (cMessage *)msg->dup();
    int n = gateSize("out");
    int k;
    do{
     k = intuniform(0,n-1);
    }while (k == getIndex());
    cChannel *txChannel = gate("out",k)->getTransmissionChannel();
    simtime_t txFinishTime = txChannel->getTransmissionFinishTime();
    if (txFinishTime > simTime())
    {
        scheduleAt(simTime()+exponential(1.0), cmsg);
        // channel free; send out packet immediately
    }else{
        EV << "Forwarding message " << cmsg << " on port out[" << k << "]\n";
        send(cmsg, "out",k);
    }
}
void Txc17::finish(){
    EV << "Hop count, mean:   " << hopCountStats.getMean() << endl;
    hopCountStats.recordAs("hop count");
}



