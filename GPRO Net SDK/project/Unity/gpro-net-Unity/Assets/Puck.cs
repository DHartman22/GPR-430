using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Puck : MonoBehaviour
{
    private Rigidbody rgd;
    public float maxSpeed;
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.layer == 11)
        {
            //GetComponent<Rigidbody>().velocity *= 1.2f;
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
        //Debug.Log(rgd.velocity.magnitude);
        Vector3.ClampMagnitude(rgd.velocity, maxSpeed);
    }
}
