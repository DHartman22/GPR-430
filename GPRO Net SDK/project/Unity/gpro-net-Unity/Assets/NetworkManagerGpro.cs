using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Mirror
{
    public class NetworkManagerGpro : NetworkManager
    {
        public Transform playerSpawn1;
        public Transform playerSpawn2;
        
        public override void OnServerAddPlayer(NetworkConnection conn)
        {
            GameObject newPlayer;
            if (numPlayers == 0)
            {
                //newPlayer = Instantiate(playerPrefab, playerSpawn1);
            }
            else
            {
                //newPlayer = Instantiate(playerPrefab, playerSpawn2);
            }
            
            base.OnServerAddPlayer(conn);
            
        }


        // Start is called before the first frame update
        void Start()
        {
        
        }

        // Update is called once per frame
        void Update()
        {
        
        }
    }
}
