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
    public Vector3 lastGivenPos;
    public Vector3 mostRecentPos;
}


public class Client : MonoBehaviour
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
    private int connectionId;
    [SerializeField]
    private int serverCnnId;
    [SerializeField]
    private float connectionTime;

    [SerializeField]
    private bool isConnected = false;
    [SerializeField]
    private bool isStarted = false;

    [SerializeField]
    private byte error;
    [SerializeField]
    private string playerName;

    public Ping pinger;
    public float smoothingSpeed;

    public GameObject playerPrefab;
    [SerializeField]
    private List<Player> localPlayerList = new List<Player>(MAX_CONNECTION);
    private Dictionary<int, Player> localPlayerMap = new Dictionary<int, Player>();

    private float lastMovementUpdate;
    public float movementUpdateRate = 0.1f;

    public GameObject puck;
    public Vector3 puckLastGivenPos;
    public Vector3 puckMostRecentPos;

    private BoxCollider divider; //assign on Start?
    private bool puckActive;

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
        if(localPlayerMap.ContainsKey(cnnId) == false)
        {
            Player player = new Player();
            player.playerObject = Instantiate(playerPrefab, GameObject.Find("P1Spawn").transform);
            player.playerObject.name = "Player [cnnId = " + cnnId + "]";
            player.playerObject.transform.parent = null;
            player.lastGivenPos = player.playerObject.transform.GetChild(0).position;
            player.mostRecentPos = player.playerObject.transform.GetChild(0).position;

            if(cnnId == serverCnnId)
            {
                player.playerObject.transform.GetChild(0).GetComponent<CharacterController>().enabled = true;
                player.playerObject.transform.GetChild(0).GetComponent<PlayerMovement>().inputAllowed = true;
                //player.playerObject.transform.GetChild(0).GetComponent<PlayerMovement>().playerCamera.enabled = true;
                isStarted = true;
            }

            localPlayerMap.Add(cnnId, player);

            if (serverCnnId == 1)
            {
                puckActive = true; //puck in control
                divider = GameObject.Find("Divider").GetComponent<BoxCollider>();
            }
            else
            {
                puckActive = false; //p1 spawns with puck...
                divider = GameObject.Find("Divider (1)").GetComponent<BoxCollider>();
            }

            Debug.Log("Spawned " + player.playerObject.name);
        }
        else
        {
            Debug.Log("Repeat ID " + cnnId + " detected, not spawning player");
        }
    }

    private void DeletePlayer(int cnnId)
    {
        if (localPlayerMap.ContainsKey(cnnId) == false)
        {
            Debug.Log("Deletion of " + cnnId + " failed, does not exist in map");
            return;
        }
        
        Destroy(localPlayerMap[cnnId].playerObject);
        localPlayerMap.Remove(cnnId);

    }

    private void JoinGame(int cnnId, string[] splitData) //for when the client joins the game
    {
        serverCnnId = cnnId;
        for (int i = 2; i < splitData.Length; i++) //2 so it skips ID and the connecting player's ID
        {
            string[] temp = splitData[i].Split('%'); //formatted as "<name>%<ID>, so temp[1] = ID
            //we're not using usernames right now, do it later id's are all we need for now
            SpawnPlayer(int.Parse(temp[1]));
        }
        //SpawnPlayer(cnnId);
           
        
    }

    private void SendPositionData()
    {
        Vector3 pos = localPlayerMap[serverCnnId].playerObject.transform.GetChild(0).position;
        char[] charPos = ("Transform" + "|" + serverCnnId + "|" + pos.x.ToString("F2") + "|" + pos.y.ToString("F2") + "|" + pos.z.ToString("F2")).ToCharArray();
        byte[] charBytes = Encoding.Unicode.GetBytes(charPos);
        NetworkTransport.Send(hostId, connectionId, unreliableChannel, charBytes, charPos.Length * sizeof(char), out error);
    }

    private void ReceivePositionData(string[] splitData) //Transform|<cnnId>|<xPos>|<yPos>|<zPos>
    {
        int cnnId = int.Parse(splitData[1]);
        if(cnnId == serverCnnId) { return; } //Dont update this client's position

        Vector3 newPos = new Vector3(float.Parse(splitData[2]),
            float.Parse(splitData[3]),
            float.Parse(splitData[4]));
        //localPlayerList[cnnId].playerObject.transform.position = newPos;
        localPlayerMap[cnnId].lastGivenPos = localPlayerMap[cnnId].playerObject.transform.GetChild(0).position;
        localPlayerMap[cnnId].mostRecentPos = newPos;

    }
    private void LerpPlayerPosition()
    {
        foreach(KeyValuePair<int, Player> p in localPlayerMap)
        {
            if(Vector3.Distance(p.Value.playerObject.transform.GetChild(0).position, p.Value.mostRecentPos) > 0.1f)
            {
                p.Value.playerObject.transform.GetChild(0).position = Vector3.Lerp(p.Value.lastGivenPos, p.Value.mostRecentPos, ((Time.time - lastMovementUpdate) / movementUpdateRate) * smoothingSpeed);
            }

        }
    }

    //PUCK OWNERSHIP
    private void CheckPuckStatus()
    {
        //https://answers.unity.com/questions/986235/is-it-possible-to-check-collision-from-another-obj.html
        //layer 10 has been set to puck layer

        if (true == true) //puck touched!!
            SendPuckSwitch();
        else
        { }
            //nothing
    }

    private void SendPuckSwitch() //publically called when divider triggers
    {
        if (puckActive == true)
        {
            //nothing
        }
        else 
        {
            bool sendStatus = puckActive; //switch puck physics ownership to our side
            puckActive = true;
            //turn bool into data
            //NetworkTransport.Send(hostId, connectionId, unreliableChannel, charBytes, charPos.Length * sizeof(char), out error); //send
        }
    }

    private void ReceivePuckSwitch(string[] splitData)
    {
        int cnnId = int.Parse(splitData[1]);
        if (cnnId == serverCnnId) { return; } //Dont update this client's position

        Vector3 newPos = new Vector3(float.Parse(splitData[2]),
            float.Parse(splitData[3]),
            float.Parse(splitData[4]));
        //localPlayerList[cnnId].playerObject.transform.position = newPos;
        puckLastGivenPos = puck.transform.position;
        puckMostRecentPos = newPos;

        //puckActive = !newStatus; //switch puck physics ownership to other side
    }

    //PUCK MOVEMENT
    private void PuckLoop()
    {
        if (puckActive == true)
        {
            SendPuckData();
        }
        else
        {
            //ReceivePuckData(); nothing?
        }
    }

    //Puck functions are experimental
    private void SendPuckData()
    {
        Vector3 pos = puck.transform.position;
        char[] charPos = ("PTransform" + "|" + serverCnnId + "|" + pos.x.ToString("F2") + "|" + pos.y.ToString("F2") + "|" + pos.z.ToString("F2")).ToCharArray();
        byte[] charBytes = Encoding.Unicode.GetBytes(charPos);
        NetworkTransport.Send(hostId, connectionId, unreliableChannel, charBytes, charPos.Length * sizeof(char), out error);
    }

    private void ReceivePuckData(string[] splitData) //Transform|<cnnId>|<xPos>|<yPos>|<zPos>
    {
        int cnnId = int.Parse(splitData[1]);
        if (cnnId == serverCnnId) { return; } //Dont update this client's position

        Vector3 newPos = new Vector3(float.Parse(splitData[2]),
            float.Parse(splitData[3]),
            float.Parse(splitData[4]));
        //localPlayerList[cnnId].playerObject.transform.position = newPos;
        puckLastGivenPos = puck.transform.position;
        puckMostRecentPos = newPos;
    }

    private void LerpPuckPosition()
    {
        if (Vector3.Distance(puck.transform.position, puckMostRecentPos) > 0.1f && serverCnnId != 1)
        {
            puck.transform.position = Vector3.Lerp(puckLastGivenPos, puckMostRecentPos, ((Time.time - lastMovementUpdate) / movementUpdateRate) * smoothingSpeed);
        }
    }

    //
    private void Compress()
    { 
        //to do
    }


    private void Start()
    {
        //for(int i = 0; i < MAX_CONNECTION; i++)
        //localPlayerList.Add(null);

        pinger = new Ping("127.0.0.1");

    }

    private void Update()
    {
        GameObject.Find("ID").GetComponent<Text>().text = serverCnnId.ToString();
    }
    // Update is called once per frame
    void FixedUpdate()
    {
        //copied from unity documentation
        if (!isConnected)
            return;

        

        int recHostId;
        int outConnectionId;
        int channelId;
        byte[] recBuffer = new byte[1024];
        int bufferSize = 1024;
        int dataSize;
        byte error;
        
        NetworkEventType recData = NetworkTransport.Receive(out recHostId, out outConnectionId, out channelId, recBuffer, bufferSize, out dataSize, out error);
        switch (recData)
        {
            case NetworkEventType.DataEvent:
                {
                    string msg = Encoding.Unicode.GetString(recBuffer, 0, dataSize);
                    Debug.Log("Receiving : " + msg);
                    string[] splitData = msg.Split('|');
                    switch(splitData[0]) //Change these to network event types, this is wasting data
                    {
                        case "ID":
                            {
                                JoinGame(int.Parse(splitData[1]), splitData);
                                break;
                            }
                        case "DC":
                            {
                                DeletePlayer(int.Parse(splitData[1]));
                                break;
                            }
                        case "Transform":
                            {
                                ReceivePositionData(splitData);
                                break;
                            }
                        case "Join":
                            {
                                SpawnPlayer(int.Parse(splitData[1]));
                                break;
                            }
                        case "PTransform":
                            {
                                ReceivePuckData(splitData);
                                break;
                            }
                        case "PChangeSide":
                            {
                                ReceivePuckSwitch(splitData);
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
                {

                break;
                }
            case NetworkEventType.ConnectEvent:
                {
                    Debug.Log("Connected!:" + outConnectionId);
                    break;
                }
        }



        //if(pinger.isDone)
        //{
        //    GameObject.Find("Ping").GetComponent<Text>().text = "Ping: " + pinger.time;
        //    pinger = new Ping(pinger.ip);
        //}

        if (Time.time - lastMovementUpdate > movementUpdateRate)
        {
            lastMovementUpdate = Time.time;
            if (isStarted)
                SendPositionData();
            //SendPuckData();
            CheckPuckStatus();
            PuckLoop();
        }
        else
        {
            LerpPlayerPosition();
            LerpPuckPosition();
        }
    }
}
#pragma warning disable CS0618 // Type or member is obsolete
