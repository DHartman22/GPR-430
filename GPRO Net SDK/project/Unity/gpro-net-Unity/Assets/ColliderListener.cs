using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColliderListener : MonoBehaviour
{
    public MeshCollider coll;

    void Awake()
    {
        //// Check if Colider is in another GameObject
        //Collider2D collider = GetComponentInChildren<Collider2D>();
        //if (collider.gameObject != gameObject)
        //{
        //    ColliderBridge cb = collider.gameObject.AddComponent<ColliderBridge>();
        //    cb.Initialize(this);
        //}

        ColliderBridge cb = GetComponent<Collider>().gameObject.AddComponent<ColliderBridge>();
        cb.Initialize(this);
    }
    public void OnCollisionEnter(Collision other)
    {
        //nothing, here just in case
    }

    public void OnTriggerEnter(Collider other)
    {
        // Do your stuff here
    }
}
