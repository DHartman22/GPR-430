using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using System.Text;
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

    private int hostId;
    private int webHostId;
    private int reliableChannel;
    private int unreliableChannel;

    private bool isStarted = false;
    private byte error;

    private List<ServerClient> clients = new List<ServerClient>();
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
    void Update()
    {
        if (!isStarted)
            return;

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
                    Debug.Log("Player " + connectionId + " has sent : " + msg);
                    break;
                }
            case NetworkEventType.DisconnectEvent:
                {
                    Debug.Log("Player " + connectionId + " has disconnected");
                    break;
                }

            case NetworkEventType.BroadcastEvent:
                {
                    Debug.Log("Player " + connectionId + " has broadcasted");
                    break;
                }
        }
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
        
    }

    private void Send(string message, int channelId, int cnnId)
    {
        List<ServerClient> c = new List<ServerClient>();
        c.Add(clients.Find(x => x.connectionId == cnnId));
        Send(message, channelId, c);
        byte[] msg = Encoding.Unicode.GetBytes(message);

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
}

#pragma warning restore CS0618 // Type or member is obsolete