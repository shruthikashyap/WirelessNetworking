#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/gnuplot.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Throughput Measurement");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nWifi = 0;
  bool tracing = true;
  double startTime = 0.0;
  double stopTime = 10.0;
  uint32_t payloadSize = 0;
  uint32_t maxPacket = 10000;
  uint32_t rate = 0;
  uint32_t cRateAlgo = 0;
  uint32_t mobMod = 0;
  StringValue dataRate;

  // Create randomness based on time
  time_t timex;
  time(&timex);
  RngSeedManager::SetSeed(timex);
  RngSeedManager::SetRun(1);

  CommandLine cmd;
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("rate", "Data rate", rate);
  cmd.AddValue ("payloadSize", "Size of the payloadSize", payloadSize);
  cmd.AddValue ("cRateAlgo", "Control rate alogrithm used", cRateAlgo);
  cmd.AddValue ("mobMod", "Mobility model used", mobMod);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // Checking if valid number of csma or wifi nodes are specified
  if (nWifi > 250 )
  {
    std::cout << "Too many wifi nodes. Cannot create more than 250." << std::endl;
    return 1;
  }

  // Set default number of nodes if not specified
  if (nWifi == 0)
    nWifi = 5;

  // Set default payloadSize if not specified
  if (payloadSize == 0)
   payloadSize = 1472; 

  // Set data rate
  switch(rate)
  {
    case 11: 
      dataRate = StringValue("DsssRate11Mbps");
      std::cout << "11" << "\t";
      break;
    case 5: 
      dataRate = StringValue("DsssRate5_5Mbps");
      std::cout << "5.5" << "\t";
      break;
    case 2: 
      dataRate = StringValue("DsssRate2Mbps");
      std::cout << "2" << "\t";
      break;
    case 1: 
      dataRate = StringValue("DsssRate1Mbps");
      std::cout << "1" << "\t";
      break;
    default: 
      dataRate = StringValue("DsssRate11Mbps");
      std::cout << "11" << "\t";
      break;
  }

  std::cout << nWifi << "\t";
  std::cout << payloadSize << "\t";

  if (verbose)
  {
    LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  }

  // Creating nodes
  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode;
  wifiApNode.Create (1);
  
  //std::cout << "The nodes and access point are created" << std::endl;
  NS_LOG_INFO("The nodes and access point are created");

  // Creating channel
  YansWifiChannelHelper channel;
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss ("ns3::FriisPropagationLossModel");

  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.Set ("RxGain", DoubleValue (0));
  phy.SetChannel (channel.Create ());

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  //std::cout << "The channel is created" << std::endl;
  NS_LOG_INFO("The channel is created");

  // Configuring MAC parameters
  // Set control rate algorithm
  //1 : ConstantRate
  //2 : Cara
  //3 : Aarf
  //4 : Arf
  switch(cRateAlgo)
  {
    case 1: 
      std::cout << "ConstantRate" << "\t";
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", dataRate, "ControlMode", dataRate);
      break;
    case 2: 
      std::cout << "Cara" << "\t";
      wifi.SetRemoteStationManager ("ns3::CaraWifiManager");
      break;
    case 3: 
      wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
      std::cout << "Aarf" << "\t";
      break;
    case 4: 
      std::cout << "Arf" << "\t";
      wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
      break;
    default: 
      std::cout << "ConstantRate" << "\t";
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", dataRate, "ControlMode", dataRate);
      break;
  }

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();
  //std::cout << "The control rate is configured" << std::endl;
  NS_LOG_INFO("The control rate is configured");
  
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);
  //std::cout << "SSID, apdevice and stadevices are configured" << std::endl;
  NS_LOG_INFO("SSID, apdevice and stadevices are configured");

  // Configuring mobility of nodes
  MobilityHelper mobility;

  // Set mobility model
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
       "MinX", DoubleValue (0.0),
       "MinY", DoubleValue (0.0),
       "DeltaX", DoubleValue (10.0),
       "DeltaY", DoubleValue (10.0),
       "GridWidth", UintegerValue (5),
       "LayoutType", StringValue ("RowFirst"));

  //1 : RandomDirection2d
  //2 : RandomWalk2d
  //3 : ConstantPosition
  switch(mobMod)
  {
    case 1: 
      std::cout << "RandomDirection2d" << "\t";
      mobility.SetMobilityModel ("ns3::RandomDirection2dMobilityModel", "Bounds", RectangleValue (Rectangle (-10, 10, -10, 10)), "Speed", 
	  	StringValue ("ns3::ConstantRandomVariable[Constant=3]"), "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=0.4]"));
      break;
    case 2: 
      std::cout << "RandomWalk2d" << "\t";
      mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                "Bounds", RectangleValue (Rectangle (-1000, 1000, -1000, 1000)), "Distance", ns3::DoubleValue (300.0));
      break;
    case 3: 
      std::cout << "ConstantPosition" << "\t";
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      break;
    default: 
      std::cout << "RandomWalk2d" << "\t";
      break;
  }

  mobility.Install (wifiStaNodes);

  // Configuring mobility of AP device
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);
  //std::cout << "Mobility of nodes configured" << std::endl;
  NS_LOG_INFO("Mobility of nodes configured");

  // Adding internet stack
  InternetStackHelper stack;
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  // Configuring IP addresses
  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer Stainterfaces = address.Assign (staDevices);
  Ipv4InterfaceContainer Apinterfaces = address.Assign (apDevices);

  //for(unsigned int i = 0; i < nWifi; i++)
  //{  
  //  std::cout << "Node" << i+1 << "IP Address = " << Stainterfaces.GetAddress(i) << std::endl;
  //}

  //std::cout << "IP Address of AP = " << Apinterfaces.GetAddress(0) << std::endl;
  //std::cout << "Internet Stack and IP addresses are configured" << std::endl;

  // Creating traffic generator
  ApplicationContainer serverApps;
  UdpServerHelper udpServer (4001); //port 4001
  serverApps = udpServer.Install (wifiStaNodes.Get (0));
  serverApps.Start (Seconds(startTime));
  serverApps.Stop (Seconds(stopTime));

  UdpClientHelper udpClient (Apinterfaces.GetAddress (0), 4001); //port 4001
  udpClient.SetAttribute ("MaxPackets", UintegerValue (maxPacket)); 
  udpClient.SetAttribute ("Interval", TimeValue (Time ("0.002"))); //packets
  udpClient.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  ApplicationContainer clientApps = udpClient.Install (wifiStaNodes.Get (0)); 
  clientApps.Start (Seconds(startTime));
  clientApps.Stop (Seconds(stopTime+5));
  //std::cout << "UDP traffic is generated" << std::endl;
  NS_LOG_INFO("UDP traffic is generated");

  // Using FlowMonitor to measure throughput and delay
  FlowMonitorHelper fMonitor;
  Ptr<FlowMonitor> monitorPtr = fMonitor.InstallAll();

  Simulator::Stop (Seconds(stopTime+2));
  Simulator::Run ();

  monitorPtr->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> flowClassifier = DynamicCast<Ipv4FlowClassifier> (fMonitor.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitorPtr->GetFlowStats ();

  double intrThrghpt = 0.0;
  Time delay;
  double timeDiff = 0.0;
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = flowStats.begin (); i != flowStats.end (); ++i)
  {
    intrThrghpt = 0.0;
    //Ipv4FlowClassifier::FiveTuple tuple = flowClassifier->FindFlow (i->first);
    //std::cout << "Flow " << i->first  << " (" << tuple.sourceAddress << " -> " << tuple.destinationAddress << ")\n";
    //std::cout << i->second.txBytes << "\t";
    //std::cout << i->second.rxBytes << "\t";

    timeDiff = i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
    if (timeDiff > 0)
      intrThrghpt = i->second.rxBytes  *  8.0  / timeDiff / 1024 / nWifi;
    
    std::cout << intrThrghpt  << "\t";

    delay = i->second.delaySum / i->second.rxPackets;
    std::cout << delay.GetMilliSeconds() << "\n";
  }

  if (tracing == true)
  {
    phy.EnablePcap ("third", apDevices.Get (0));
  }

  Simulator::Destroy ();
  return 0;
}
