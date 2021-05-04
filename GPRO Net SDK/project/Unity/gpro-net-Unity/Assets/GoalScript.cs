using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GoalScript : MonoBehaviour
{
    public GameObject client;

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
        if (other.layer == 10) //puck layer
        {
            client.GetComponent<Client>().score++; //trigger switch of ownership in client
        }
    }
}
