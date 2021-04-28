using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;
#pragma warning disable CS0618 // Type or member is obsolete

enum NetworkingEventCustom
{
    TransformEvent = NetworkEventType.BroadcastEvent+1,

}

public class Player
{
    public GameObject playerObject;
    public int connectionId;
}


public class Client : MonoBehaviour
{
    //Much of the basic server and client architecture is lifted from this tutorial:
    //https://youtu.be/qGkkaNkq8co 

    private const int MAX_CONNECTION = 100;

    private int port = 7777;

    private int hostId;
    private int webHostId;
    private int reliableChannel;
    private int unreliableChannel;

    private int connectionId;
    private float connectionTime;

    private bool isConnected = false;
    private bool isStarted = false;

    private byte error;
    private string playerName;


    public GameObject playerPrefab;
    private List<Player> localPlayerList = new List<Player>(MAX_CONNECTION);
    
    // Start is called before the first frame update
    public void Connect()
    {
        //string name = GameObject.Find("InputField").GetComponent<InputField>().text;

        //if(name == "")
        //{
        //    Debug.Log("Enter a name");
        //    return;
        //}

        NetworkTransport.Init();
        ConnectionConfig cc = new ConnectionConfig();

        reliableChannel = cc.AddChannel(QosType.Reliable);
        unreliableChannel = cc.AddChannel(QosType.Unreliable);

        HostTopology topo = new HostTopology(cc, MAX_CONNECTION);

        hostId = NetworkTransport.AddHost(topo, 0);
        connectionId = NetworkTransport.Connect(hostId, "127.0.0.1", port, 0, out error);

        connectionTime = Time.time;
        isConnected = true;
    }

    private void SpawnPlayer(int cnnId)
    {
        if(localPlayerList[cnnId] == null)
        {
            Player player = new Player();
            player.playerObject = Instantiate(playerPrefab, GameObject.Find("P1Spawn").transform);
            player.playerObject.name = "Player [cnnId = " + cnnId + "]";
            player.playerObject.transform.parent = null;

            localPlayerList.Insert(cnnId, player);
            if(cnnId == connectionId)
            {
                playerPrefab.GetComponent<PlayerMovement>().inputAllowed = true;
                playerPrefab.GetComponent<PlayerMovement>().playerCamera.enabled = true;
            }
            Debug.Log("Spawned " + player.playerObject.name);
        }
        else
        {
            Debug.Log("Repeat ID " + cnnId + " detected, not spawning player");
        }
    }

    private void JoinGame(int cnnId, string[] splitData) //for when the client joins the game
    {
        for(int i = 2; i < splitData.Length; i++) //2 so it skips ID and the connecting player's ID
        {
            string[] temp = splitData[i].Split('%'); //formatted as "<name>%<ID>, so temp[1] = ID
            //we're not using usernames right now, do it later id's are all we need for now
            SpawnPlayer(int.Parse(temp[1]));
        }
        //SpawnPlayer(cnnId);

        
    }

    private void Start()
    {
        for(int i = 0; i < MAX_CONNECTION; i++)
        localPlayerList.Add(null);

    }

    // Update is called once per frame
    void Update()
    {
        //copied from unity documentation
        if (!isConnected)
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
            case NetworkEventType.DataEvent:
                {
                    string msg = Encoding.Unicode.GetString(recBuffer, 0, dataSize);
                    Debug.Log("Receiving : " + msg);
                    string[] splitData = msg.Split('|');

                    switch(splitData[0])
                    {
                        case "ID":
                            {

                                JoinGame(int.Parse(splitData[1]), splitData);
                                break;
                            }
                        case "PlayerDC":
                            {

                                break;
                            }
                    }
                    break;
                }
            case (NetworkEventType)NetworkingEventCustom.TransformEvent:
            {
                    
                    break;
            }
            case NetworkEventType.BroadcastEvent:

                break;
        }
    }
}
#pragma warning disable CS0618 // Type or member is obsolete
