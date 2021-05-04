using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using System.Text;
using UnityEngine.UI;
#pragma warning disable CS0618 // Type or member is obsolete

public class ServerClient
{
    public int connectionId;
    public string playerName;
};

public class Server : MonoBehaviour
{
    //Much of the basic server and client architecture is lifted from this tutorial:
    //https://youtu.be/qGkkaNkq8co 

    private const int MAX_CONNECTION = 100;

    private int port = 7777;

    [SerializeField]
    private int hostId;
    [SerializeField]
    private int webHostId;
    [SerializeField]
    private int reliableChannel;
    [SerializeField]
    private int unreliableChannel;

    [SerializeField]
    private bool isStarted = false;
    [SerializeField]
    private byte error;

    [SerializeField]
    public List<ServerClient> clients = new List<ServerClient>();

    public int player1Score;
    public int player2Score;



    // Start is called before the first frame update
    void Start()
    {
        NetworkTransport.Init();
        ConnectionConfig cc = new ConnectionConfig();

        reliableChannel = cc.AddChannel(QosType.Reliable);
        unreliableChannel = cc.AddChannel(QosType.Unreliable);

        HostTopology topo = new HostTopology(cc, MAX_CONNECTION);

        hostId = NetworkTransport.AddHost(topo, port, null);
        webHostId = NetworkTransport.AddWebsocketHost(topo, port, null);

        isStarted = true;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (!isStarted)
            return;

        ServerList();

        int recHostId;
        int connectionId;
        int channelId;
        byte[] recBuffer = new byte[1024];
        int bufferSize = 1024;
        int dataSize;
        byte error;
        NetworkEventType recData = NetworkTransport.Receive(out recHostId, out connectionId, out channelId, recBuffer, bufferSize, out dataSize, out error);
        switch (recData)
        {
            case NetworkEventType.ConnectEvent:
                {
                    OnConnection(connectionId);
                    Debug.Log("Player " + connectionId + " has connected");
                    break;
                }
            case NetworkEventType.DataEvent:
                {
                    string msg = Encoding.Unicode.GetString(recBuffer, 0, dataSize);
                    Debug.Log("Player " + connectionId + " has sent : " + msg + " of size: " + dataSize);

                    string[] splitData = msg.Split('|');

                    switch (splitData[0])
                    {
                        case "Transform":
                            {
                                Send(msg, channelId, clients);
                                break;
                            }
                        case "PTransform":
                            {
                                Send(msg, channelId, clients);
                                break;
                            }
                        case "PChangeSide": //is this correct?
                            {
                                //Send(msg, channelId, clients, connectionId);
                                break;
                            }
                        case "GameOver": //is this correct?
                            {
                                Send(msg, channelId, clients, connectionId);
                                break;
                            }
                        case "P1":
                            {
                                Send(msg, channelId, clients, connectionId);
                                player1Score++;
                                break;
                            }
                        case "P2":
                            {
                                Send(msg, channelId, clients, connectionId);
                                player2Score++;
                                break;
                            }
                    }
                    break;
                }
            case NetworkEventType.DisconnectEvent:
                {
                    Debug.Log("Player " + connectionId + " has disconnected");
                    OnDisconnect(connectionId);
                    break;
                }

            case NetworkEventType.BroadcastEvent:
                {
                    Debug.Log("Player " + connectionId + " has broadcasted");
                    break;
                }
                
        }
        
    }


    private void ServerList()
    {
        string clientNames = "";
        foreach(ServerClient sc in clients)
        {
            clientNames.Insert(clientNames.Length, sc.connectionId.ToString());
        }
        GameObject.Find("Text").GetComponent<Text>().text = clientNames;
    }



    private void OnConnection(int cnnId)
    {
        //add to player list
        ServerClient c = new ServerClient();
        c.connectionId = cnnId;
        c.playerName = "Player";
        
        clients.Add(c);
        string msg = "ID|" + cnnId + "|";
        foreach (ServerClient sc in clients)
        {
            msg += sc.playerName + '%' + sc.connectionId + '|';
        }
        msg = msg.Trim('|');
        //ID|3|temp%1|temp%2|temp%3
        Send(msg, reliableChannel, cnnId);

        //notify other players of join
        string joinMsg = "Join" + "|" + cnnId;
        Send(joinMsg, reliableChannel, clients, cnnId); //exclude the client connecting to avoid spawning an extra player

    }

    private void OnDisconnect(int cnnId)
    {
        ServerClient tempPlayer = new ServerClient();
        tempPlayer.connectionId = cnnId;
        tempPlayer.playerName = "Player";
        foreach(ServerClient sc in clients)
        {
            if(sc.connectionId == cnnId)
            {
                clients.Remove(sc);
                string message = "DC" + "|" + cnnId;
                Send(message, reliableChannel, clients);
                return;
            }
        }
        Debug.Log("OnDisconnect: cnnId " + cnnId + " not found");
        return;
    }

    private void TransformUpdate(string position, int cnnId)
    {
        Send(position, unreliableChannel, clients);
    }

    private void Send(string message, int channelId, int cnnId)
    {
        List<ServerClient> c = new List<ServerClient>();
        c.Add(clients.Find(x => x.connectionId == cnnId));
        Send(message, channelId, c);
        //byte[] msg = Encoding.Unicode.GetBytes(message);

    }
    private void Send(string message, int channelId, List<ServerClient> c)
    {
        Debug.Log("Sending : " + message);
        byte[] msg = Encoding.Unicode.GetBytes(message);
        foreach (ServerClient sc in c)
        {
            NetworkTransport.Send(hostId, sc.connectionId, channelId, msg, message.Length * sizeof(char), out error);
        }

    }

    private void Send(string message, int channelId, List<ServerClient> c, int exclude)
    {
        Debug.Log("Sending : " + message);
        byte[] msg = Encoding.Unicode.GetBytes(message);
        foreach (ServerClient sc in c)
        {
            if(sc.connectionId != exclude)
            NetworkTransport.Send(hostId, sc.connectionId, channelId, msg, message.Length * sizeof(char), out error);
        }

    }

}

#pragma warning restore CS0618 // Type or member is obsolete