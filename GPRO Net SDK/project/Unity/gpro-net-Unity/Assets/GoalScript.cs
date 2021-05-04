using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GoalScript : MonoBehaviour
{
    public GameObject client;
    public bool isP1Goal;
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }

    //Upon collision with another GameObject, this GameObject will reverse direction
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.layer == 10) //puck layer
        {
            if(isP1Goal && client.GetComponent<Client>().serverCnnId == 2)
            client.GetComponent<Client>().Score(2); 

            if(isP1Goal == false && client.GetComponent<Client>().serverCnnId == 1)
            client.GetComponent<Client>().Score(1); 

        }
    }
}
