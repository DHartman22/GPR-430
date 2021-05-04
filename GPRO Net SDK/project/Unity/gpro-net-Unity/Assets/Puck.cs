using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Puck : MonoBehaviour
{
    public Vector3 vel;

    private Rigidbody rgd;
    public float maxSpeed;
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.layer == 11)
        {
            GetComponent<Rigidbody>().velocity *= 1.2f;
        }
    }
    // Start is called before the first frame update
    void Start()
    {
        rgd = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        //rgd.velocity = Vector3.ClampMagnitude(rgd.velocity, maxSpeed);
        if(rgd.velocity.x > maxSpeed)
        {
            
            
            rgd.velocity = new Vector3(maxSpeed, 0f, rgd.velocity.z);
            Debug.Log("Slowing X");
        }

        if (rgd.velocity.x < maxSpeed * -1)
        {
            rgd.velocity = new Vector3(maxSpeed * -1, 0f, rgd.velocity.z);
            Debug.Log("Slowing X");
        }

        if (rgd.velocity.z > maxSpeed)
        {
            rgd.velocity = new Vector3(rgd.velocity.x, 0f, maxSpeed);
            Debug.Log("Slowing Z");
        }

        if (rgd.velocity.z < maxSpeed * -1)
        {
            rgd.velocity = new Vector3(rgd.velocity.x, 0f, maxSpeed);
            Debug.Log("Slowing Z");
        }
        vel = rgd.velocity;
    }
}
