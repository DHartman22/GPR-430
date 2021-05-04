using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PuckScript : MonoBehaviour
{
    public GameObject client;
    public int framesSinceLastTouch;
    public int frameCount = 0;
    // Start is called before the first frame update
    void Start()
    {
    framesSinceLastTouch = 0;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void FixedUpdate()
    {
        framesSinceLastTouch++;
    }

    //Upon collision with another GameObject, this GameObject will reverse direction
    private void OnTriggerExit(Collider other)
    {
        if (other.gameObject.layer == 10) //puck layer
        {
            if (client.GetComponent<Client>().puck.GetComponent<Rigidbody>().velocity.x > 0)
            {
                client.GetComponent<Client>().predictionPos += new Vector3(-6.0f, 0.0f, 0.0f);
            }
            else
            {
                client.GetComponent<Client>().predictionPos += new Vector3(6.0f, 0.0f, 0.0f);

            }

            //if (client.GetComponent<Client>().puckActive == true)
            //    client.GetComponent<Client>().puckActive = false;
            //else
            //    client.GetComponent<Client>().puckActive = true; //trigger switch of ownership in client

            client.GetComponent<Client>().puck.GetComponent<Rigidbody>().velocity = client.GetComponent<Client>().puckVelocity;

        }
    }
}
