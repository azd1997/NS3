#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("BottleNeckTcpScriptExample");

int
main (int argc, char *argv[])
{
    Time::SetResolution (Time::NS);//设置时间单位为纳秒
    LogComponentEnable ("BottleNeckTcpScriptExample", LOG_LEVEL_INFO);
    LogComponentEnable ("TcpL4Protocol", LOG_LEVEL_INFO);
//    LogComponentEnable ("TcpSocketImpl", LOG_LEVEL_ALL);
    LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);

    Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (1024));
    Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("50Mb/s"));
    CommandLine cmd;
    cmd.Parse (argc,argv);

    NodeContainer nodes;
    nodes.Create (6);//创建六个节点

    //各条边的节点组合
    vector<NodeContainer> nodeAdjacencyList(5);
    nodeAdjacencyList[0]=NodeContainer(nodes.Get(0),nodes.Get(4));
    nodeAdjacencyList[1]=NodeContainer(nodes.Get(1),nodes.Get(4));
    nodeAdjacencyList[2]=NodeContainer(nodes.Get(4),nodes.Get(5));
    nodeAdjacencyList[3]=NodeContainer(nodes.Get(5),nodes.Get(2));
    nodeAdjacencyList[4]=NodeContainer(nodes.Get(5),nodes.Get(3));

    vector<PointToPointHelper> pointToPoint(5);
    pointToPoint[0].SetDeviceAttribute ("DataRate", StringValue ("300Kbps"));//网卡最大速率
    pointToPoint[0].SetChannelAttribute ("Delay", StringValue ("2ms"));

    pointToPoint[1].SetDeviceAttribute ("DataRate", StringValue ("20Mbps"));//网卡最大速率
    pointToPoint[1].SetChannelAttribute ("Delay", StringValue ("2ms"));

    pointToPoint[2].SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));//网卡最大速率
    pointToPoint[2].SetChannelAttribute ("Delay", StringValue ("2ms"));

    pointToPoint[3].SetDeviceAttribute ("DataRate", StringValue ("20Mbps"));//网卡最大速率
    pointToPoint[3].SetChannelAttribute ("Delay", StringValue ("2ms"));

    pointToPoint[4].SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));//网卡最大速率
    pointToPoint[4].SetChannelAttribute ("Delay", StringValue ("2ms"));

    vector<NetDeviceContainer> devices(5);
    for(uint32_t i=0; i<5; i++)
    {
        devices[i] = pointToPoint[i].Install (nodeAdjacencyList[i]);
    }

    InternetStackHelper stack;
    stack.Install (nodes);//安装协议栈，tcp、udp、ip等

    Ipv4AddressHelper address;
    vector<Ipv4InterfaceContainer> interfaces(5);
    for(uint32_t i=0; i<5; i++)
    {
        ostringstream subset;
        subset<<"10.1."<<i+1<<".0";
        address.SetBase(subset.str().c_str (),"255.255.255.0");//设置基地址（默认网关）、子网掩码
        interfaces[i]=address.Assign(devices[i]);//把IP地址分配给网卡,ip地址分别是10.1.1.1和10.1.1.2
    }

    // Create a packet sink on the star "hub" to receive these packets
    uint16_t port = 50000;
    ApplicationContainer sinkApp;
    Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
    sinkApp.Add(sinkHelper.Install(nodeAdjacencyList[1].Get(0)));
    sinkApp.Add(sinkHelper.Install (nodeAdjacencyList[4].Get(1)));
    sinkApp.Add(sinkHelper.Install(nodeAdjacencyList[3].Get(1)));
    sinkApp.Start (Seconds (0.0));
    sinkApp.Stop (Seconds (30.0));

    OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address ());
    clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer clientApps;
    //A->B
    AddressValue remoteAddress
    (InetSocketAddress (interfaces[1].GetAddress (0), port));
    clientHelper.SetAttribute("Remote",remoteAddress);
    clientApps.Add(clientHelper.Install(nodeAdjacencyList[0].Get(0)));

    //A->C
    remoteAddress=AddressValue(InetSocketAddress (interfaces[3].GetAddress (1), port));
    clientHelper.SetAttribute("Remote",remoteAddress);
    clientApps.Add(clientHelper.Install(nodeAdjacencyList[0].Get(0)));

    //A->D
    remoteAddress=AddressValue(InetSocketAddress (interfaces[4].GetAddress (1), port));
    clientHelper.SetAttribute("Remote",remoteAddress);
    clientApps.Add(clientHelper.Install(nodeAdjacencyList[0].Get(0)));

    //B->C
    remoteAddress=AddressValue(InetSocketAddress (interfaces[3].GetAddress (1), port));
    clientHelper.SetAttribute("Remote",remoteAddress);
    clientApps.Add(clientHelper.Install(nodeAdjacencyList[1].Get(0)));

    //B->D
    remoteAddress=AddressValue(InetSocketAddress (interfaces[4].GetAddress (1), port));
    clientHelper.SetAttribute("Remote",remoteAddress);
    clientApps.Add(clientHelper.Install(nodeAdjacencyList[1].Get(0)));

    //C->D
    remoteAddress=AddressValue(InetSocketAddress (interfaces[4].GetAddress (1), port));
    clientHelper.SetAttribute("Remote",remoteAddress);
    clientApps.Add(clientHelper.Install(nodeAdjacencyList[3].Get(1)));
    clientApps.Start(Seconds(1.0));
    clientApps.Stop (Seconds (3601.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    //嗅探,记录所有节点相关的数据包
    for(uint32_t i=0; i<5; i++)
        pointToPoint[i].EnablePcapAll("bottleneckTcp");

    Simulator::Run ();
    Simulator::Destroy ();
    return 0;
}
